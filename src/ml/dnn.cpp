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
  _dW.push_back(Eigen::MatrixXf());
  _dB.push_back(Eigen::MatrixXf());
  _X.push_back(Eigen::MatrixXf());

  return *this;
}

/*// Function: layer
DnnClassifier& DnnClassifier::layer(DnnLayer l) {

  _L.push_back(l);
  
  if(_L.size() > 1) {
    auto l = _L.size() - 1;
    _W.push_back (Eigen::MatrixXf::Random(_L[l-1], _L[l]));
    _B.push_back (Eigen::MatrixXf::Random(1, _L[l]));
    _dW.push_back(Eigen::MatrixXf());
    _dB.push_back(Eigen::MatrixXf());
  }

  _X.push_back(Eigen::MatrixXf());

  return *this;
} */

// Fucntion: error
DnnClassifier& DnnClassifier::error(Loss loss) {
  _loss = loss;
  return *this;
}

// Function: infer
Eigen::VectorXi DnnClassifier::infer(const Eigen::MatrixXf& data) const {

  Eigen::MatrixXf res = data;

  for(size_t l=1; l<_L.size(); ++l) {
    res = res * _W[l-1] + _B[l-1].replicate(data.rows(), 1);
    _act(res, _L[l].activation);
  }

  // Loss layer
  if(_loss == Loss::SOFTMAX_CROSS_ENTROPY) {
    const auto l = _L.size() - 1;
    res = (res - res.rowwise().maxCoeff().replicate(1, _L[l])).array().exp().matrix();
    res = res.cwiseQuotient(res.rowwise().sum().replicate(1, _L[l]));
  }
    
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
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), _gen);
  D = p * D;
  L = p * L;
}

// Procedure: _fprop
void DnnClassifier::_fprop(const Eigen::MatrixXf& D) {
  
  // Internal layer.
  for(size_t l=0; l<_L.size(); ++l) {
    _X[l] = l == 0 ? D : _X[l-1] * _W[l-1] + _B[l-1].replicate(D.rows(), 1);
    _act(_X[l], _L[l].activation);
  }
  
  // Loss layer
  if(_loss == Loss::SOFTMAX_CROSS_ENTROPY) {
    const auto l = _L.size() - 1;
    // Here we minus the max for numeric stability.
    _X[l] = (_X[l] - _X[l].rowwise().maxCoeff().replicate(1, _L[l])).array().exp().matrix();
    _X[l] = _X[l].cwiseQuotient(_X[l].rowwise().sum().replicate(1, _L[l]));
  }
}

// Procedure: _bprop
void DnnClassifier::_bprop(Eigen::MatrixXf& delta) {
  for(size_t l=_L.size()-1; l>0; --l) {
    // Last layer
    if(l == _L.size()-1) {
      delta = delta.cwiseProduct(_dact(_X[l], _L[l].activation));
    }
    // Internal layer
    else {
      delta = (delta * _W[l].transpose()).cwiseProduct(_dact(_X[l], _L[l].activation));
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

  switch(_loss) {
    case Loss::MSE:
      for(int i=0; i<Dtr.rows(); ++i) {
        delta(i, Ltr(i, 0)) -= 1.0f;
      }
    break;

    case Loss::SOFTMAX_CROSS_ENTROPY:
      for(int i=0; i<Dtr.rows(); ++i) {
        delta(i, Ltr(i, 0)) -= 1.0f;
      }
    break;
  };

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

// ------------------------------------------------------------------------------------------------





};  // end of namespace dtc::ml -------------------------------------------------------------------
