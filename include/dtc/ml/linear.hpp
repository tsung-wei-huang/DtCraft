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

#ifndef DTC_ML_LINEAR_HPP_
#define DTC_ML_LINEAR_HPP_

#include <dtc/ml/optimizer.hpp>
#include <dtc/ml/activation.hpp>
#include <dtc/ml/loss.hpp>
#include <dtc/ml/optimizer.hpp>
#include <dtc/ipc/block_file.hpp>
#include <dtc/ipc/streambuf.hpp>
#include <dtc/archive/binary.hpp>

namespace dtc::ml {

// Class: LinearRegressor
class LinearRegressor {

  public:
    
    LinearRegressor() = default;

    LinearRegressor& dimension(size_t, size_t);

    template <typename O, typename... ArgsT>
    O& optimizer(ArgsT&&...);

    template <typename C>
    LinearRegressor& train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);

    Eigen::MatrixXf infer(Eigen::MatrixXf&) const;

    inline size_t label_dimension() const;
    inline size_t feature_columns() const;

  private:

    Eigen::MatrixXf _W;
    Eigen::MatrixXf _dW;
    Eigen::MatrixXf _B;
    Eigen::MatrixXf _dB;

    Optimizer _optimizer{std::in_place_type<AdamOptimizer>};
    
    void _shuffle(Eigen::MatrixXf&, Eigen::MatrixXf&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::MatrixXf&, float);
    void _update(float);
    
    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);
};

// Function: label_dimension
inline size_t LinearRegressor::label_dimension() const {
  return _W.cols();
}

// Function: feature_columns
inline size_t LinearRegressor::feature_columns() const {
  return _W.rows();
}
    
// Function: optimizer
template <typename O, typename... ArgsT>
O& LinearRegressor::optimizer(ArgsT&&... args) {
  return _optimizer.emplace<O>(std::forward<ArgsT>(args)...); 
}

// Function: train
template <typename C>
LinearRegressor& LinearRegressor::train(Eigen::MatrixXf& X, Eigen::MatrixXf& Y, size_t e, size_t b, float l, C&& c) {

  if(X.rows() != Y.rows()) {
    DTC_THROW("Dimension of training data and labels don't match");
  }

  _train(X, Y, e, b, l, std::forward<C>(c)); 

  return *this;
}

// Function: _train
template <typename C>
void LinearRegressor::_train(Eigen::MatrixXf& X, Eigen::MatrixXf& Y, size_t e, size_t b, float l, C&& c) {

	const size_t num_trains = X.rows();

  for(size_t i=0; i<e; ++i) {

    _shuffle(X, Y);

		for(size_t j=0; j<num_trains; j+= b) {
      auto n = (j + b) < num_trains ? b : num_trains - j;
      _optimize(X.middleRows(j, n), Y.middleRows(j, n), l);
    }            
    
    if constexpr(std::is_invocable_v<C, LinearRegressor&>) {
      c(*this);
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Class: LinearClassifier
// Linear classifier using softmax cross-entropy.
class LinearClassifier {
  
  public:

    LinearClassifier() = default;

    LinearClassifier& dimension(size_t, size_t = 1);

    inline std::tuple<size_t, size_t> dimension() const;
    
    template <typename O, typename... ArgsT>
    O& optimizer(ArgsT&&...);

    template <typename C>
    LinearClassifier& train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);

    Eigen::VectorXi infer(Eigen::MatrixXf&) const;

  private:
    
    Eigen::MatrixXf _W;
    Eigen::MatrixXf _dW;
    Eigen::MatrixXf _B;
    Eigen::MatrixXf _dB;

    Optimizer _optimizer{std::in_place_type<AdamOptimizer>};
    
    std::default_random_engine _gen {0};
    
    void _shuffle(Eigen::MatrixXf&, Eigen::VectorXi&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::VectorXi&, float);
    void _update(float);
    
    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);
};

// Function: dimension
inline std::tuple<size_t, size_t> LinearClassifier::dimension() const {
  return std::make_tuple(_W.rows(), _W.cols());
}

// Function: optimizer
template <typename O, typename... ArgsT>
O& LinearClassifier::optimizer(ArgsT&&... args) {
  return _optimizer.emplace<O>(std::forward<ArgsT>(args)...); 
}

// Function: train
template <typename C>
LinearClassifier& LinearClassifier::train(Eigen::MatrixXf& X, Eigen::VectorXi& Y, size_t e, size_t b, float l, C&& c) {

  if(X.rows() != Y.rows()) {
    DTC_THROW("Dimension of training data and labels don't match");
  }

  _train(X, Y, e, b, l, std::forward<C>(c)); 

  return *this;
}

// Function: _train
template <typename C>
void LinearClassifier::_train(Eigen::MatrixXf& X, Eigen::VectorXi& Y, size_t e, size_t b, float l, C&& c) {

	const size_t num_trains = X.rows();

  for(size_t i=0; i<e; ++i) {

    _shuffle(X, Y);

		for(size_t j=0; j<num_trains; j+= b) {
      auto n = (j + b) < num_trains ? b : num_trains - j;
      _optimize(X.middleRows(j, n), Y.middleRows(j, n), l);
    }            
    
    if constexpr(std::is_invocable_v<C, LinearClassifier&>) {
      c(*this);
    }
  }
}



};  // end of namespace dtc::ml. ------------------------------------------------------------------


#endif

