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

#include <dtc/ml/linear.hpp>

namespace dtc::ml {

// Procedure: _shuffle
void LinearRegressor::_shuffle(Eigen::MatrixXf& X, Eigen::MatrixXf& Y) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(X.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), _gen);
  X = p * X;
  Y = p * Y;
}

// Function: infer
Eigen::MatrixXf LinearRegressor::infer(Eigen::MatrixXf& X) const {
  return (X*_W + _B.replicate(X.rows(), 1));
}

// Function: dimension
LinearRegressor& LinearRegressor::dimension(size_t features, size_t labels) {
  _W = Eigen::MatrixXf::Random(features, labels);
  _B = Eigen::MatrixXf::Random(1, labels);
  return *this;
}

// Procedure: _optimize
void LinearRegressor::_optimize(const Eigen::MatrixXf& X, const Eigen::MatrixXf& Y, float l) {

  // Find the derivative at the last layer
  Eigen::MatrixXf delta = ((X*_W + _B.replicate(X.rows(), 1)) - Y) / (2.0f * Y.cols());
  _dW = X.transpose() * delta;
  _dB = delta.colwise().sum();

  // Update the weight
  _update(l / std::sqrt(X.rows()));
}

// Procedure: _update
void LinearRegressor::_update(float rate) {

  // Update the weight
  constexpr static int tag_w = 0;
  constexpr static int tag_b = 1;

  std::visit([this, rate](auto&& opt){
    opt.alpha(rate).update(_dW, _W, tag_w);
    opt.alpha(rate).update(_dB, _B, tag_b);
  }, _optimizer);
}

// ------------------------------------------------------------------------------------------------

// Procedure: _shuffle
void LinearClassifier::_shuffle(Eigen::MatrixXf& X, Eigen::VectorXi& Y) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(X.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), _gen);
  X = p * X;
  Y = p * Y;
}

// Function: infer
Eigen::VectorXi LinearClassifier::infer(Eigen::MatrixXf& X) const {

  // Compute the res
  Eigen::MatrixXf res = X*_W + _B.replicate(X.rows(), 1);

  // Softmax
  res = (res - res.rowwise().maxCoeff().replicate(1, _B.cols())).array().exp().matrix();
  res = res.cwiseQuotient(res.rowwise().sum().replicate(1, _B.cols()));

  // Prediction
  Eigen::VectorXi label(res.rows());

  for(int k=0; k<label.rows(); ++k) {
    res.row(k).maxCoeff(&label(k));
  }

  return label;
}

// Function: dimension
LinearClassifier& LinearClassifier::dimension(size_t features, size_t labels) {
  _W = Eigen::MatrixXf::Random(features, labels);
  _B = Eigen::MatrixXf::Random(1, labels);
  return *this;
}

// Procedure: _optimize
void LinearClassifier::_optimize(const Eigen::MatrixXf& X, const Eigen::VectorXi& Y, float l) {

  // Compute the output
  Eigen::MatrixXf delta = X*_W + _B.replicate(X.rows(), 1);

  // Softmax (subtract the max for numeric stability)
  delta = (delta - delta.rowwise().maxCoeff().replicate(1, _B.cols())).array().exp().matrix();
  delta = delta.cwiseQuotient(delta.rowwise().sum().replicate(1, _B.cols()));

  // Cross entropy derivative.
  for(int i=0; i<delta.rows(); ++i) {
    delta(i, Y(i)) -= 1.0f;
  }

  _dW = X.transpose() * delta;
  _dB = delta.colwise().sum();

  // Update the weight
  _update(l / std::sqrt(X.rows()));
}

// Procedure: _update
void LinearClassifier::_update(float rate) {

  // Update the weight
  constexpr static int tag_w = 0;
  constexpr static int tag_b = 1;

  std::visit([this, rate](auto&& opt){
    opt.alpha(rate).update(_dW, _W, tag_w);
    opt.alpha(rate).update(_dB, _B, tag_b);
  }, _optimizer);
}


};  // end of namespace dtc::ml -------------------------------------------------------------------



