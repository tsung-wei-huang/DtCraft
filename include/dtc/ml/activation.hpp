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

#ifndef DTC_ML_ACTIVATION_HPP_
#define DTC_ML_ACTIVATION_HPP_

#include <dtc/headerdef.hpp>

namespace dtc::ml {

enum class Activation {
  SIGMOID,
  TANH,
  RELU,
  LEAKY_RELU,
  NONE
};

// Function: dtanh 
inline Eigen::MatrixXf dtanh(const Eigen::MatrixXf& x) {
  return 1.0f - x.array().square();
}

// Function: dsigmoid
inline Eigen::MatrixXf dsigmoid(const Eigen::MatrixXf& x) {
  return x.array() * (1 - x.array());
}

// Function: drelu
inline Eigen::MatrixXf drelu(const Eigen::MatrixXf& x) {
  Eigen::MatrixXf res(x.rows(), x.cols());
  for(int j=0; j<res.cols(); ++j) {
    for(int i=0; i<res.rows(); ++i) {
      res(i, j) = x(i, j) > 0.0f ? 1.0f : 0.0f;
    }
  }
  return res;
}

// Function: dleaky_relu
inline Eigen::MatrixXf dleaky_relu(const Eigen::MatrixXf& x) {
  Eigen::MatrixXf res(x.rows(), x.cols());
  for(int j=0; j<res.cols(); ++j) {
    for(int i=0; i<res.rows(); ++i) {
      res(i, j) = x(i, j) > 0.0f ? 1.0f : 0.01f;
    }
  }
  return x;
}

// Procedure: tanh
inline void tanh(Eigen::MatrixXf& x) {
  x = (x.array().tanh()).matrix();
}

// Procedure: sigmoid
inline void sigmoid(Eigen::MatrixXf& x) {
  x = ((1.0f + (-x).array().exp()).inverse()).matrix();
}

// Procedure: relu
inline void relu(Eigen::MatrixXf& x) {
  for(int j=0; j<x.cols(); ++j) {
    for(int i=0; i<x.rows(); ++i) {
      if(x(i, j) <= 0.0f) {
        x(i, j) = 0.0f;
      }
    }
  }
}

// Procedure: leaky_relu
inline void leaky_relu(Eigen::MatrixXf& x) {
  for(int j=0; j<x.cols(); ++j) {
    for(int i=0; i<x.rows(); ++i) {
      if(x(i, j) <= 0.0f) {
        x(i, j) *= 0.01f;
      }
    }
  }
}

// Procedure: activate
inline void activate(Eigen::MatrixXf& x, Activation a) {

  switch(a) {
    case Activation::SIGMOID:
      sigmoid(x);
    break;

    case Activation::TANH:
      tanh(x);
    break;

    case Activation::RELU:
      relu(x);
    break;

    case Activation::LEAKY_RELU:
      leaky_relu(x);
    break;

    default:
    break;
  }
}

// Function: dactivate
inline Eigen::MatrixXf dactivate(const Eigen::MatrixXf& x, Activation a) {
  
  switch(a) {
    case Activation::SIGMOID:
      return dsigmoid(x);
    break;

    case Activation::TANH:
      return dtanh(x);
    break;

    case Activation::RELU:
      return drelu(x);
    break;

    case Activation::LEAKY_RELU:
      return dleaky_relu(x);
    break;

    default:
      return Eigen::MatrixXf::Ones(x.rows(), x.cols());
    break;
  }
}

};  // end of namespace dtc::ml -------------------------------------------------------------------

#endif
