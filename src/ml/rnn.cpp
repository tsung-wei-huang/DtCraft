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

#include <dtc/ml/rnn.hpp>

namespace dtc::ml {

//// Function: __clip
//static void __clip(Eigen::MatrixXf& x, float l, float u) {
//  for(Eigen::MatrixXf::Index c=0; c<x.cols(); ++c) {
//    for(Eigen::MatrixXf::Index r=0; r<x.rows(); ++r) {
//      x(r, c) = std::clamp(x(r, c), l, u);
//    }
//  }
//}

// ------------------------------------------------------------------------------------------------

// Function: cell
RnnRegressorNx1& RnnRegressorNx1::cell(size_t f, size_t h, size_t l, Activation act) {

  _activation = act;

  _Wx = Eigen::MatrixXf::Random(f, h);
  _Ws = Eigen::MatrixXf::Random(h, h);
  _Wy = Eigen::MatrixXf::Random(h, l);
  _Bx = Eigen::MatrixXf::Random(1, h);
  _By = Eigen::MatrixXf::Random(1, l);

  return *this;
}

// Procedure: _shuffle
void RnnRegressorNx1::_shuffle(Eigen::MatrixXf& D, Eigen::MatrixXf& L) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(D.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), this_thread::random_engine());
  D = p * D;
  L = p * L;
}

// Function: infer
Eigen::MatrixXf RnnRegressorNx1::infer(const Eigen::MatrixXf& data) {

  if(data.cols() % feature_columns() != 0) {
    DTC_THROW("Time dimension error (", data.cols(), "%", feature_columns(), "!=0)");;
  }

  size_t time_steps = data.cols() / feature_columns();

  Eigen::MatrixXf s = Eigen::MatrixXf::Zero(data.rows(), hidden_units());

  for(size_t t=1, beg=0; t<=time_steps; ++t) {

    s = data.middleCols(beg, feature_columns()) * _Wx + s * _Ws + _Bx.replicate(data.rows(), 1);
    activate(s, _activation);
    beg += feature_columns();
  }
  
  s = s * _Wy + _By.replicate(data.rows(), 1);

  // Loss layer?
  
  return s;
}

  
// Procedure: _optimize
void RnnRegressorNx1::_optimize(const Eigen::MatrixXf& Dtr, const Eigen::MatrixXf& Ltr, float lrate) {
  
  assert(Dtr.cols() % feature_columns() == 0);

  // ----------------------------------
  // Forward
  // ----------------------------------
  size_t time_steps = Dtr.cols() / feature_columns();

  if(_S.size() < time_steps + 1) {
    _S.resize(time_steps + 1);
  }

  _S[0] = Eigen::MatrixXf::Zero(Dtr.rows(), hidden_units());
  
  for(size_t t=1, beg=0; t<=time_steps; ++t) {
    _S[t] = Dtr.middleCols(beg, feature_columns()) * _Wx + _S[t-1] * _Ws + _Bx.replicate(Dtr.rows(), 1);
    activate(_S[t], _activation);
    beg += feature_columns();
  }
  
  //std::cout << "done forward" << std::endl;
  
  // ----------------------------------
  // Loss layer
  // ----------------------------------
  Eigen::MatrixXf delta = _S.back() * _Wy + _By.replicate(Dtr.rows(), 1);

  std::visit([&] (auto&& loss) mutable {
    loss.dloss(delta, Ltr);
  }, _loss);

  _dWy = _S.back().transpose() * delta;
  _dBy = delta.colwise().sum();

  // ----------------------------------
  // Backward propagation
  // ----------------------------------
  _dBx = Eigen::MatrixXf::Zero(_Bx.rows(), _Bx.cols());
  _dWx = Eigen::MatrixXf::Zero(_Wx.rows(), _Wx.cols());
  _dWs = Eigen::MatrixXf::Zero(_Ws.rows(), _Ws.cols());

  for(size_t t=time_steps, beg=(time_steps-1)*feature_columns(); t>=1; t--) {

    // Last layer
    if(t == time_steps) {
      delta = (delta * _Wy.transpose()).cwiseProduct(dactivate(_S[t], _activation));
    }
    // Middle layer
    else {
      delta = (delta * _Ws.transpose()).cwiseProduct(dactivate(_S[t], _activation));
    }
    _dBx = _dBx + delta.colwise().sum();
    _dWx = _dWx + Dtr.middleCols(beg, feature_columns()).transpose() * delta;
    _dWs = _dWs + _S[t-1].transpose() * delta;

    beg -= feature_columns();
  }

  //__clip(_dBx, -5.0f, 5.0f);
  //__clip(_dWx, -5.0f, 5.0f);
  //__clip(_dWs, -5.0f, 5.0f);
  //__clip(_dBx, -5.0f, 5.0f);
  //__clip(_dBy, -5.0f, 5.0f);

  // Update the weight
  _update(lrate / std::sqrt(Dtr.rows()));

  assert(!_dBx.hasNaN() && !_dWx.hasNaN() && !_dWs.hasNaN() && !_dBx.hasNaN() && !_dBy.hasNaN());
  assert(!_Bx.hasNaN() && !_Wx.hasNaN() && !_Ws.hasNaN() && !_Bx.hasNaN() && !_By.hasNaN());
}

// Procedure: _update
void RnnRegressorNx1::_update(float rate) {
  
  std::visit([this, rate](auto&& opt){
    opt.alpha(rate).update(_dWx, _Wx);
    opt.alpha(rate).update(_dBx, _Bx);
    opt.alpha(rate).update(_dWy, _Wy);
    opt.alpha(rate).update(_dBy, _By);
    opt.alpha(rate).update(_dWs, _Ws);
  }, _optimizer); 

  /*constexpr static int tag_wx = 0;
  constexpr static int tag_bx = 1;
  constexpr static int tag_wy = 2;
  constexpr static int tag_by = 3;
  constexpr static int tag_ws = 4;

  std::visit([this, rate](auto&& opt){
    opt.alpha(rate).update(_dWx, _Wx, tag_wx);
    opt.alpha(rate).update(_dBx, _Bx, tag_bx);
    opt.alpha(rate).update(_dWy, _Wy, tag_wy);
    opt.alpha(rate).update(_dBy, _By, tag_by);
    opt.alpha(rate).update(_dWs, _Ws, tag_ws);
  }, _optimizer); */

}



};  // end of namespace dtc::ml. ------------------------------------------------------------------




