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

#include <dtc/ml/loss.hpp>

namespace dtc::ml {

// Procedure: dloss
void MeanSquaredError::dloss(Eigen::MatrixXf& D, const Eigen::VectorXi& Y) const {

  assert(D.rows() == Y.rows());

  for(int i=0; i<Y.rows(); ++i) {
    D(i, Y(i)) -= 1.0f;
  }
}

// Procedure: dloss
void MeanSquaredError::dloss(Eigen::MatrixXf& D, const Eigen::VectorXf& Y) const {
  assert(D.rows() == Y.rows() && D.cols() == 1);
  D -= Y;
}

// ------------------------------------------------------------------------------------------------

// Procedure: dloss
void SoftmaxCrossEntropy::dloss(Eigen::MatrixXf& D, const Eigen::VectorXi& Y) const {

  assert(D.rows() == Y.rows());

  for(int i=0; i<Y.rows(); ++i) {
    D(i, Y(i)) -= 1.0f;
  }
}

// Procedure: dloss
void SoftmaxCrossEntropy::dloss(Eigen::MatrixXf& D, const Eigen::VectorXf& Y) const {
  assert(false);
}


// ------------------------------------------------------------------------------------------------

// Procedure: dloss
void MeanAbsoluteError::dloss(Eigen::MatrixXf& D, const Eigen::VectorXi& Y) const {

  assert(D.rows() == Y.rows());

  for(int i=0; i<D.rows(); ++i) {
    for(int j=0; j<D.cols(); ++j) {
      float pred = j == Y(i) ? 1.0f : 0.0f;
      if(pred > D(i, j)) {
        D(i, j) = -1.0f;
      }
      else if(pred < D(i, j)) {
        D(i, j) = 1.0f;
      }
      else {
        D(i, j) = 0.0f;
      }
    }
  }
}

// Procedure: dloss
void MeanAbsoluteError::dloss(Eigen::MatrixXf& D, const Eigen::VectorXf& Y) const {

  assert(D.rows() == Y.rows() && D.cols() == 1);

  for(int i=0; i<D.rows(); ++i) {
    if(Y(i) > D(i, 0)) {
      D(i, 0) = -1.0f;
    }
    else if(Y(i) < D(i, 0)) {
      D(i, 0) = 1.0f;
    }
    else {
      D(i, 0) = .0f;
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dloss
void HuberLoss::dloss(Eigen::MatrixXf& D, const Eigen::VectorXi& Y) const {
  
  assert(D.rows() == Y.rows());
      
  for(int i=0; i<D.rows(); ++i) {
    for(int j=0; j<D.cols(); ++j) {
      float pred = j == Y(i) ? 1.0f : 0.0f;
      float diff = std::fabs(D(i, j) - pred);
      if(diff <= _delta) {
        D(i, j) -= pred;
      }
      else {
        if(pred > D(i, j)) {
          D(i, j) = -_delta;
        }
        else if(pred < D(i, j)) {
          D(i, j) = _delta;
        }
        else D(i, j) = 0.0f;
      }
    }
  }
}

// Procedure: dloss
void HuberLoss::dloss(Eigen::MatrixXf& D, const Eigen::VectorXf& Y) const {

  assert(D.rows() == Y.rows() && D.cols() == 1);

  for(int i=0; i<D.rows(); ++i) {
    float d = std::fabs(D(i, 0) - Y(i));
    if(d <= _delta) {
      D(i, 0) -= Y(i);
    }
    else {
      if(Y(i) > D(i, 0)) {
        D(i, 0) = -_delta;
      }
      else if(Y(i) < D(i, 0)) {
        D(i, 0) = _delta;
      }
      else {
        D(i, 0) = .0f;
      }
    }
  }
  
}


};  // end of namespace dtc::ml -------------------------------------------------------------------


