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

#ifndef DTC_ML_LOSS_HPP_
#define DTC_ML_LOSS_HPP_

#include <dtc/headerdef.hpp>

namespace dtc::ml {

// ------------------------------------------------------------------------------------------------

// MeanSquaredError
struct MeanSquaredError {

  MeanSquaredError() = default;

  void dloss(Eigen::MatrixXf&, const Eigen::VectorXi&) const;
  void dloss(Eigen::MatrixXf&, const Eigen::VectorXf&) const;
  void dloss(Eigen::MatrixXf&, const Eigen::MatrixXf&) const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT&);
};
  
template <typename ArchiverT>
std::streamsize MeanSquaredError::archive(ArchiverT& ar) {
  return 0;
}

// ------------------------------------------------------------------------------------------------

// MeanAbsoluteError
struct MeanAbsoluteError {

  MeanAbsoluteError() = default;
  
  void dloss(Eigen::MatrixXf&, const Eigen::VectorXi&) const;
  void dloss(Eigen::MatrixXf&, const Eigen::VectorXf&) const;
  void dloss(Eigen::MatrixXf&, const Eigen::MatrixXf&) const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT&);
};

template <typename ArchiverT>
std::streamsize MeanAbsoluteError::archive(ArchiverT& ar) {
  return 0;
}

// ------------------------------------------------------------------------------------------------

// SoftmaxCrossEntropy
struct SoftmaxCrossEntropy {

  SoftmaxCrossEntropy() = default;
  
  void dloss(Eigen::MatrixXf&, const Eigen::VectorXi&) const;
  void dloss(Eigen::MatrixXf&, const Eigen::VectorXf&) const;
  void dloss(Eigen::MatrixXf&, const Eigen::MatrixXf&) const;

  template <typename ArchiverT>
  std::streamsize archive(ArchiverT&);
};

template <typename ArchiverT>
std::streamsize SoftmaxCrossEntropy::archive(ArchiverT& ar) {
  return 0;
}

// ------------------------------------------------------------------------------------------------

// Class: HuberLoss
class HuberLoss {
  
  public:

    HuberLoss() = default;

    inline HuberLoss(float);

    inline void delta(float);
    inline float delta() const;
    
    void dloss(Eigen::MatrixXf&, const Eigen::VectorXi&) const;
    void dloss(Eigen::MatrixXf&, const Eigen::VectorXf&) const;
    void dloss(Eigen::MatrixXf&, const Eigen::MatrixXf&) const;

    template <typename ArchiverT>
    std::streamsize archive(ArchiverT&);
  
  private:
  
    float _delta {1.0f};
};

// Constructor
inline HuberLoss::HuberLoss(float v) : _delta {v} {
}

// Function: delta
inline void HuberLoss::delta(float d) {
  _delta = d; 
}

// Function: delta
inline float HuberLoss::delta() const {
  return _delta;
}

template <typename ArchiverT>
std::streamsize HuberLoss::archive(ArchiverT& ar) {
  return 0;
}

// ------------------------------------------------------------------------------------------------

// Loss type
using Loss = std::variant<
  MeanSquaredError,
  MeanAbsoluteError,
  SoftmaxCrossEntropy,
  HuberLoss
>;

};  // end of namespace dtc::ml -------------------------------------------------------------------

#endif




