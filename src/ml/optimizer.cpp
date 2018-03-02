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

// Procedure: __restate__
static void __restate__(std::vector<Eigen::MatrixXf>& tgt, const std::vector<Eigen::MatrixXf>& W) {
  if(tgt.size() != W.size()) {
    tgt.resize(W.size());
    for(size_t i=0; i<W.size(); ++i) {
      tgt[i] = Eigen::MatrixXf::Zero(W[i].rows(), W[i].cols());
    }
  }
}

// Procedure: __restate__
static void __restate__(std::vector<Eigen::MatrixXf>& tgt, Eigen::MatrixXf& W) {
  if(tgt.size() != 1) {
    tgt.resize(1);
    tgt[0] = Eigen::MatrixXf::Zero(W.rows(), W.cols());
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: update
void AdamOptimizer::update(const std::vector<Eigen::MatrixXf>& dW, std::vector<Eigen::MatrixXf>& W, int tag) {
  
  assert(dW.size() == W.size());

  auto& mt = _mts[tag];
  auto& vt = _vts[tag];

  __restate__(mt, W);
  __restate__(vt, W);

  for(size_t i=0; i<W.size(); ++i) {

    mt[i] = (_b1 * mt[i].array() + (1.0f - _b1) * dW[i].array()).matrix();
    vt[i] = (_b2 * vt[i].array() + (1.0f - _b2) * dW[i].array().square()).matrix();

    W[i] -= (_alpha * (mt[i].array() / (1.0f - _b1_t)) / 
                     ((vt[i].array() / (1.0f - _b2_t)).sqrt() + _eps)).matrix();
  }

  _b1_t *= _b1;
  _b2_t *= _b2;
}

// Procedure: update
void AdamOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W, int tag) {
  
  auto& mt = _mts[tag];
  auto& vt = _vts[tag];

  __restate__(mt, W);
  __restate__(vt, W);

  mt[0] = (_b1 * mt[0].array() + (1.0f - _b1) * dW.array()).matrix();
  vt[0] = (_b2 * vt[0].array() + (1.0f - _b2) * dW.array().square()).matrix();

  W -= (_alpha * (mt[0].array() / (1.0f - _b1_t)) / 
                ((vt[0].array() / (1.0f - _b2_t)).sqrt() + _eps)).matrix();
  
  _b1_t *= _b1;
  _b2_t *= _b2;
}


// ------------------------------------------------------------------------------------------------

// Procedure: update
void AdamaxOptimizer::update(const std::vector<Eigen::MatrixXf>& dW, std::vector<Eigen::MatrixXf>& W, int tag) {
  
  assert(dW.size() == W.size());

  auto& mt = _mts[tag];
  auto& ut = _uts[tag];

  __restate__(mt, W);
  __restate__(ut, W);

  for(size_t i=0; i<W.size(); ++i) {
    mt[i] = (_b1 * mt[i].array() + (1.0f - _b1) * dW[i].array()).matrix();
    ut[i] = (_b2 * ut[i].array()).max(dW[i].array().abs());
    W[i] -= (_alpha / (1.0f - _b1_t) * (mt[i].array() / (ut[i].array() + _eps))).matrix();
  }

  _b1_t *= _b1;
}

// Procedure: update
void AdamaxOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W, int tag) {
  
  auto& mt = _mts[tag];
  auto& ut = _uts[tag];

  __restate__(mt, W);
  __restate__(ut, W);

  mt[0] = (_b1 * mt[0].array() + (1.0f - _b1) * dW.array()).matrix();
  ut[0] = (_b2 * ut[0].array()).max(dW.array().abs());
  W -= (_alpha / (1.0f - _b1_t) * (mt[0].array() / (ut[0].array() + _eps))).matrix();

  _b1_t *= _b1;
}

// ------------------------------------------------------------------------------------------------

// Procedure: update
void GradientDescentOptimizer::update(const std::vector<Eigen::MatrixXf>& dW, std::vector<Eigen::MatrixXf>& W, int tag) {

  assert(dW.size() == W.size());

  for(size_t i=0; i<W.size(); ++i) {
    W[i] = W[i] - _alpha * (dW[i] + _decay * W[i]);
  }
}
    
// Procedure: update
void GradientDescentOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W, int tag) {
  W = W - _alpha * (dW + _decay * W);
}
    
// ------------------------------------------------------------------------------------------------

// Procedure: update
void AdagradOptimizer::update(const std::vector<Eigen::MatrixXf>& dW, std::vector<Eigen::MatrixXf>& W, int tag) {
  
  assert(dW.size() == W.size());

  auto& g = _gs[tag];

  __restate__(g, W);
  
  for(size_t i=0; i<W.size(); ++i) {
    g[i] += dW[i].array().square().matrix();
    W[i] -= (_alpha * dW[i].array() / (g[i].array().sqrt() + _eps)).matrix();
  }
}

// Procedure: update
void AdagradOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W, int tag) {

  auto& g = _gs[tag];

  __restate__(g, W);
  
  g[0] += dW.array().square().matrix();
  W -= (_alpha * dW.array() / (g[0].array().sqrt() + _eps)).matrix();
}
    
// ------------------------------------------------------------------------------------------------

// Procedure: update
void RMSpropOptimizer::update(const std::vector<Eigen::MatrixXf>& dW, std::vector<Eigen::MatrixXf>& W, int tag) {
  
  assert(dW.size() == W.size());

  auto& g = _gs[tag];

  __restate__(g, W);

  for(size_t i=0; i<W.size(); ++i) {
    g[i] = (_mu * g[i].array() + (1.0f - _mu) * dW[i].array().square()).matrix();
    W[i] -= (_alpha * dW[i].array() / (g[i].array().sqrt() + _eps)).matrix();
  };

}

// Procedure: update
void RMSpropOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W, int tag) {
  
  auto& g = _gs[tag];

  __restate__(g, W);

  g[0] = (_mu * g[0].array() + (1.0f - _mu) * dW.array().square()).matrix();
  W -= (_alpha * dW.array() / (g[0].array().sqrt() + _eps)).matrix();
}


// ------------------------------------------------------------------------------------------------

// Procedure: update
void MomentumOptimizer::update(const std::vector<Eigen::MatrixXf>& dW, std::vector<Eigen::MatrixXf>& W, int tag) {

  assert(dW.size() == W.size());

  auto& dW_prev = _dW_prevs[tag];

  __restate__(dW_prev, W);

  for(size_t i=0; i<W.size(); ++i) {
    dW_prev[i] = _mu * dW_prev[i] - _alpha * (dW[i] + W[i] * _lambda);
    W[i] += dW_prev[i];
  }
}

// Procedure: update
void MomentumOptimizer::update(const Eigen::MatrixXf& dW, Eigen::MatrixXf& W, int tag) {

  auto& dW_prev = _dW_prevs[tag];

  __restate__(dW_prev, W);

  dW_prev[0] = _mu * dW_prev[0] - _alpha * (dW + W * _lambda);
  W += dW_prev[0];
}



};  // end of namespace dtc::ml .------------------------------------------------------------------





