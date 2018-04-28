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

#ifndef DTC_ML_DNN_HPP_
#define DTC_ML_DNN_HPP_

#include <dtc/ml/loss.hpp>
#include <dtc/ml/layer.hpp>
#include <dtc/ipc/block_file.hpp>
#include <dtc/ipc/streambuf.hpp>
#include <dtc/archive/binary.hpp>

namespace dtc::ml {

// Class: DnnClassifier
class DnnClassifier {

  public:

    DnnClassifier() = default;

    template <typename O, typename... ArgsT>
    O& optimizer(ArgsT&&...);

    template <typename C>
    DnnClassifier& train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);
    
    Eigen::VectorXi infer(const Eigen::MatrixXf&) const;

    template <typename ArchiverT>
    auto archive(ArchiverT&);
    
    template <typename L, typename... ArgsT>
    L& loss(ArgsT&&...);

    template <typename L, typename... ArgsT>
    L& layer(ArgsT&&...);

    std::streamsize load(const std::filesystem::path&);
    std::streamsize save(const std::filesystem::path&);

  private:

    std::vector<Layer> _layers;
    
    Loss _loss {std::in_place_type<SoftmaxCrossEntropy>};
    Optimizer _optimizer {std::in_place_type<AdamOptimizer>};

    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);

    void _shuffle(Eigen::MatrixXf&, Eigen::VectorXi&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::VectorXi&, float);
    void _update(float);
};

// Function: archive
template <typename ArchiverT>
auto DnnClassifier::archive(ArchiverT& ar) {
  return ar(_layers, _loss);
}
    
// Function: _train
template <typename C>
void DnnClassifier::_train(Eigen::MatrixXf& Dtr, Eigen::VectorXi& Ltr, size_t e, size_t b, float l, C&& c) {

	const size_t num_trains = Dtr.rows();

  for(size_t i=0; i<e; ++i) {
    _shuffle(Dtr, Ltr);
		for(size_t j=0; j<num_trains; j+= b) {
      auto n = (j + b) < num_trains ? b : num_trains - j;
      _optimize(Dtr.middleRows(j, n), Ltr.middleRows(j, n), l);
    }            
    
    if constexpr(std::is_invocable_v<C, DnnClassifier&>) {
      c(*this);
    }
  }
}

// Function: loss
template <typename L, typename... ArgsT>
L& DnnClassifier::loss(ArgsT&&... args) {
  return _loss.emplace<L>(std::forward<ArgsT>(args)...);
}
    
// Function: optimizer
template <typename O, typename... ArgsT>
O& DnnClassifier::optimizer(ArgsT&&... args) {
  return _optimizer.emplace<O>(std::forward<ArgsT>(args)...); 
}

// Function: layer
template <typename L, typename... ArgsT>
L& DnnClassifier::layer(ArgsT&&... args) {
  L& l = std::get<L>(_layers.emplace_back(std::in_place_type<L>, std::forward<ArgsT>(args)...));
  l.layer = _layers.size() - 1;
  return l;
}

// Function: train
template <typename C>
DnnClassifier& DnnClassifier::train(Eigen::MatrixXf& Dtr, Eigen::VectorXi& Ltr, size_t e, size_t b, float lrate, C&& c) {

  if(Dtr.rows() != Ltr.rows()) {
    DTC_THROW("Dimension of training data and labels don't match");
  }

  if(_layers.empty()) {
    DTC_THROW("Neural network must have at one layer");
  }

  _train(Dtr, Ltr, e, b, lrate, std::forward<C>(c)); 

  return *this;
}

// ------------------------------------------------------------------------------------------------

// Class: DnnRegressor
class DnnRegressor {

  public:

    DnnRegressor() = default;

    template <typename L, typename... ArgsT>
    L& layer(ArgsT&&...);

    template <typename O, typename... ArgsT>
    O& optimizer(ArgsT&&...);
    
    template <typename L, typename... ArgsT>
    L& loss(ArgsT&&...);

    template <typename C>
    DnnRegressor& train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);
    
    Eigen::MatrixXf infer(const Eigen::MatrixXf&);

    template <typename ArchiverT>
    auto archive(ArchiverT&);
    
    std::streamsize load(const std::filesystem::path&);
    std::streamsize save(const std::filesystem::path&);

  private:

    std::vector<Layer> _layers;

    Loss _loss {std::in_place_type<MeanSquaredError>};
    Optimizer _optimizer {std::in_place_type<AdamOptimizer>};

    void _shuffle(Eigen::MatrixXf&, Eigen::MatrixXf&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::MatrixXf&, float);
    void _update(float);
    
    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);

};

// Function: archive
template <typename ArchiverT>
auto DnnRegressor::archive(ArchiverT& ar) {
  return ar(_layers, _loss);
}

// Function: _train
template <typename C>
void DnnRegressor::_train(Eigen::MatrixXf& Dtr, Eigen::MatrixXf& Ltr, size_t e, size_t b, float l, C&& c) {

	const size_t num_trains = Dtr.rows();

  for(size_t i=0; i<e; ++i) {
    _shuffle(Dtr, Ltr);
		for(size_t j=0; j<num_trains; j+= b) {
      auto n = (j + b) < num_trains ? b : num_trains - j;
      _optimize(Dtr.middleRows(j, n), Ltr.middleRows(j, n), l);
    }            
    
    if constexpr(std::is_invocable_v<C, DnnRegressor&>) {
      c(*this);
    }
  }
}
    
// Function: optimizer
template <typename O, typename... ArgsT>
O& DnnRegressor::optimizer(ArgsT&&... args) {
  return _optimizer.emplace<O>(std::forward<ArgsT>(args)...); 
}

// Function: loss
template <typename L, typename... ArgsT>
L& DnnRegressor::loss(ArgsT&&... args) {
  static_assert(!std::is_same_v<L, SoftmaxCrossEntropy>);
  return _loss.emplace<L>(std::forward<ArgsT>(args)...);
}

// Function: layer
template <typename L, typename... ArgsT>
L& DnnRegressor::layer(ArgsT&&... args) {
  L& l = std::get<L>(_layers.emplace_back(std::in_place_type<L>, std::forward<ArgsT>(args)...));
  l.layer = _layers.size() - 1;
  return l;
}

// Function: train
template <typename C>
DnnRegressor& DnnRegressor::train(Eigen::MatrixXf& Dtr, Eigen::MatrixXf& Ltr, size_t e, size_t b, float lrate, C&& c) {

  if(Dtr.rows() != Ltr.rows()) {
    DTC_THROW("Dimension of training data and labels don't match");
  }

  if(_layers.empty()) {
    DTC_THROW("Neural network must have at least one layer");
  }

  _train(Dtr, Ltr, e, b, lrate, std::forward<C>(c)); 

  return *this;
}

// 


};  // end of namespace dtc::ml -------------------------------------------------------------------



#endif
