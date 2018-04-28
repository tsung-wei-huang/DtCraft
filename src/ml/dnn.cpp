/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/ml/dnn.hpp>

namespace dtc::ml {

// ------------------------------------------------------------------------------------------------
// DnnClassifier
// ------------------------------------------------------------------------------------------------

// Function: infer
Eigen::VectorXi DnnClassifier::infer(const Eigen::MatrixXf& data) const {
  
  Eigen::MatrixXf res = data;

  for(const auto& layer : _layers) {
    res = std::visit([&] (auto&& l) { return l.infer(res); }, layer);
  }

  // Loss layer.
  std::visit(Functors{
    [&] (SoftmaxCrossEntropy&) {
      res = (res - res.rowwise().maxCoeff().replicate(1, res.cols())).array().exp().matrix();
      res = res.cwiseQuotient(res.rowwise().sum().replicate(1, res.cols()));
    },
    [&] (auto&&) {
    }
  }, _loss);

  Eigen::VectorXi label(res.rows());

  for(int k=0; k<label.rows(); ++k) {
    res.row(k).maxCoeff(&label(k));
  }
  
  return label;
}

// Procedure: _shuffle
void DnnClassifier::_shuffle(Eigen::MatrixXf& D, Eigen::VectorXi& L) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(D.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), this_thread::random_engine());
  D = p * D;
  L = p * L;
}

// Procedure: _optimize
void DnnClassifier::_optimize(const Eigen::MatrixXf& Dtr, const Eigen::VectorXi& Ltr, float lrate) {
  
  // Forward propagation
  for(size_t i=0; i<_layers.size(); ++i) {
    const Eigen::MatrixXf& X = (i == 0) ? Dtr : std::visit([&] (auto&& layer) { return layer.X; }, _layers[i-1]);
    std::visit([&] (auto&& layer) { layer.fprop(X); }, _layers[i]);
  }

  // Find the derivative at the last layer
  Eigen::MatrixXf delta = std::visit([] (auto&& layer) { return layer.X; }, _layers.back());
  
  std::visit(Functors{
    [&] (SoftmaxCrossEntropy&) {
      // Here we minus the max for numeric stability.
      delta = (delta - delta.rowwise().maxCoeff().replicate(1, delta.cols())).array().exp().matrix();
      delta = delta.cwiseQuotient(delta.rowwise().sum().replicate(1, delta.cols()));
    },
    [&] (auto&&) {
    }
  }, _loss);

  std::visit([&] (auto&& loss) mutable { loss.dloss(delta, Ltr); }, _loss);

  // Backward
  for(int i=_layers.size()-1; i>=0; --i) {
    const auto& X = (i == 0) ? Dtr : std::visit([&] (auto&& layer) { return layer.X; }, _layers[i-1]);
    std::visit([&] (auto&& layer) mutable { layer.bprop(X, delta); }, _layers[i]);
  }
  
  // Update the weight 
  _update(lrate / std::sqrt(Dtr.rows()));
}

// Procedure: _update
void DnnClassifier::_update(float rate) {
  
  for(auto& layer : _layers) {
    std::visit([&](auto&& l){ l.update(_optimizer, rate); }, layer);
  }
}

// Function: save
std::streamsize DnnClassifier::save(const std::filesystem::path& path) {

  auto dev = make_block_file(path, std::ios_base::out | std::ios_base::trunc);

  // Create an osbuf
  OutputStreamBuffer osbuf(dev.get());
  auto sz1 = BinaryOutputArchiver(osbuf)(*this);
  auto sz2 = osbuf.flush();

  if(sz1 != sz2 or osbuf.out_avail() != 0) {
    DTC_THROW("Failed to save DnnClassifier to ", path);
  }
  
  return sz1; 
}

// Function: load
std::streamsize DnnClassifier::load(const std::filesystem::path& path) {
  
  auto dev = make_block_file(path, std::ios_base::in);

  // Create an isbuf
  InputStreamBuffer isbuf(dev.get());
  auto sz1 = isbuf.purge();
  auto sz2 = BinaryInputArchiver(isbuf)(*this);

  if(sz1 != sz2 or isbuf.in_avail() != 0) {
    DTC_THROW("Failed to load DnnClassifier from ", path);
  }

  return sz1;
}

// ------------------------------------------------------------------------------------------------
// DnnRegressor
// ------------------------------------------------------------------------------------------------

// Function: infer
Eigen::MatrixXf DnnRegressor::infer(const Eigen::MatrixXf& data){
  
  Eigen::MatrixXf p = data;

  for(const auto& layer : _layers) {
    p = std::visit([&] (auto&& l) { return l.infer(p); }, layer);
  }

  return p;
}

// Procedure: _shuffle
void DnnRegressor::_shuffle(Eigen::MatrixXf& D, Eigen::MatrixXf& L) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(D.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), this_thread::random_engine());
  D = p * D;
  L = p * L;
}

// Procedure: _optimize
void DnnRegressor::_optimize(const Eigen::MatrixXf& Dtr, const Eigen::MatrixXf& Ltr, float lrate) {

  // Forward propagation
  for(size_t i=0; i<_layers.size(); ++i) {
    const Eigen::MatrixXf& X = (i == 0) ? Dtr : std::visit([&] (auto&& layer) { return layer.X; }, _layers[i-1]);
    std::visit([&] (auto&& layer) { layer.fprop(X); }, _layers[i]);
  }

  // Find the derivative at the last layer
  Eigen::MatrixXf delta = std::visit([] (auto&& layer) { return layer.X; }, _layers.back());

  std::visit([&] (auto&& loss) mutable { loss.dloss(delta, Ltr); }, _loss);

  // Backward
  for(int i=_layers.size()-1; i>=0; --i) {
    const auto& X = (i == 0) ? Dtr : std::visit([&] (auto&& layer) { return layer.X; }, _layers[i-1]);
    std::visit([&] (auto&& layer) mutable { layer.bprop(X, delta); }, _layers[i]);
  }
  
  // Update the weight 
  _update(lrate / std::sqrt(Dtr.rows()));
}

// Procedure: _update
void DnnRegressor::_update(float rate) {

  for(auto& layer : _layers) {
    std::visit([&](auto&& l){ l.update(_optimizer, rate); }, layer);
  }
}

// Function: save
std::streamsize DnnRegressor::save(const std::filesystem::path& path) {

  auto dev = make_block_file(path, std::ios_base::out | std::ios_base::trunc);

  // Create an osbuf
  OutputStreamBuffer osbuf(dev.get());
  auto sz1 = BinaryOutputArchiver(osbuf)(*this);
  auto sz2 = osbuf.flush();

  if(sz1 != sz2 or osbuf.out_avail() != 0) {
    DTC_THROW("Failed to save DnnRegressor to ", path);
  }
  
  return sz1; 
}

// Function: load
std::streamsize DnnRegressor::load(const std::filesystem::path& path) {
  
  auto dev = make_block_file(path, std::ios_base::in);

  // Create an isbuf
  InputStreamBuffer isbuf(dev.get());
  auto sz1 = isbuf.purge();
  auto sz2 = BinaryInputArchiver(isbuf)(*this);

  if(sz1 != sz2 or isbuf.in_avail() != 0) {
    DTC_THROW("Failed to load DnnRegressor from ", path);
  }

  return sz1;
}


};  // end of namespace dtc::ml -------------------------------------------------------------------





