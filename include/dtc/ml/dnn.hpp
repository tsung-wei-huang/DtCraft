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

#include <dtc/ml/activation.hpp>
#include <dtc/ml/loss.hpp>
#include <dtc/ml/optimizer.hpp>
#include <dtc/ipc/block_file.hpp>
#include <dtc/ipc/streambuf.hpp>
#include <dtc/archive/binary.hpp>

namespace dtc::ml {

// Struct
struct DnnLayer {

  size_t num_neurons {0};
  Activation activation {Activation::NONE};

  inline operator size_t () const;

  template <typename ArchiverT>
  auto archive(ArchiverT&);
};

// Operator ()
inline DnnLayer::operator size_t() const {
  return num_neurons;
}

// Function: archive
template <typename ArchiverT>
auto DnnLayer::archive(ArchiverT& ar) {
  return ar(num_neurons, activation);
}

// ------------------------------------------------------------------------------------------------

// Class: DnnClassifier
class DnnClassifier {

  public:

    DnnClassifier() = default;

    DnnClassifier& fully_connected_layer(size_t, size_t, Activation);

    template <typename O, typename... ArgsT>
    O& optimizer(ArgsT&&...);

    template <typename C>
    DnnClassifier& train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);
    
    Eigen::VectorXi infer(const Eigen::MatrixXf&) const;

    template <typename ArchiverT>
    auto archive(ArchiverT&);
    
    template <typename L, typename... ArgsT>
    L& loss(ArgsT&&...);

    std::streamsize load(const std::filesystem::path&);
    std::streamsize save(const std::filesystem::path&);

  private:
    
    std::vector<DnnLayer> _L;
    std::vector<Eigen::MatrixXf> _X;
    std::vector<Eigen::MatrixXf> _W;
    std::vector<Eigen::MatrixXf> _dW;
    std::vector<Eigen::MatrixXf> _B;
    std::vector<Eigen::MatrixXf> _dB;
    
    Loss _loss {std::in_place_type<SoftmaxCrossEntropy>};
    Optimizer _optimizer {std::in_place_type<AdamOptimizer>};

    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);

    void _shuffle(Eigen::MatrixXf&, Eigen::VectorXi&);
    void _fprop(const Eigen::MatrixXf&);
    void _bprop(Eigen::MatrixXf&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::VectorXi&, float);
    void _update(float);
};

// Function: archive
template <typename ArchiverT>
auto DnnClassifier::archive(ArchiverT& ar) {
  return ar(_loss, _L, _X, _W, _dW, _B, _dB, _optimizer);
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

// Function: train
template <typename C>
DnnClassifier& DnnClassifier::train(Eigen::MatrixXf& Dtr, Eigen::VectorXi& Ltr, size_t e, size_t b, float lrate, C&& c) {

  if(Dtr.rows() != Ltr.rows()) {
    DTC_THROW("Dimension of training data and labels don't match");
  }

  if(_L.size() < 2) {
    DTC_THROW("Neural network must have at least two layers");
  }

  _train(Dtr, Ltr, e, b, lrate, std::forward<C>(c)); 

  return *this;
}

// ------------------------------------------------------------------------------------------------

// Class: DnnRegressor
class DnnRegressor {

  public:

    DnnRegressor() = default;

    DnnRegressor& fully_connected_layer(size_t, size_t, Activation);

    template <typename O, typename... ArgsT>
    O& optimizer(ArgsT&&...);
    
    template <typename L, typename... ArgsT>
    L& loss(ArgsT&&...);

    template <typename C>
    DnnRegressor& train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);
    
    Eigen::MatrixXf infer(const Eigen::MatrixXf&);

    DnnRegressor& batch_norm(bool);

    bool batch_norm() const;

    template <typename ArchiverT>
    auto archive(ArchiverT&);
    
    std::streamsize load(const std::filesystem::path&);
    std::streamsize save(const std::filesystem::path&);

  private:

    //bool _batch_norm {false};

    std::vector<DnnLayer> _L;
    std::vector<Eigen::MatrixXf> _X;
    std::vector<Eigen::MatrixXf> _W;
    std::vector<Eigen::MatrixXf> _dW;
    std::vector<Eigen::MatrixXf> _B;
    std::vector<Eigen::MatrixXf> _dB;
    //std::vector<Eigen::MatrixXf> _gamma;
    //std::vector<Eigen::MatrixXf> _beta;
    //std::vector<Eigen::MatrixXf> _mean;
    //std::vector<Eigen::MatrixXf> _var;
    //std::vector<Eigen::MatrixXf> _isqrtvar;
    //std::vector<Eigen::MatrixXf> _hhat;
    //std::vector<Eigen::MatrixXf> _dbeta;
    //std::vector<Eigen::MatrixXf> _dgamma;
    //std::vector<Eigen::MatrixXf> _rmean;
    //std::vector<Eigen::MatrixXf> _rvar;

    Loss _loss {std::in_place_type<MeanSquaredError>};
    Optimizer _optimizer {std::in_place_type<AdamOptimizer>};

    //void _batch_norm_fp(Eigen::MatrixXf& D, size_t level);
    //void _batch_norm_infer_fp(Eigen::MatrixXf& D, size_t level);
    //void _batch_norm_bp(Eigen::MatrixXf& delta, size_t level);
    void _shuffle(Eigen::MatrixXf&, Eigen::MatrixXf&);
    void _fprop(const Eigen::MatrixXf&);
    void _bprop(Eigen::MatrixXf&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::MatrixXf&, float);
    void _update(float);
    
    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);

};

// Function: archive
template <typename ArchiverT>
auto DnnRegressor::archive(ArchiverT& ar) {
  return ar(_loss, _L, _X, _W, _dW, _B, _dB, _optimizer);
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

// Function: train
template <typename C>
DnnRegressor& DnnRegressor::train(Eigen::MatrixXf& Dtr, Eigen::MatrixXf& Ltr, size_t e, size_t b, float lrate, C&& c) {

  if(Dtr.rows() != Ltr.rows()) {
    DTC_THROW("Dimension of training data and labels don't match");
  }

  if(_L.size() < 2) {
    DTC_THROW("Neural network must have at least two layers");
  }

  _train(Dtr, Ltr, e, b, lrate, std::forward<C>(c)); 

  return *this;
}


};  // end of namespace dtc::ml -------------------------------------------------------------------



#endif
