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

#ifndef DTC_ML_RNN_HPP_
#define DTC_ML_RNN_HPP_

#include <dtc/ml/activation.hpp>
#include <dtc/ml/loss.hpp>
#include <dtc/ml/optimizer.hpp>
#include <dtc/ipc/block_file.hpp>
#include <dtc/ipc/streambuf.hpp>
#include <dtc/archive/binary.hpp>

namespace dtc::ml {

// ------------------------------------------------------------------------------------------------

// Class: RnnRegressorNx1
class RnnRegressorNx1 {

  public:

    RnnRegressorNx1() = default;

    RnnRegressorNx1& cell(size_t, size_t, size_t, Activation act);

    template <typename O, typename... ArgsT>
    O& optimizer(ArgsT&&...);
    
    template <typename L, typename... ArgsT>
    L& loss(ArgsT&&...);

    template <typename C>
    RnnRegressorNx1& train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);
    
    Eigen::MatrixXf infer(const Eigen::MatrixXf&);

    inline size_t label_dimension() const;
    inline size_t feature_columns() const;
    inline size_t hidden_units() const;

  private:

    Activation _activation;

    Eigen::MatrixXf _Wx;
    Eigen::MatrixXf _Ws;       // X * _Wx + S * _Ws + _Bx
    Eigen::MatrixXf _Wy;       // S * _Wy + _By 
    Eigen::MatrixXf _Bx;
    Eigen::MatrixXf _By;
    Eigen::MatrixXf _dWx;
    Eigen::MatrixXf _dWy;
    Eigen::MatrixXf _dWs;
    Eigen::MatrixXf _dBx;
    Eigen::MatrixXf _dBy;

    std::vector<Eigen::MatrixXf> _S;

    Loss _loss {std::in_place_type<MeanSquaredError>};
    Optimizer _optimizer {std::in_place_type<AdamOptimizer>};

    void _shuffle(Eigen::MatrixXf&, Eigen::MatrixXf&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::MatrixXf&, float);
    void _update(float);
    
    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);
};

// Function: label_dimenstion    
inline size_t RnnRegressorNx1::label_dimension() const {
  return _Wy.cols();
}

inline size_t RnnRegressorNx1::feature_columns() const {
  return _Wx.rows();
}

inline size_t RnnRegressorNx1::hidden_units() const {
  return _Wx.cols();
}

// Function: _train
template <typename C>
void RnnRegressorNx1::_train(Eigen::MatrixXf& Dtr, Eigen::MatrixXf& Ltr, size_t e, size_t b, float l, C&& c) {

	const size_t num_trains = Dtr.rows();

  for(size_t i=0; i<e; ++i) {
    _shuffle(Dtr, Ltr);
		for(size_t j=0; j<num_trains; j+= b) {
      auto n = (j + b) < num_trains ? b : num_trains - j;
      _optimize(Dtr.middleRows(j, n), Ltr.middleRows(j, n), l);
    }            
    
    if constexpr(std::is_invocable_v<C, RnnRegressorNx1&>) {
      c(*this);
    }
  }
}
    
// Function: optimizer
template <typename O, typename... ArgsT>
O& RnnRegressorNx1::optimizer(ArgsT&&... args) {
  return _optimizer.emplace<O>(std::forward<ArgsT>(args)...); 
}

// Function: loss
template <typename L, typename... ArgsT>
L& RnnRegressorNx1::loss(ArgsT&&... args) {
  static_assert(!std::is_same_v<L, SoftmaxCrossEntropy>);
  return _loss.emplace<L>(std::forward<ArgsT>(args)...);
}

// Function: train
template <typename C>
RnnRegressorNx1& RnnRegressorNx1::train(Eigen::MatrixXf& Dtr, Eigen::MatrixXf& Ltr, size_t e, size_t b, float lrate, C&& c) {

  if(Dtr.rows() != Ltr.rows() || Ltr.cols() != static_cast<int>(label_dimension())) {
    DTC_THROW("Data dimension mismatched");
  }

  if(Dtr.cols() % feature_columns() != 0) {
    DTC_THROW("Time dimension mismatched");
  }

  _train(Dtr, Ltr, e, b, lrate, std::forward<C>(c)); 
  return *this;
}


};  // end of namespace dtc::ml -------------------------------------------------------------------



#endif
