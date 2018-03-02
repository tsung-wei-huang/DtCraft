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

namespace dtc::ml {

// Struct
struct DnnLayer {
  const size_t num_neurons {0};
  const Activation activation {Activation::NONE};
  inline operator size_t () const { return num_neurons; }
};

// ------------------------------------------------------------------------------------------------

// Class: DnnClassifier
class DnnClassifier {

  public:

    DnnClassifier() = default;

    DnnClassifier& fully_connected_layer(size_t, size_t, Activation);
    DnnClassifier& error(Loss);

    template <typename O, typename... ArgsT>
    DnnClassifier& optimizer(ArgsT&&...);

    template <typename C>
    DnnClassifier& train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);
    
    Eigen::VectorXi infer(const Eigen::MatrixXf&) const;

  private:
    
    Loss _loss {Loss::SOFTMAX_CROSS_ENTROPY};

    std::vector<DnnLayer> _L;
    std::vector<Eigen::MatrixXf> _X;
    std::vector<Eigen::MatrixXf> _W;
    std::vector<Eigen::MatrixXf> _dW;
    std::vector<Eigen::MatrixXf> _B;
    std::vector<Eigen::MatrixXf> _dB;

    Optimizer _optimizer;

    std::default_random_engine _gen {0};

    void _shuffle(Eigen::MatrixXf&, Eigen::VectorXi&);
    void _fprop(const Eigen::MatrixXf&);
    void _bprop(Eigen::MatrixXf&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::VectorXi&, float);
    void _update(float);

    inline Eigen::MatrixXf _dact(const Eigen::MatrixXf&, Activation) const;
    inline Eigen::MatrixXf _dtanh(const Eigen::MatrixXf&) const;
    inline Eigen::MatrixXf _dsigmoid(const Eigen::MatrixXf&) const;
    inline Eigen::MatrixXf _drelu(const Eigen::MatrixXf&) const;

    inline void _tanh(Eigen::MatrixXf&) const;
    inline void _sigmoid(Eigen::MatrixXf&) const;
    inline void _relu(Eigen::MatrixXf&) const;
    inline void _act(Eigen::MatrixXf&, Activation) const;
    
    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::VectorXi&, size_t, size_t, float, C&&);

};
    
// Function: _dtanh 
inline Eigen::MatrixXf DnnClassifier::_dtanh(const Eigen::MatrixXf& x) const {
  return 1.0f - x.array().square();
}

// Function: _dsigmoid
inline Eigen::MatrixXf DnnClassifier::_dsigmoid(const Eigen::MatrixXf& x) const {
  return x.array() * (1 - x.array());
}

// Function: _drelu
inline Eigen::MatrixXf DnnClassifier::_drelu(const Eigen::MatrixXf& x) const {
  Eigen::MatrixXf res(x.rows(), x.cols());
  for(int j=0; j<res.cols(); ++j) {
    for(int i=0; i<res.rows(); ++i) {
      res(i, j) = x(i, j) > 0.0f ? 1.0f : 0.0f;
    }
  }
  return res;
}

// Procedure: _tanh
inline void DnnClassifier::_tanh(Eigen::MatrixXf& x) const {
  x = (x.array().tanh()).matrix();
}

// Procedure: _sigmoid
inline void DnnClassifier::_sigmoid(Eigen::MatrixXf& x) const {
  x = ((1.0f + (-x).array().exp()).inverse()).matrix();
}

// Procedure: _relu
inline void DnnClassifier::_relu(Eigen::MatrixXf& x) const {
  for(int j=0; j<x.cols(); ++j) {
    for(int i=0; i<x.rows(); ++i) {
      if(x(i, j) <= 0.0f) {
        x(i, j) = 0.0f;
      }
    }
  }
}

// Procedure: _act
inline void DnnClassifier::_act(Eigen::MatrixXf& x, Activation a) const {

  switch(a) {
    case Activation::SIGMOID:
      _sigmoid(x);
    break;

    case Activation::TANH:
      _tanh(x);
    break;

    case Activation::RELU:
      _relu(x);
    break;

    default:
    break;
  }
}

// Function: _dact
inline Eigen::MatrixXf DnnClassifier::_dact(const Eigen::MatrixXf& x, Activation a) const {
  
  switch(a) {
    case Activation::SIGMOID:
      return _dsigmoid(x);
    break;

    case Activation::TANH:
      return _dtanh(x);
    break;

    case Activation::RELU:
      return _drelu(x);
    break;

    default:
      return Eigen::MatrixXf::Ones(x.rows(), x.cols());
    break;
  }

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
    
// Function: optimizer
template <typename O, typename... ArgsT>
DnnClassifier& DnnClassifier::optimizer(ArgsT&&... args) {
  _optimizer.emplace<O>(std::forward<ArgsT>(args)...); 
  return *this;
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
    DnnRegressor& error(Loss);

    template <typename O, typename... ArgsT>
    DnnRegressor& optimizer(ArgsT&&...);

    template <typename C>
    DnnRegressor& train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);
    
    Eigen::MatrixXf infer(const Eigen::MatrixXf&) const;

  private:
    
    Loss _loss {Loss::SOFTMAX_CROSS_ENTROPY};

    std::vector<DnnLayer> _L;
    std::vector<Eigen::MatrixXf> _X;
    std::vector<Eigen::MatrixXf> _W;
    std::vector<Eigen::MatrixXf> _dW;
    std::vector<Eigen::MatrixXf> _B;
    std::vector<Eigen::MatrixXf> _dB;

    Optimizer _optimizer;

    std::default_random_engine _gen {0};

    void _shuffle(Eigen::MatrixXf&, Eigen::MatrixXf&);
    void _fprop(const Eigen::MatrixXf&);
    void _bprop(Eigen::MatrixXf&);
    void _optimize(const Eigen::MatrixXf&, const Eigen::MatrixXf&, float);
    void _update(float);

    inline Eigen::MatrixXf _dact(const Eigen::MatrixXf&, Activation) const;
    inline Eigen::MatrixXf _dtanh(const Eigen::MatrixXf&) const;
    inline Eigen::MatrixXf _dsigmoid(const Eigen::MatrixXf&) const;
    inline Eigen::MatrixXf _drelu(const Eigen::MatrixXf&) const;

    inline void _tanh(Eigen::MatrixXf&) const;
    inline void _sigmoid(Eigen::MatrixXf&) const;
    inline void _relu(Eigen::MatrixXf&) const;
    inline void _act(Eigen::MatrixXf&, Activation) const;
    
    template <typename C>
    void _train(Eigen::MatrixXf&, Eigen::MatrixXf&, size_t, size_t, float, C&&);

};
    


};  // end of namespace dtc::ml -------------------------------------------------------------------



#endif
