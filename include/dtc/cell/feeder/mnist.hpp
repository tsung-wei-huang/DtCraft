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

#ifndef DTC_CELL_FEEDER_MNIST_HPP_
#define DTC_CELL_FEEDER_MNIST_HPP_

#include <dtc/kernel/graph.hpp>
#include <dtc/ml/mnist.hpp>

namespace dtc::cell {

// Class: MnistStreamFeeder
template <typename F>
class MnistStreamFeeder {

  using R = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;

  static_assert(closure_traits<F>::arity::value == 2);

  using M = std::decay_t<typename closure_traits<F>::template arg<0>>;
  using L = std::decay_t<typename closure_traits<F>::template arg<1>>;

  static_assert(
    (is_eigen_matrix_v<M> && is_eigen_matrix_v<L>)
  );

  struct Storage {

    mutable M images;
    mutable L labels;

    int cursor {0};

    Storage(auto&&, auto&&);
    Storage(const Storage&);
  };

  private:
    
    Graph* const _graph {nullptr};

    VertexBuilder _vertex;
    ProberBuilder _prober;

    PlaceHolder _out;

    F _op;

    size_t _frequency {1};

    Event::Signal _next_batch(Vertex& v);

  public:

    MnistStreamFeeder(Graph*, auto&&, auto&&, F&&);
    
    MnistStreamFeeder(const MnistStreamFeeder&) = delete;
    MnistStreamFeeder(MnistStreamFeeder&&) = delete;
    MnistStreamFeeder& operator = (const MnistStreamFeeder&) = delete;
    MnistStreamFeeder& operator = (MnistStreamFeeder&&) = delete;
    
    operator key_type() const;

    PlaceHolder& out();

    template <typename D>
    MnistStreamFeeder& duration(D&&);
    
    MnistStreamFeeder& frequency(size_t);
};

// Constructor
template <typename F>
MnistStreamFeeder<F>::Storage::Storage(auto&& m, auto&& l) :
  images {ml::read_mnist_image<M>(m)},
  labels {ml::read_mnist_label<L>(l)},
  cursor {0} {
  
  assert(images.rows() == labels.rows());
}

// Constructor
template <typename F>
MnistStreamFeeder<F>::Storage::Storage(const Storage& rhs) :
  images {std::move(rhs.images)},
  labels {std::move(rhs.labels)},
  cursor {rhs.cursor} {
}

// Constructor
template <typename F>
MnistStreamFeeder<F>::MnistStreamFeeder(Graph* g, auto&& m, auto&& l, F&& op) : 
  _graph  {g},
  _vertex {_graph->vertex()},
  _prober {_graph->prober(_vertex)},
  _out    {_vertex, {}},
  _op     {std::forward<F>(op)}
{
  _vertex.on(
    [this, m=std::forward<decltype(m)>(m), l=std::forward<decltype(l)>(l)] (Vertex& v) { 
      v.any.emplace<Storage>(m, l);
    }
  );
  
  _prober.on([this] (Vertex& v) { 
    return _next_batch(v); 
  });
}

// Operator
template <typename F>
MnistStreamFeeder<F>::operator key_type() const {
  return _vertex;
}

// Function: duration
template <typename F>
template <typename D>
MnistStreamFeeder<F>& MnistStreamFeeder<F>::duration(D&& d) {
  _prober.duration(std::forward<D>(d));
  return *this;
}

// Function: out
template <typename F>
PlaceHolder& MnistStreamFeeder<F>::out() {
  return _out;
} 

// Function: frequency
template <typename F>
MnistStreamFeeder<F>& MnistStreamFeeder<F>::frequency(size_t n) {
  _frequency = n;
  return *this;
} 

// Function: _next_batch
template <typename F>
Event::Signal MnistStreamFeeder<F>::_next_batch(Vertex& v) {

  Storage& s = std::any_cast<Storage&>(v.any);
 
  size_t N = s.images.rows();
  size_t b = s.cursor + _frequency < N ? _frequency : N - s.cursor;

  M m = s.images.middleRows(s.cursor, b);
  L l = s.labels.middleRows(s.cursor, b);

  if constexpr(std::is_same_v<void, R>) {
    _op(m, l);
  }
  else {
    if(R dout = _op(m, l); dout) {
      v.broadcast_to(_out.keys(), *dout);
    }
  }
  
  s.cursor += b;

  if(s.cursor == s.images.rows()) { 
    for(const auto& k : _out.keys()) {
      v.remove_ostream(k);
    }
    v.any.reset();
    return Event::REMOVE;
  }
  
  return Event::DEFAULT;
}

// ------------------------------------------------------------------------------------------------


};  // end of namespace dtc::cell. ----------------------------------------------------------------


#endif







