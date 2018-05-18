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

#ifndef DTC_CELL_OPERATOR_HPP_
#define DTC_CELL_OPERATOR_HPP_

#include <dtc/kernel/graph.hpp>

namespace dtc::cell {

// Class: Operator1x1
template <typename F>
class Operator1x1 {

  using R = typename closure_traits<F>::result_type;
  using T = typename closure_traits<F>::decay_args;

  static_assert(is_std_variant_v<R>, "return type must be a variant containing Event::Signal");

  private:
    
    Graph& _graph;

    VertexBuilder _vertex;

    key_type _in {-1};

    PlaceHolder _out;

    F _op;

  public:
    
    Operator1x1(Graph&, F&&);

    Operator1x1(const Operator1x1&) = delete;
    Operator1x1(Operator1x1&&) = delete;
    Operator1x1& operator = (const Operator1x1&) = delete;
    Operator1x1& operator = (Operator1x1&&) = delete;
    
    operator key_type() const;
    
    key_type in() const;
    key_type in(auto&&);

    PlaceHolder& out();

};

// Constructor
template <typename F>
Operator1x1<F>::Operator1x1(Graph& graph, F&& f) : 
  _graph  {graph},
  _vertex {_graph.vertex()},
  _in     {-1},
  _out    {_vertex, {}},
  _op     {std::forward<F>(f)} {
}

// Operator
template <typename F>
Operator1x1<F>::operator key_type() const {
  return _vertex;
}

// Function: out
template <typename F>
PlaceHolder& Operator1x1<F>::out() {
  return _out;
}

// Function: in
template <typename F>
key_type Operator1x1<F>::in() const {
  return _in;
}

// Function: in
template <typename F>
key_type Operator1x1<F>::in(auto&& tail) {
  
  if(_in != -1) {
    DTC_THROW("Operator1x1:in already connected");
  }

  _in = _graph.stream(tail, _vertex).on([&] (Vertex& v, InputStream& is) mutable { 
    
    T tuple;

    while(is(tuple) != -1) {

      auto sig = std::visit(Functors{
        [] (Event::Signal sig) {
          return sig;
        },
        [&] (auto&& others) {
          v.broadcast_to(_out.keys(), others);        
          return Event::Signal::DEFAULT;
        } 
      }, std::apply(_op, tuple));

      if(sig == Event::Signal::REMOVE) {
        for(const auto& okey : _out.keys()) {
          v.remove_ostream(okey);
        }
        return sig;
      }
    }

    return Event::Signal::DEFAULT;
  });

  return _in;
}

// Deduction guide
template <typename F>
Operator1x1(Graph&, F&&) -> Operator1x1<F>;

// --------------------------------------------------------------------------------------

/*// Operator2x1
template <typename F>
class Operator2x1 {
  
  static_assert(closure_traits<F>::arity::value == 2, "Operator2x1 must take two arguments");
  
  using R  = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;
  using T1 = std::decay_t<typename closure_traits<F>::template arg<0>>;
  using T2 = std::decay_t<typename closure_traits<F>::template arg<1>>;

  private:

    Graph* const _graph {nullptr};

    VertexBuilder _vertex {-1};

    key_type _in1 {-1};
    key_type _in2 {-1};

    PlaceHolder _out;

    Event::Signal _sync(dtc::Vertex&) const;
    
    F _op;

  public:
    
    Operator2x1(Graph*, F&&);

    Operator2x1(const Operator2x1&) = delete;
    Operator2x1(Operator2x1&&) = delete;
    Operator2x1& operator = (const Operator2x1&) = delete;
    Operator2x1& operator = (Operator2x1&&) = delete;

    operator key_type() const;

    key_type in1() const;
    key_type in2() const;

    Operator2x1& in1(auto&&);
    Operator2x1& in2(auto&&);
    
    PlaceHolder& out();
};

// Constructor
template <typename F>
Operator2x1<F>::Operator2x1(Graph* graph, F&& op) : 
  _graph  {graph},
  _vertex {_graph->vertex()},
  _in1    {-1},
  _in2    {-1},
  _out    {_vertex, {}},
  _op     {std::forward<F>(op)} {
}

// Operator
template <typename F>
Operator2x1<F>::operator key_type() const {
  return _vertex;
}

// Function: out
template <typename F>
PlaceHolder& Operator2x1<F>::out() {
  return _out;
}

// Procedure: _sync
template <typename F>
Event::Signal Operator2x1<F>::_sync(Vertex& v) const {

  if(_in1 == -1 || _in2 == -1) {
    DTC_THROW("Both in1 and in2 must be connected in Operator2x1");
  }

  // Case 1: Lock both stream buffers and apply the operator.
  auto is1 = v.istream(_in1);
  auto is2 = v.istream(_in2);

  if(is1 && is2) {
    auto lock = LockStreamBuffer(is1->isbuf, is2->isbuf);
    T1 d1;
    T2 d2;
    while(*is1 && *is2) {
      (*is1)(d1);
      (*is2)(d2);
      if constexpr(std::is_same_v<void, R>) {
        _op(d1, d2);
      }
      else {
        if(R s = _op(d1, d2); s) {
          v.broadcast_to(_out.keys(), *s);
        }
      }
    }
    return dtc::Event::DEFAULT;
  }
  // Case 2: Close this stream if one another disappeared.
  else {
    for(const auto& k : _out.keys()) {
      v.remove_ostream(k);
    }
    return dtc::Event::REMOVE;
  }
}

// Function: in1
template <typename F>
key_type Operator2x1<F>::in1() const {
  return _in1;
}

// Function: in2
template <typename F>
key_type Operator2x1<F>::in2() const {
  return _in2;
}

// Function: in1
template <typename F>
Operator2x1<F>& Operator2x1<F>::in1(auto&& tail) {
  if(_in1 != -1) {
    DTC_THROW("Operator2x1:in1 already connected");
  }
  _in1 = _graph->stream(tail, _vertex).on([&] (Vertex& v, InputStream&) { return _sync(v); });
  return *this;
}

// Function: in2
template <typename F>
Operator2x1<F>& Operator2x1<F>::in2(auto&& tail) {
  if(_in2 != -1) {
    DTC_THROW("Operator2x1:in2 already connected");
  }
  _in2 = _graph->stream(tail, _vertex).on([&] (Vertex& v, InputStream&) { return _sync(v); });
  return *this;
}

// Deduction guide
template <typename F>
Operator2x1(Graph*, F&&) -> Operator2x1<F>;*/

};  // end of namespace dtc::cell. ----------------------------------------------------------------

#endif

