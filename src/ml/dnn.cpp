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

// Function: __dtanh 
static Eigen::MatrixXf __dtanh(const Eigen::MatrixXf& x) {
  return 1.0f - x.array().square();
}

// Function: __dsigmoid
static Eigen::MatrixXf __dsigmoid(const Eigen::MatrixXf& x) {
  return x.array() * (1 - x.array());
}

// Function: __drelu
static Eigen::MatrixXf __drelu(const Eigen::MatrixXf& x) {
  Eigen::MatrixXf res(x.rows(), x.cols());
  for(int j=0; j<res.cols(); ++j) {
    for(int i=0; i<res.rows(); ++i) {
      res(i, j) = x(i, j) > 0.0f ? 1.0f : 0.0f;
    }
  }
  return res;
}

// Procedure: __tanh
static void __tanh(Eigen::MatrixXf& x) {
  x = (x.array().tanh()).matrix();
}

// Procedure: __sigmoid
static void __sigmoid(Eigen::MatrixXf& x) {
  x = ((1.0f + (-x).array().exp()).inverse()).matrix();
}

// Procedure: __relu
static void __relu(Eigen::MatrixXf& x) {
  for(int j=0; j<x.cols(); ++j) {
    for(int i=0; i<x.rows(); ++i) {
      if(x(i, j) <= 0.0f) {
        x(i, j) = 0.0f;
      }
    }
  }
}

// Procedure: __act
static void __act(Eigen::MatrixXf& x, Activation a) {

  switch(a) {
    case Activation::SIGMOID:
      __sigmoid(x);
    break;

    case Activation::TANH:
      __tanh(x);
    break;

    case Activation::RELU:
      __relu(x);
    break;

    default:
    break;
  }
}

// Function: __dact
static Eigen::MatrixXf __dact(const Eigen::MatrixXf& x, Activation a) {
  
  switch(a) {
    case Activation::SIGMOID:
      return __dsigmoid(x);
    break;

    case Activation::TANH:
      return __dtanh(x);
    break;

    case Activation::RELU:
      return __drelu(x);
    break;

    default:
      return Eigen::MatrixXf::Ones(x.rows(), x.cols());
    break;
  }
}

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
  _dW.push_back(Eigen::MatrixXf());
  _dB.push_back(Eigen::MatrixXf());
  _X.push_back(Eigen::MatrixXf());

  return *this;
}

// Fucntion: loss
DnnClassifier& DnnClassifier::loss(Loss loss) {
  _loss = loss;
  return *this;
}


// Function: infer
Eigen::VectorXi DnnClassifier::infer(const Eigen::MatrixXf& data) const {

  Eigen::MatrixXf res = data;

  for(size_t l=1; l<_L.size(); ++l) {
    res = res * _W[l-1] + _B[l-1].replicate(data.rows(), 1);
    __act(res, _L[l].activation);
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
    __act(_X[l], _L[l].activation);
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
      delta = delta.cwiseProduct(__dact(_X[l], _L[l].activation));
    }
    // Internal layer
    else {
      delta = (delta * _W[l].transpose()).cwiseProduct(__dact(_X[l], _L[l].activation));
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

// Function: batch_norm
DnnRegressor& DnnRegressor::batch_norm(bool on) {
  _batch_norm = on;
  return *this;
}

// Function: batch_norm
bool DnnRegressor::batch_norm() const {
  return _batch_norm;
}

// Function: fully_connected_layer
DnnRegressor& DnnRegressor::fully_connected_layer(size_t ni, size_t ni_1, Activation act) {

  if(_L.size() == 0) {
    _L.push_back({ni, Activation::NONE});        
    _X.push_back(Eigen::MatrixXf());
  }
  else if(ni != _L.back()){
    DTC_THROW("Dimension mismatched ", ni, "x", ni_1, " (dimension mismatch)");
  }

  _L.push_back({ni_1, act});
  _W.push_back(Eigen::MatrixXf::Random(ni, ni_1));
  _B.push_back(Eigen::MatrixXf::Random(1, ni_1));
  _dW.push_back(Eigen::MatrixXf());
  _dB.push_back(Eigen::MatrixXf());
  _X.push_back(Eigen::MatrixXf());

  // Batch norm field.
  _gamma.push_back(Eigen::MatrixXf::Ones(1, ni_1));
  _beta.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  _mean.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  _var.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  _isqrtvar.push_back(Eigen::MatrixXf::Ones(1, ni_1));
  _hhat.push_back(Eigen::MatrixXf());
  _dbeta.push_back(Eigen::MatrixXf());
  _dgamma.push_back(Eigen::MatrixXf());
  _rmean.push_back(Eigen::MatrixXf::Zero(1, ni_1));
  _rvar.push_back(Eigen::MatrixXf::Ones(1, ni_1));

  return *this;
}

//Batch Norm functions, level not including input level
void DnnRegressor::_batch_norm_fp(Eigen::MatrixXf& D, size_t level){

  _mean[level] = D.colwise().sum() / D.rows(); //mini batch mean
  _var[level] = (D- _mean[level].replicate(D.rows(), 1)).cwiseAbs2().colwise().sum() / D.rows(); 
  _isqrtvar[level] = (_var[level].array() + 1e-6f).matrix().cwiseInverse().cwiseSqrt(); 
  _hhat[level] = (D-_mean[level].replicate(D.rows(), 1)).cwiseProduct(_isqrtvar[level].replicate(D.rows(), 1));
  D = _hhat[level].cwiseProduct(_gamma[level].replicate(D.rows(), 1)) + _beta[level].replicate(D.rows(), 1);  
}

void DnnRegressor::_batch_norm_infer_fp(Eigen::MatrixXf& D, size_t level){

  //need for improvement
  Eigen::MatrixXf _risqrt_var = (_rvar[level].array() + 1e-6f).matrix().cwiseInverse().cwiseSqrt();
  Eigen::MatrixXf _hhat = (D- _rmean[level].replicate(D.rows(), 1)).cwiseProduct(_risqrt_var.replicate(D.rows(), 1));  
  D = _hhat.cwiseProduct(_gamma[level].replicate(D.rows(), 1)) + _beta[level].replicate(D.rows(), 1);  
}

void DnnRegressor::_batch_norm_bp(Eigen::MatrixXf& delta, size_t level){
  
  size_t bsize = delta.rows();
  _dgamma[level] = delta.cwiseProduct(_hhat[level]).colwise().sum(); 
  _dbeta[level] = delta.colwise().sum();
  delta = _isqrtvar[level].replicate(bsize, 1).cwiseProduct(_gamma[level].replicate(bsize, 1))
          .cwiseProduct(bsize*delta - _dbeta[level].replicate(bsize,1) - _hhat[level].cwiseProduct(_dgamma[level].replicate(bsize, 1)) )
           / delta.rows();

}

// Fucntion: loss
DnnRegressor& DnnRegressor::loss(Loss loss) {
  _loss = loss;
  return *this;
}

// Function: infer
Eigen::VectorXf DnnRegressor::infer(const Eigen::MatrixXf& data){

  Eigen::MatrixXf res = data;

  for(size_t l=1; l<_L.size(); ++l) {
    res = res * _W[l-1] + _B[l-1].replicate(data.rows(), 1);
    if(_batch_norm == true) {
      _batch_norm_infer_fp(res, l-1);
    }
    __act(res, _L[l].activation);
  }
  
  // Loss layer
  if(_loss == Loss::SOFTMAX_CROSS_ENTROPY) {
    const auto l = _L.size() - 1;
    res = (res - res.rowwise().maxCoeff().replicate(1, _L[l])).array().exp().matrix();
    res = res.cwiseQuotient(res.rowwise().sum().replicate(1, _L[l]));
  }

  //output layer are linear neurons 
  return res;
}

// Procedure: _shuffle
void DnnRegressor::_shuffle(Eigen::MatrixXf& D, Eigen::VectorXf& L) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(D.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), _gen);
  D = p * D;
  L = p * L;
}

// Procedure: _fprop
void DnnRegressor::_fprop(const Eigen::MatrixXf& D) {
  
  // Internal layer.
  for(size_t l=0; l<_L.size(); ++l) {
    _X[l] = l == 0 ? D : _X[l-1] * _W[l-1] + _B[l-1].replicate(D.rows(), 1); 
    if(l > 0 && _batch_norm == true) {
      _batch_norm_fp(_X[l], l-1); //batch norm forward
    }
    __act(_X[l], _L[l].activation);
  }
  
  // Loss layer
  switch(_loss){
    case Loss::MSE:
      //the Loss is _X[l]-y, which is computed in _optimize   
    break;

    case Loss::SOFTMAX_CROSS_ENTROPY:    
      const auto l = _L.size() - 1;
      // Here we minus the max for numeric stability.
      _X[l] = (_X[l] - _X[l].rowwise().maxCoeff().replicate(1, _L[l])).array().exp().matrix();
      _X[l] = _X[l].cwiseQuotient(_X[l].rowwise().sum().replicate(1, _L[l]));
    break;
  }
}

// Procedure: _bprop
void DnnRegressor::_bprop(Eigen::MatrixXf& delta) {
  for(size_t l=_L.size()-1; l>0; --l) {
    // Last layer
    if(l == _L.size()-1) {
      delta = delta.cwiseProduct(__dact(_X[l], _L[l].activation));
    }
    // Internal layer
    else {
      delta = (delta * _W[l].transpose()).cwiseProduct(__dact(_X[l], _L[l].activation));
    }

    if(_batch_norm == true) {
      _batch_norm_bp(delta, l-1); //batch norm backprop
    }
    
    _dB[l-1] = delta.colwise().sum();
    _dW[l-1] = _X[l-1].transpose() * delta;
  }
}

// Procedure: _optimize
void DnnRegressor::_optimize(const Eigen::MatrixXf& Dtr, const Eigen::VectorXf& Ltr, float lrate) {

  // Forward
  _fprop(Dtr);
  
  // Find the derivative at the last layer
  Eigen::MatrixXf delta = _X[_L.size()-1];

  switch(_loss) {
    case Loss::MSE:
      for(int i=0; i<Dtr.rows(); ++i) {
        delta(i, 0) -= Ltr(i, 0);
      }
    break;

    case Loss::SOFTMAX_CROSS_ENTROPY:
      for(int i=0; i<Dtr.rows(); ++i) {
        delta(i, 0) -= Ltr(i, 0);
      }
    break;
  };

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


  if(_batch_norm) {

    constexpr static int tag_gamma = 2;
    constexpr static int tag_beta = 3;
    
    std::visit([this, rate](auto&& opt){
      opt.alpha(rate).update(_dgamma, _gamma, tag_gamma);
      opt.alpha(rate).update(_dbeta, _beta, tag_beta);
    }, _optimizer);

    for(size_t i=0; i<_rmean.size(); i++){
      _rmean[i] = _rmean[i]*0.9f + _mean[i]*0.1f;
      _rvar[i] = _rvar[i]*0.9f + _var[i]*0.1f;
    }
  }
}


};  // end of namespace dtc::ml -------------------------------------------------------------------
