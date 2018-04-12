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

// Function: fully_connected_layer
DnnClassifier& DnnClassifier::fully_connected_layer(size_t ni, size_t ni_1, Activation act) {

  if(_L.size() == 0) {
    _L.push_back({ni, Activation::NONE});        
    _X.push_back(Eigen::MatrixXf());
  }
  else if(ni != _L.back()){
    DTC_THROW("Failed to add layer ", ni, "x", ni_1, " (dimension mismatch)");
  }

  _L.push_back({ni_1, act});
  _W.push_back(Eigen::MatrixXf::Random(ni, ni_1));
  _B.push_back(Eigen::MatrixXf::Random(1, ni_1));
  _dW.emplace_back(ni, ni_1);
  _dB.emplace_back(1, ni_1);
  _X.emplace_back();

  return *this;
}

// Function: infer
Eigen::VectorXi DnnClassifier::infer(const Eigen::MatrixXf& data) const {

  Eigen::MatrixXf res = data;

  for(size_t l=1; l<_L.size(); ++l) {
    res = res * _W[l-1] + _B[l-1].replicate(data.rows(), 1);
    activate(res, _L[l].activation);
  }

  std::visit(Functors{
    [&] (SoftmaxCrossEntropy&) {
      const auto l = _L.size() - 1;
      res = (res - res.rowwise().maxCoeff().replicate(1, _L[l])).array().exp().matrix();
      res = res.cwiseQuotient(res.rowwise().sum().replicate(1, _L[l]));
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

// Procedure: _fprop
void DnnClassifier::_fprop(const Eigen::MatrixXf& D) {
  
  // Internal layer.
  for(size_t l=0; l<_L.size(); ++l) {
    _X[l] = l == 0 ? D : _X[l-1] * _W[l-1] + _B[l-1].replicate(D.rows(), 1);
    activate(_X[l], _L[l].activation);
  }

  std::visit(Functors{
    [&] (SoftmaxCrossEntropy&) {
      const auto l = _L.size() - 1;
      // Here we minus the max for numeric stability.
      _X[l] = (_X[l] - _X[l].rowwise().maxCoeff().replicate(1, _L[l])).array().exp().matrix();
      _X[l] = _X[l].cwiseQuotient(_X[l].rowwise().sum().replicate(1, _L[l]));
    },
    [&] (auto&&) {
    }
  }, _loss);
}

// Procedure: _bprop
void DnnClassifier::_bprop(Eigen::MatrixXf& delta) {
  for(size_t l=_L.size()-1; l>0; --l) {
    // Last layer
    if(l == _L.size()-1) {
      delta = delta.cwiseProduct(dactivate(_X[l], _L[l].activation));
    }
    // Internal layer
    else {
      delta = (delta * _W[l].transpose()).cwiseProduct(dactivate(_X[l], _L[l].activation));
    }
    _dB[l-1] = delta.colwise().sum();
    _dW[l-1] = _X[l-1].transpose() * delta;
  }
}

// Procedure: _optimize
void DnnClassifier::_optimize(const Eigen::MatrixXf& Dtr, const Eigen::VectorXi& Ltr, float lrate) {

  // Forward
  _fprop(Dtr);
  
  // Find the derivative at the last layer
  Eigen::MatrixXf delta = _X[_L.size()-1];

  std::visit([&] (auto&& loss) mutable {
    loss.dloss(delta, Ltr);
  }, _loss);

  // Backward
  _bprop(delta);
  
  // Update the weight
  _update(lrate / std::sqrt(Dtr.rows()));
}

// Procedure: _update
void DnnClassifier::_update(float rate) {

  constexpr static int tag_w = 0;
  constexpr static int tag_b = 1;

  std::visit([this, rate](auto&& opt){
    opt.alpha(rate).update(_dW, _W, tag_w);
    opt.alpha(rate).update(_dB, _B, tag_b);
  }, _optimizer);

}

// Function: save
std::streamsize DnnClassifier::save(const std::filesystem::path& path) {

  auto dev = make_block_file(path, std::ios_base::out | std::ios_base::trunc);

  // Create an osbuf
  OutputStreamBuffer osbuf(dev.get());
  auto sz1 = BinaryOutputArchiver(osbuf)(*this);
  auto sz2 = osbuf.flush();

  if(sz1 != sz2 or osbuf.out_avail() != 0) {
    DTC_THROW("Failed to save the DNN model to ", path);
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
    DTC_THROW("Failed to load the DNN model from ", path);
  }

  return sz1;
}

// ------------------------------------------------------------------------------------------------
// DnnRegressor
// ------------------------------------------------------------------------------------------------

//// Function: batch_norm
//DnnRegressor& DnnRegressor::batch_norm(bool on) {
//  _batch_norm = on;
//  return *this;
//}
//
//// Function: batch_norm
//bool DnnRegressor::batch_norm() const {
//  return _batch_norm;
//}

// Function: fully_connected_layer
DnnRegressor& DnnRegressor::fully_connected_layer(size_t ni, size_t ni_1, Activation act) {

  if(_L.size() == 0) {
    _L.push_back({ni, Activation::NONE});        
    _X.push_back(Eigen::MatrixXf());
  }
  else if(ni != _L.back()){
    DTC_THROW("Layer ", ni, "x", ni_1, " doesn't match the previous layer");
  }

  _L.push_back({ni_1, act});
  _W.push_back(Eigen::MatrixXf::Random(ni, ni_1));
  _B.push_back(Eigen::MatrixXf::Random(1, ni_1));
  _dW.emplace_back();
  _dB.emplace_back();
  _X.emplace_back();

  // Batch norm field.
  //_gamma.push_back(Eigen::MatrixXf::Ones(1, ni_1));
  //_beta.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  //_mean.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  //_var.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  //_isqrtvar.push_back(Eigen::MatrixXf::Ones(1, ni_1));
  //_hhat.push_back(Eigen::MatrixXf());
  //_dbeta.push_back(Eigen::MatrixXf());
  //_dgamma.push_back(Eigen::MatrixXf());
  //_rmean.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  //_rvar.push_back(Eigen::MatrixXf::Ones(1, ni_1));

  return *this;
}

////Batch Norm functions, level not including input level
//void DnnRegressor::_batch_norm_fp(Eigen::MatrixXf& D, size_t level){
//
//  _mean[level] = D.colwise().sum() / D.rows(); //mini batch mean
//  _var[level] = (D- _mean[level].replicate(D.rows(), 1)).cwiseAbs2().colwise().sum() / D.rows(); 
//  _isqrtvar[level] = (_var[level].array() + 1e-6f).matrix().cwiseInverse().cwiseSqrt(); 
//  _hhat[level] = (D-_mean[level].replicate(D.rows(), 1)).cwiseProduct(_isqrtvar[level].replicate(D.rows(), 1));
//  D = _hhat[level].cwiseProduct(_gamma[level].replicate(D.rows(), 1)) + _beta[level].replicate(D.rows(), 1);  
//}
//
//void DnnRegressor::_batch_norm_infer_fp(Eigen::MatrixXf& D, size_t level){
//
//  //need for improvement
//  Eigen::MatrixXf _risqrt_var = (_rvar[level].array() + 1e-6f).matrix().cwiseInverse().cwiseSqrt();
//  Eigen::MatrixXf _hhat = (D- _rmean[level].replicate(D.rows(), 1)).cwiseProduct(_risqrt_var.replicate(D.rows(), 1));  
//  D = _hhat.cwiseProduct(_gamma[level].replicate(D.rows(), 1)) + _beta[level].replicate(D.rows(), 1);  
//}
//
//void DnnRegressor::_batch_norm_bp(Eigen::MatrixXf& delta, size_t level){
//  
//  size_t bsize = delta.rows();
//  _dgamma[level] = delta.cwiseProduct(_hhat[level]).colwise().sum(); 
//  _dbeta[level] = delta.colwise().sum();
//  delta = _isqrtvar[level].replicate(bsize, 1).cwiseProduct(_gamma[level].replicate(bsize, 1))
//          .cwiseProduct(bsize*delta - _dbeta[level].replicate(bsize,1) - _hhat[level].cwiseProduct(_dgamma[level].replicate(bsize, 1)) )
//           / delta.rows();
//
//}

// Function: infer
Eigen::MatrixXf DnnRegressor::infer(const Eigen::MatrixXf& data){

  Eigen::MatrixXf res = data;

  for(size_t l=1; l<_L.size(); ++l) {
    res = res * _W[l-1] + _B[l-1].replicate(data.rows(), 1);
    //if(_batch_norm == true) {
    //  _batch_norm_infer_fp(res, l-1);
    //}
    activate(res, _L[l].activation);
  }
  
  //output layer are linear neurons 
  return res;
}

// Procedure: _shuffle
void DnnRegressor::_shuffle(Eigen::MatrixXf& D, Eigen::MatrixXf& L) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(D.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), this_thread::random_engine());
  D = p * D;
  L = p * L;
}

// Procedure: _fprop
void DnnRegressor::_fprop(const Eigen::MatrixXf& D) {
  
  // Internal layer.
  for(size_t l=0; l<_L.size(); ++l) {
    _X[l] = l == 0 ? D : _X[l-1] * _W[l-1] + _B[l-1].replicate(D.rows(), 1); 
    //if(l > 0 && _batch_norm == true) {
    //  _batch_norm_fp(_X[l], l-1); //batch norm forward
    //}
    activate(_X[l], _L[l].activation);
  }
}

// Procedure: _bprop
void DnnRegressor::_bprop(Eigen::MatrixXf& delta) {
  for(size_t l=_L.size()-1; l>0; --l) {
    // Last layer
    if(l == _L.size()-1) {
      delta = delta.cwiseProduct(dactivate(_X[l], _L[l].activation));
    }
    // Internal layer
    else {
      delta = (delta * _W[l].transpose()).cwiseProduct(dactivate(_X[l], _L[l].activation));
    }

    //if(_batch_norm == true) {
    //  _batch_norm_bp(delta, l-1); //batch norm backprop
    //}
    
    _dB[l-1] = delta.colwise().sum();
    _dW[l-1] = _X[l-1].transpose() * delta;
  }
}

// Procedure: _optimize
void DnnRegressor::_optimize(const Eigen::MatrixXf& Dtr, const Eigen::MatrixXf& Ltr, float lrate) {

  // Forward
  _fprop(Dtr);
  
  // Find the derivative at the last layer
  Eigen::MatrixXf delta = _X[_L.size()-1];
  
  std::visit([&] (auto&& loss) mutable {
    loss.dloss(delta, Ltr);
  }, _loss);

  // Backward
  _bprop(delta);
  
  // Update the weight 
  _update(lrate / std::sqrt(Dtr.rows()));
}

// Procedure: _update
void DnnRegressor::_update(float rate) {

  constexpr static int tag_w = 0;
  constexpr static int tag_b = 1;

  std::visit([this, rate](auto&& opt){
    opt.alpha(rate).update(_dW, _W, tag_w);
    opt.alpha(rate).update(_dB, _B, tag_b);
  }, _optimizer);


  //if(_batch_norm) {

  //  constexpr static int tag_gamma = 2;
  //  constexpr static int tag_beta = 3;
  //  
  //  std::visit([this, rate](auto&& opt){
  //    opt.alpha(rate).update(_dgamma, _gamma, tag_gamma);
  //    opt.alpha(rate).update(_dbeta, _beta, tag_beta);
  //  }, _optimizer);

  //  for(size_t i=0; i<_rmean.size(); i++){
  //    _rmean[i] = _rmean[i]*0.9f + _mean[i]*0.1f;
  //    _rvar[i] = _rvar[i]*0.9f + _var[i]*0.1f;
  //  }
  //}
}

// Function: save
std::streamsize DnnRegressor::save(const std::filesystem::path& path) {

  auto dev = make_block_file(path, std::ios_base::out | std::ios_base::trunc);

  // Create an osbuf
  OutputStreamBuffer osbuf(dev.get());
  auto sz1 = BinaryOutputArchiver(osbuf)(*this);
  auto sz2 = osbuf.flush();

  if(sz1 != sz2 or osbuf.out_avail() != 0) {
    DTC_THROW("Failed to save the DNN model to ", path);
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
    DTC_THROW("Failed to load the DNN model from ", path);
  }

  return sz1;
}


};  // end of namespace dtc::ml -------------------------------------------------------------------





