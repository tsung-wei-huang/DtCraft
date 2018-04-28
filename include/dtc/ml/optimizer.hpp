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

#ifndef DTC_ML_OPTIMIZER_HPP_
#define DTC_ML_OPTIMIZER_HPP_

#include <dtc/headerdef.hpp>

namespace dtc::ml {

// Class: AdamOptimizer
// Adaptive Moment Estimation (Adam) keeps separate learning rates for each weight 
// as well as an exponentially decaying average of previous gradients. This combines 
// elements of Momentum and Adagrad together and is fairly memory efficient since it 
// doesn’t keep a history of anything (just the rolling averages). It is reputed to work 
// well for both sparse matrices and noisy data.
class AdamOptimizer {
    
  struct MovingAverage {
    Eigen::MatrixXf mt;
    Eigen::MatrixXf vt;
    float b1_t {0.9f};
    float b2_t {0.999f};
  };

  private:
  
    float _alpha {0.001f};     // learning rate
    float _b1    {0.9f};       // decay term
    float _b2    {0.999f};     // decay term
    float _eps   {1e-8};    

    std::unordered_map<Eigen::MatrixXf*, MovingAverage> _states;

  public:

    AdamOptimizer() = default;

    inline AdamOptimizer& alpha(float);

    void update(const Eigen::MatrixXf&, Eigen::MatrixXf&);
};

// Function: alpha  
inline AdamOptimizer& AdamOptimizer::alpha(float v) { 
  _alpha = v; 
  return *this; 
}

// ------------------------------------------------------------------------------------------------

// Class: AdamaxOptimizer
class AdamaxOptimizer {
    
  struct MovingAverage {
    Eigen::MatrixXf mt;
    Eigen::MatrixXf ut;
    float b1_t {0.9f};
  };

  private:

    float _alpha {0.002f};     // learning rate
    float _b1    {0.9f};       // decay term
    float _b2    {0.999f};     // decay term
    float _eps   {1e-8};    

    std::unordered_map<Eigen::MatrixXf*, MovingAverage> _states;

  public:

    AdamaxOptimizer() = default;

    inline AdamaxOptimizer& alpha(float);

    void update(const Eigen::MatrixXf&, Eigen::MatrixXf&);

    //template <typename ArchiverT>
    //auto archive(ArchiverT&);
};

//// Function: archive
//template <typename ArchiverT>
//auto AdamaxOptimizer::archive(ArchiverT& ar) {
//  return ar(_alpha, _b1, _b2, _b1_t, _eps, _mts, _uts);
//}

// Function: alpha    
inline AdamaxOptimizer& AdamaxOptimizer::alpha(float v) { 
  _alpha = v; 
  return *this; 
}

// ------------------------------------------------------------------------------------------------

// Class: GradientDescentOptimizer
class GradientDescentOptimizer {

  private:
    
    float_t _alpha {0.01f};
    float_t _decay {0.0f}; 

  public:

    GradientDescentOptimizer() = default;

    GradientDescentOptimizer& decay(float v) { _decay = v; return *this; }
    GradientDescentOptimizer& alpha(float v) { _alpha = v; return *this; }

    void update(const Eigen::MatrixXf&, Eigen::MatrixXf&);

    //template <typename ArchiverT>
    //auto archive(ArchiverT&);
};

//// Function: archive
//template <typename ArchiverT>
//auto GradientDescentOptimizer::archive(ArchiverT& ar) {
//  return ar(_alpha, _decay);
//}

// ------------------------------------------------------------------------------------------------

// Class: AdagradOptimizer
class AdagradOptimizer {

  private:

    float _alpha {0.01f};
    float _eps {1e-8};
    
    std::unordered_map<Eigen::MatrixXf*, Eigen::MatrixXf> _states;
    
  public:

    AdagradOptimizer() = default;

    AdagradOptimizer& alpha(float v) { _alpha = v; return *this; }

    void update(const Eigen::MatrixXf&, Eigen::MatrixXf&);

    //template <typename ArchiverT>
    //auto archive(ArchiverT&);
};

//// Function: archive
//template <typename ArchiverT>
//auto AdagradOptimizer::archive(ArchiverT& ar) {
//  return ar(_alpha, _eps, _gs);
//}

// ------------------------------------------------------------------------------------------------

// Class: RMSprobOptimizer
// RMSprop is similar to Adam it just uses different moving averages but has the same goals.
class RMSpropOptimizer {

  private:

    float _alpha {0.0001f};
    float _mu {0.99f};
    float _eps {1e-8};

    std::unordered_map<Eigen::MatrixXf*, Eigen::MatrixXf> _states;

  public:

    RMSpropOptimizer() = default;
    
    RMSpropOptimizer& alpha(float v) { _alpha = v; return *this; }

    void update(const Eigen::MatrixXf&, Eigen::MatrixXf&);

    //template <typename ArchiverT>
    //auto archive(ArchiverT&);
};

//// Function: archive
//template <typename ArchiverT>
//auto RMSpropOptimizer::archive(ArchiverT& ar) {
//  return ar(_alpha, _mu, _eps, _gs);
//}

// ------------------------------------------------------------------------------------------------

// Class: MomentumOptimizer
// If gradient descent is navigating down a valley with steep sides, it tends to madly oscillate 
// from one valley wall to the other without making much progress down the valley. 
// This is because the largest gradients point up and down the valley walls whereas the gradient 
// along the floor of the valley is quite small. Momentum ptimization attempts to remedy this by 
// keeping track of the prior gradients and if they keep changing direction then damp them, and if 
// the gradients stay in the same direction then reward them. This way the valley wall gradients 
// get reduced and the valley floor gradient enhanced. 
class MomentumOptimizer {

  private:

    float _alpha {0.01f};
    float _lambda {0};
    float _mu {0.9};

    std::unordered_map<Eigen::MatrixXf*, Eigen::MatrixXf> _states;

  public:

    MomentumOptimizer() = default;

    MomentumOptimizer& alpha(float v) { _alpha = v; return *this; }

    void update(const Eigen::MatrixXf&, Eigen::MatrixXf&);

    //template <typename ArchiverT>
    //auto archive(ArchiverT&);
};

//// Function: archive
//template <typename ArchiverT>
//auto MomentumOptimizer::archive(ArchiverT& ar) {
//  return ar(_alpha, _lambda, _mu, _dW_prevs);
//}

// ------------------------------------------------------------------------------------------------

// Optimizer
using Optimizer = std::variant<
  GradientDescentOptimizer,
  AdagradOptimizer,
  AdamOptimizer,
  AdamaxOptimizer,
  RMSpropOptimizer,
  MomentumOptimizer
>;


};  // end of namespace dtc::ml -------------------------------------------------------------------


#endif


