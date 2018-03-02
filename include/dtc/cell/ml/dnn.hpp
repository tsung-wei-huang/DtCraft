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

#ifndef DTC_CELL_ML_DNN_HPP_
#define DTC_CELL_ML_DNN_HPP_

#include <dtc/ml/classifier.hpp>

namespace dtc::cell {

/*// Class: DnnClassifier
template <typename F>
class DnnClassifier {

  static_assert(closure_traits<F>::arity::value == 1, "DnnClassifier must take one argument");
  
  private:

  struct Storage {

    mutable ml::DnnClassifier dnnc;
    mutable std::mutex mutex;
    
    Storage() = default;
    Storage(const Storage&);
  };

    Graph* const _graph {nullptr};

    VertexBuilder _vertex;

    key_type _train {-1};
    key_type _infer {-1};

    PlaceHolder _label;
    PlaceHolder _model;

    F _init;

  public:
    
    DnnClassifier(Graph*, F&&);

    DnnClassifier(const DnnClassifier&) = delete;
    DnnClassifier(DnnClassifier&&) = delete;
    DnnClassifier& operator = (const DnnClassifier&) = delete;
    DnnClassifier& operator = (DnnClassifier&&) = delete;
    
    operator key_type() const;
    
    key_type train() const;
    key_type infer() const;

    DnnClassifier& train(auto&&);
    DnnClassifier& infer(auto&&);

    PlaceHolder& label();
    PlaceHolder& model();
};

// Copy constructor
template <typename F>
DnnClassifier<F>::Storage::Storage(const Storage& rhs) : 
  dnnc {std::move(rhs.dnnc)} {
}

// Constructor
template <typename F>
DnnClassifier<F>::DnnClassifier(Graph* graph, F&& f) : 
  _graph  {graph},
  _vertex {_graph->vertex()},
  _train  {-1},
  _infer  {-1},
  _label  {_vertex, {}},
  _model  {_vertex, {}},
  _init   {std::forward<F>(f)} {

  _vertex.on([this] (Vertex& v) {
    _init(v.any.emplace<Storage>().dnnc);
  });
}

// Operator
template <typename F>
DnnClassifier<F>::operator key_type() const {
  return _vertex;
}

// Function: label
template <typename F>
PlaceHolder& DnnClassifier<F>::label() {
  return _label;
}

// Function: model
template <typename F>
PlaceHolder& DnnClassifier<F>::model() {
  return _model;
}

// Function: train
template <typename F>
key_type DnnClassifier<F>::train() const {
  return _train;
}

// Function: infer
template <typename F>
key_type DnnClassifier<F>::infer() const {
  return _infer;
}

// Function: train
template <typename F>
DnnClassifier<F>& DnnClassifier<F>::train(auto&& tail) {
  
  if(_train != -1) {
    DTC_THROW("DnnClassifier:train already connected");
  }

  _train = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 

    auto& s = std::any_cast<Storage&>(v.any);

    Eigen::MatrixXf f;
    Eigen::VectorXi l;

    while(is(f, l) != -1) {
      std::unique_lock lock(s.mutex);
      s.dnnc.train(f, l);
    }

    return Event::DEFAULT;
  });

  return *this;
}

// Function: infer
template <typename F>
DnnClassifier<F>& DnnClassifier<F>::infer(auto&& tail) {
  
  if(_infer != -1) {
    DTC_THROW("DnnClassifier:infer already connected");
  }

  _infer = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 

    auto& s = std::any_cast<Storage&>(v.any);

    Eigen::MatrixXf f;

    while(is(f) != -1) {
      std::unique_lock lock(s.mutex);
      Eigen::VectorXi l = s.dnnc.infer(f);
      v.broadcast_to(_label.keys(), l);
    }

    return Event::DEFAULT;
  });

  return *this;
}

// Deduction guide
template <typename F>
DnnClassifier(Graph*, F&&) -> DnnClassifier<F>; */

// ------------------------------------------------------------------------------------------------


};  // end of namespace dtc::cell .----------------------------------------------------------------

#endif
