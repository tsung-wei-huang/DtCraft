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

#include <dtc/ml/optimizer.hpp>

namespace dtc::ml {

// Procedure: update
void AdamOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W) {
  
  if(auto itr = _states.find(&W); itr == _states.end()) {
    _states[&W].mt = Eigen::MatrixXf::Zero(W.rows(), W.cols());
    _states[&W].vt = Eigen::MatrixXf::Zero(W.rows(), W.cols());
  }

  auto& s = _states[&W];

  s.mt = (_b1 * s.mt.array() + (1.0f - _b1) * dW.array()).matrix();
  s.vt = (_b2 * s.vt.array() + (1.0f - _b2) * dW.array().square()).matrix();

  W -= (_alpha * (s.mt.array() / (1.0f - s.b1_t)) / 
                ((s.vt.array() / (1.0f - s.b2_t)).sqrt() + _eps)).matrix();
  
  s.b1_t *= _b1;
  s.b2_t *= _b2;

}

// ------------------------------------------------------------------------------------------------

// Procedure: update
void AdamaxOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W) {
  
  if(auto itr = _states.find(&W); itr == _states.end()) {
    _states[&W].mt = Eigen::MatrixXf::Zero(W.rows(), W.cols());
    _states[&W].ut = Eigen::MatrixXf::Zero(W.rows(), W.cols());
  }

  auto& s = _states[&W];

  s.mt = (_b1 * s.mt.array() + (1.0f - _b1) * dW.array()).matrix();
  s.ut = (_b2 * s.ut.array()).max(dW.array().abs());
  W -= (_alpha / (1.0f - s.b1_t) * (s.mt.array() / (s.ut.array() + _eps))).matrix();
  s.b1_t *= _b1;
}

// ------------------------------------------------------------------------------------------------

// Procedure: update
void GradientDescentOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W) {
  W = W - _alpha * (dW + _decay * W);
}
    
// ------------------------------------------------------------------------------------------------

// Procedure: update
void AdagradOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W) {

  if(auto itr = _states.find(&W); itr == _states.end()) {
    _states[&W] = Eigen::MatrixXf::Zero(W.rows(), W.cols());
  }

  auto& g = _states[&W];

  g += dW.array().square().matrix();
  W -= (_alpha * dW.array() / (g.array().sqrt() + _eps)).matrix();
}
    
// ------------------------------------------------------------------------------------------------

// Procedure: update
void RMSpropOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W) {

  if(auto itr = _states.find(&W); itr == _states.end()) {
    _states[&W] = Eigen::MatrixXf::Zero(W.rows(), W.cols());
  }

  auto& g = _states[&W];
  
  g = (_mu * g.array() + (1.0f - _mu) * dW.array().square()).matrix();
  W -= (_alpha * dW.array() / (g.array().sqrt() + _eps)).matrix();
}

// ------------------------------------------------------------------------------------------------

// Procedure: update
void MomentumOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W) {
  
  if(auto itr = _states.find(&W); itr == _states.end()) {
    _states[&W] = Eigen::MatrixXf::Zero(W.rows(), W.cols());
  }

  auto& dW_prev = _states[&W];

  dW_prev = _mu * dW_prev - _alpha * (dW + W * _lambda);
  W += dW_prev;
}



};  // end of namespace dtc::ml .------------------------------------------------------------------





