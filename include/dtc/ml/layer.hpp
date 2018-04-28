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

#ifndef DTC_ML_LAYER_HPP_
#define DTC_ML_LAYER_HPP_

#include <dtc/utility/utility.hpp>
#include <dtc/ml/activation.hpp>
#include <dtc/ml/loss.hpp>
#include <dtc/ml/optimizer.hpp>

namespace dtc::ml {

// Function: unique_weight_tag
inline int unique_weight_tag() {
  static std::atomic<int> tag {0};
  return ++tag;
}

// ------------------------------------------------------------------------------------------------

// Struct: FullyConnectedLayer
struct FullyConnectedLayer {

  FullyConnectedLayer() = default;
  FullyConnectedLayer(size_t, size_t, std::optional<Activation> = {});

  size_t layer;
  
  Eigen::MatrixXf X;
  Eigen::MatrixXf W;
  Eigen::MatrixXf B;
  Eigen::MatrixXf dW;
  Eigen::MatrixXf dB;
  
  std::optional<Activation> activation;

  Eigen::MatrixXf infer(const Eigen::MatrixXf&) const;

  void fprop(const Eigen::MatrixXf&);
  void bprop(const Eigen::MatrixXf&, Eigen::MatrixXf&);
  void update(Optimizer&, float);

  template <typename ArchiverT>
  auto archive(ArchiverT&);
};

// Function: archive
template <typename ArchiverT>
auto FullyConnectedLayer::archive(ArchiverT& ar) {
  return ar(layer, X, W, B, dW, dB, activation);
}

// ------------------------------------------------------------------------------------------------

// Struct: DropoutLayer
struct DropoutLayer {
  
  DropoutLayer() = default;
  DropoutLayer(float);

  size_t layer;
  
  float keep {0.5f};

  Eigen::MatrixXf X;
  Eigen::MatrixXf P;
  
  Eigen::MatrixXf infer(const Eigen::MatrixXf&) const;

  void fprop(const Eigen::MatrixXf&);
  void bprop(const Eigen::MatrixXf&, Eigen::MatrixXf&);
  void update(Optimizer&, float);

  template <typename ArchiverT>
  auto archive(ArchiverT&);

};

// Function: archive
template <typename ArchiverT>
auto DropoutLayer::archive(ArchiverT& ar) {
  return ar(layer, keep, X);
}

// ------------------------------------------------------------------------------------------------

using Layer = std::variant<
  FullyConnectedLayer,
  DropoutLayer
>;



};  // end of namespace dtc::ml. ------------------------------------------------------------------


#endif
