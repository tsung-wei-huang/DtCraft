/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin, and Martin Xin. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/ml/layer.hpp>

namespace dtc::ml {

// Constructor
FullyConnectedLayer::FullyConnectedLayer(size_t f, size_t t, std::optional<Activation> a) :
  activation {std::move(a)} {
  W = Eigen::MatrixXf::Random(f, t);
  B = Eigen::MatrixXf::Random(1, t);
  dW.resize(f, t);
  dB.resize(1, t);
}

// Function: infer
Eigen::MatrixXf FullyConnectedLayer::infer(const Eigen::MatrixXf& Xin) const {

  Eigen::MatrixXf res = Xin*W + B.replicate(Xin.rows(), 1);

  if(activation) {
    activate(res, *activation);
  }

  return res;
}

// Procedure: fprop
void FullyConnectedLayer::fprop(const Eigen::MatrixXf& Xin) {

  X = Xin*W + B.replicate(Xin.rows(), 1);

  if(activation) {
    activate(X, *activation);
  }
}

// Procedure: bprop
void FullyConnectedLayer::bprop(const Eigen::MatrixXf& Xin, Eigen::MatrixXf& delta) {

  if(activation) {
    delta = delta.cwiseProduct(dactivate(X, *activation));
  }

  dB = delta.colwise().sum();
  dW = Xin.transpose() * delta;

  if(layer) {
    delta = delta * W.transpose();
  }
}

// Procedure: update
void FullyConnectedLayer::update(Optimizer& optimizer, float rate) {
  std::visit([this, rate] (auto&& opt) {
    opt.alpha(rate).update(dW, W);
    opt.alpha(rate).update(dB, B);
  }, optimizer);
}

// ------------------------------------------------------------------------------------------------

// Constructor
DropoutLayer::DropoutLayer(float prob) : keep {std::clamp(prob, 0.0f, 1.0f)} {
}

// Function: infer
Eigen::MatrixXf DropoutLayer::infer(const Eigen::MatrixXf& Xin) const {
  return Xin;
}

// Procedure: fprop
void DropoutLayer::fprop(const Eigen::MatrixXf& Xin) {
	std::binomial_distribution<int> distribution(1, keep);
  P.resize(1, Xin.cols());
  for(int c=0; c<P.cols(); ++c) {
    P(c) = distribution(this_thread::random_engine());
  }
  X = Xin.cwiseProduct(P.replicate(Xin.rows(), 1));
}

// Procedure: bprop
void DropoutLayer::bprop(const Eigen::MatrixXf& Xin, Eigen::MatrixXf& delta) {
  if(layer) {
    delta = delta.cwiseProduct(P.replicate(delta.rows(), 1));
  }
}

// Procedure: update  
void DropoutLayer::update(Optimizer&, float) {
}


};  // end of namespace dtc::ml. ------------------------------------------------------------------
