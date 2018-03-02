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

#ifndef DTC_CELL_VISITOR_HPP_
#define DTC_CELL_VISITOR_HPP_

#include <dtc/kernel/graph.hpp>

namespace dtc::cell {

// Class: Visitor1x1
template <typename P, typename F>
class Visitor1x1 {

  static_assert(closure_traits<P>::arity::value == 1);
  static_assert(closure_traits<F>::arity::value == 2);
 
  using R = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;
  using T = std::decay_t<typename closure_traits<P>::template arg<0>>;
  using D = std::decay_t<typename closure_traits<F>::template arg<1>>;
  
  private:

    struct Storage {
      mutable std::unique_ptr<T> object;

      template <typename... ArgsT>
      Storage(ArgsT&&...);

      Storage(const Storage&);
    };

    Graph* const _graph {nullptr};

    VertexBuilder _vertex;

    key_type _in {-1};

    PlaceHolder _out;
    
    P _init;
    F _visit;

  public:
    
    template <typename... ArgsT>
    Visitor1x1(Graph*, P&&, F&&, ArgsT&&...);

    Visitor1x1(const Visitor1x1&) = delete;
    Visitor1x1(Visitor1x1&&) = delete;
    Visitor1x1& operator = (const Visitor1x1&) = delete;
    Visitor1x1& operator = (Visitor1x1&&) = delete;
    
    operator key_type() const;
    
    key_type in(auto&&);

    PlaceHolder& out();
};

// Constructor
template <typename P, typename F>
template <typename... ArgsT>
Visitor1x1<P, F>::Storage::Storage(ArgsT&&... args) :
  object {std::make_unique<T>(std::forward<ArgsT>(args)...)} {
}

// Copy constructor
template <typename P, typename F>
Visitor1x1<P, F>::Storage::Storage(const Storage& rhs) : 
  object {std::move(rhs.object)} {
}

// Constructor
template <typename P, typename F>
template <typename... ArgsT>
Visitor1x1<P, F>::Visitor1x1(Graph* graph, P&& p, F&& f, ArgsT&&... args) : 
  _graph  {graph},
  _vertex {_graph->vertex()},
  _in     {-1},
  _out    {_vertex, {}},
  _init   {std::forward<P>(p)},
  _visit  {std::forward<F>(f)} {
  
  _vertex.on([this, args...] (Vertex& v) {
    auto& s = v.any.emplace<Storage>(args...);
    _init(*s.object);
  });
}

// Operator
template <typename P, typename F>
Visitor1x1<P, F>::operator key_type() const {
  return _vertex;
}

// Function: out
template <typename P, typename F>
PlaceHolder& Visitor1x1<P, F>::out() {
  return _out;
}

// Function: in
template <typename P, typename F>
key_type Visitor1x1<P, F>::in(auto&& tail) {
  
  if(_in != -1) {
    DTC_THROW("Visitor1x1:in already connected");
  }

  return _in = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 
    
    auto& s = std::any_cast<Storage&>(v.any);

    D din;

    while(is(din) != -1) {
      if constexpr(std::is_same_v<void, R>) {
        _visit(*s.object, din);
      }
      else {
        if(R dout = _visit(*s.object, din); dout) {
          v.broadcast_to(_out.keys(), *dout);
        }
      }
    }
    return Event::DEFAULT;
  });
}

// Deduction guide
template <typename P, typename F>
Visitor1x1(Graph&, P&&, F&&) -> Visitor1x1<P, F>;

//-------------------------------------------------------------------------------------------------

// Class: Visitor2x1
template <typename P, typename F1, typename F2>
class Visitor2x1 {
  
  static_assert(closure_traits<P >::arity::value == 1, "Visitor2x1 must take one argument");
  static_assert(closure_traits<F1>::arity::value == 2, "Visitor2x1:in1 must take two arguments");
  static_assert(closure_traits<F2>::arity::value == 2, "Visitor2x1:in2 must take two arguments");

  using R1 = add_optionality_t<std::decay_t<typename closure_traits<F1>::result_type>>;
  using R2 = add_optionality_t<std::decay_t<typename closure_traits<F2>::result_type>>;
  using T  = std::decay_t<typename closure_traits<P >::template arg<0>>;
  using T1 = std::decay_t<typename closure_traits<F1>::template arg<0>>;
  using T2 = std::decay_t<typename closure_traits<F2>::template arg<0>>;
  using D1 = std::decay_t<typename closure_traits<F1>::template arg<1>>;
  using D2 = std::decay_t<typename closure_traits<F2>::template arg<1>>;
  
  static_assert(std::is_same_v<T,  T1> && std::is_same_v<T, T2>, "Object to visit must have the same type");
  static_assert(std::is_same_v<R1, R2>, "Object to return must have the same type");

  struct Storage {
    mutable std::unique_ptr<T> object;
    mutable std::mutex mutex;

    template <typename... ArgsT>
    Storage(ArgsT&&...);

    Storage(const Storage&);
  };

  private:
    
    Graph* const _graph {nullptr};

    VertexBuilder _vertex;

    key_type _in1 {-1};
    key_type _in2 {-1};

    PlaceHolder _out;
    
    P _init;
    F1 _visit1;
    F2 _visit2;
    
    bool _synchronized {true};

    R1 _sync1(std::mutex&, T&, D1&);
    R2 _sync2(std::mutex&, T&, D2&);

    std::unique_lock<std::mutex> _lock(std::mutex&) const;

  public:
    
    template <typename... ArgsT>
    Visitor2x1(Graph*, P&&, F1&&, F2&&, ArgsT&&...);

    Visitor2x1(const Visitor2x1&) = delete;
    Visitor2x1(Visitor2x1&&) = delete;
    Visitor2x1& operator = (const Visitor2x1&) = delete;
    Visitor2x1& operator = (Visitor2x1&&) = delete;
    
    operator key_type() const;

    key_type in1() const;
    key_type in2() const;
    
    PlaceHolder& out();
    
    Visitor2x1& in1(auto&&);
    Visitor2x1& in2(auto&&);
    Visitor2x1& synchronized(bool);

};

// Constructor
template <typename P, typename F1, typename F2>
template <typename... ArgsT>
Visitor2x1<P, F1, F2>::Storage::Storage(ArgsT&&... args) :
  object {std::make_unique<T>(std::forward<ArgsT>(args)...)} {
}

// Copy constructor
template <typename P, typename F1, typename F2>
Visitor2x1<P, F1, F2>::Storage::Storage(const Storage& rhs) : 
  object {std::move(rhs.object)} {
}

// Constructor
template <typename P, typename F1, typename F2>
template <typename... ArgsT>
Visitor2x1<P, F1, F2>::Visitor2x1(Graph* g, P&& p, F1&& f1, F2&& f2, ArgsT&&... args) :
  _graph  {g},
  _vertex {_graph->vertex()},
  _in1    {-1},
  _in2    {-1},
  _out    {_vertex, {}},
  _init   {std::forward<P>(p)},
  _visit1 {std::forward<F1>(f1)},
  _visit2 {std::forward<F2>(f2)} {
  
  _vertex.on([this, args...] (Vertex& v) {
    auto& s = v.any.emplace<Storage>(args...);
    _init(*s.object);
  });
}

// Operator
template <typename P, typename F1, typename F2>
Visitor2x1<P, F1, F2>::operator key_type () const {
  return _vertex;
}

// Function: out
template <typename P, typename F1, typename F2>
PlaceHolder& Visitor2x1<P, F1, F2>::out() {
  return _out;
}

// Function: in1
template <typename P, typename F1, typename F2>
key_type Visitor2x1<P, F1, F2>::in1() const {
  return _in1;
}

// Function: in2
template <typename P, typename F1, typename F2>
key_type Visitor2x1<P, F1, F2>::in2() const {
  return _in2;
}

// Function: synchronize
template <typename P, typename F1, typename F2>
Visitor2x1<P, F1, F2>& Visitor2x1<P, F1, F2>::synchronized(bool flag) {
  _synchronized = flag;
  return *this;
}

// Function: _sync1
template <typename P, typename F1, typename F2>
typename Visitor2x1<P, F1, F2>::R1 Visitor2x1<P, F1, F2>::_sync1(std::mutex& mtx, T& obj, D1& d1) {
  auto lock = _synchronized ? std::unique_lock(mtx) : std::unique_lock(mtx, std::defer_lock);
  return _visit1(obj, d1);
}

// Function: _sync2
template <typename P, typename F1, typename F2>
typename Visitor2x1<P, F1, F2>::R2 Visitor2x1<P, F1, F2>::_sync2(std::mutex& mtx, T& obj, D2& d2) {
  auto lock = _synchronized ? std::unique_lock(mtx) : std::unique_lock(mtx, std::defer_lock);
  return _visit2(obj, d2);
}


// Function: in1
template <typename P, typename F1, typename F2>
Visitor2x1<P, F1, F2>& Visitor2x1<P, F1, F2>::in1(auto&& tail) {

  if(_in1 != -1) {
    DTC_THROW("Visitor2x1:in1 already connected");
  }

  _in1 = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) { 

    auto& s = std::any_cast<Storage&>(v.any);

    D1 din;

    while(is(din) != -1) {
      if constexpr(std::is_same_v<void, R1>) {
        _sync1(s.mutex, *s.object, din);
      }
      else {
        if(R1 dout = _sync1(s.mutex, *s.object, din); dout) {
          v.broadcast_to(_out.keys(), *dout);
        }
      }
    }
    return Event::DEFAULT;
  });

  return *this;
}

// Function: in2
template <typename P, typename F1, typename F2>
Visitor2x1<P, F1, F2>& Visitor2x1<P, F1, F2>::in2(auto&& tail) {
  
  if(_in2 != -1) {
    DTC_THROW("Visitor2x1:in2 already connected");
  }

  _in2 = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 
    
    auto& s = std::any_cast<Storage&>(v.any);

    D2 din;

    while(is(din) != -1) {
      if constexpr(std::is_same_v<void, R2>) {
        _sync2(s.mutex, *s.object, din);
      }
      else {
        if(R2 dout = _sync2(s.mutex, *s.object, din); dout) {
          v.broadcast_to(_out.keys(), *dout);
        }
      }
    }
    return Event::DEFAULT;
  });

  return *this;
}

// Deduction guide
template <typename P, typename F1, typename F2, typename... ArgsT>
Visitor2x1(Graph&, P&&, F1&&, F2&&, ArgsT&&...) -> Visitor2x1<P, F1, F2>;

// ------------------------------------------------------------------------------------------------

/*// Class: Visitor2x2
template <typename F1, typename F2>
class Visitor2x2 {

  static_assert(closure_traits<F1>::arity::value == 2, "Visitor2x2 must take two arguments");
  static_assert(closure_traits<F2>::arity::value == 2, "Visitor2x2 must take two arguments");
 
  using R1 = add_optionality_t<std::decay_t<typename closure_traits<F1>::result_type>>;
  using R2 = add_optionality_t<std::decay_t<typename closure_traits<F2>::result_type>>;
  using T1 = std::decay_t<typename closure_traits<F1>::template arg<0>>;
  using T2 = std::decay_t<typename closure_traits<F2>::template arg<0>>;
  using D1 = std::decay_t<typename closure_traits<F1>::template arg<1>>;
  using D2 = std::decay_t<typename closure_traits<F2>::template arg<1>>;

  static_assert(std::is_same_v<T1, T2>, "Object to visit must have the same type");

  using T = T1;
  
  private:

    Graph* const _graph {nullptr};

    VertexBuilder _vertex;

    key_type _in1 {-1};
    key_type _in2 {-1};

    PlaceHolder _out1;
    PlaceHolder _out2;

    F1 _op1;
    F2 _op2;

    bool _synchronized {true};

    std::mutex _mutex;

    R1 _sync1(T&, D1&);
    R2 _sync2(T&, D2&);

  public:
    
    template <typename... ArgsT>
    Visitor2x2(Graph*, F1&&, F2&&, ArgsT&&...);

    Visitor2x2(const Visitor2x2&) = delete;
    Visitor2x2(Visitor2x2&&) = delete;
    Visitor2x2& operator = (const Visitor2x2&) = delete;
    Visitor2x2& operator = (Visitor2x2&&) = delete;
    
    operator key_type() const;

    key_type in1() const;
    key_type in2() const;
    
    PlaceHolder& out1();
    PlaceHolder& out2();
    
    Visitor2x2& in1(auto&&);
    Visitor2x2& in2(auto&&);
    Visitor2x2& synchronized(bool);
};

// Constructor
template <typename F1, typename F2>
template <typename... ArgsT>
Visitor2x2<F1, F2>::Visitor2x2(Graph* graph, F1&& f1, F2&& f2, ArgsT&&... args) : 
  _graph  {graph},
  _vertex {_graph->vertex()},
  _in1    {-1},
  _in2    {-1},
  _out1   {_vertex, {}},
  _out2   {_vertex, {}},
  _op1    {std::forward<F1>(f1)},
  _op2    {std::forward<F2>(f2)} {

  _vertex.on([tpl=std::make_tuple(std::forward<ArgsT>(args)...)] (Vertex& v) mutable {
    v.any = std::make_from_tuple<T>(std::move(tpl));
  });
}

// Operator
template <typename F1, typename F2>
Visitor2x2<F1, F2>::operator key_type() const {
  return _vertex;
}
    
// Function: out1
template <typename F1, typename F2>
PlaceHolder& Visitor2x2<F1, F2>::out1() {
  return _out1;
}

// Function: out2
template <typename F1, typename F2>
PlaceHolder& Visitor2x2<F1, F2>::out2() {
  return _out2;
}

// Function: in1
template <typename F1, typename F2>
key_type Visitor2x2<F1, F2>::in1() const {
  return _in1;
}

// Function: in2
template <typename F1, typename F2>
key_type Visitor2x2<F1, F2>::in2() const {
  return _in2;
}

// Function: _sync1
template <typename F1, typename F2>
typename Visitor2x2<F1, F2>::R1 Visitor2x2<F1, F2>::_sync1(T& obj, D1& d1) {
  auto lock = _synchronized ? std::unique_lock(_mutex) : std::unique_lock(_mutex, std::defer_lock);
  return _op1(obj, d1);
}

// Function: _sync2
template <typename F1, typename F2>
typename Visitor2x2<F1, F2>::R2 Visitor2x2<F1, F2>::_sync2(T& obj, D2& d2) {
  auto lock = _synchronized ? std::unique_lock(_mutex) : std::unique_lock(_mutex, std::defer_lock);
  return _op2(obj, d2);
}

// Function: in1
template <typename F1, typename F2>
Visitor2x2<F1, F2>& Visitor2x2<F1, F2>::in1(auto&& tail) {
  
  if(_in1 != -1) {
    DTC_THROW("Visitor2x2:in1 already connected");
  }

  _in1 = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 
    D1 din;
    while(is(din) != -1) {
      if constexpr(std::is_same_v<void, R1>) {
        _sync1(std::any_cast<T&>(v.any), din);
      }
      else {
        if(R1 dout = _sync1(std::any_cast<T&>(v.any), din); dout) {
          v.broadcast_to(_out1.keys(), *dout);
        }
      }
    }
    return Event::DEFAULT;
  });

  return *this;
}

// Function: in2
template <typename F1, typename F2>
Visitor2x2<F1, F2>& Visitor2x2<F1, F2>::in2(auto&& tail) {
  
  if(_in2 != -1) {
    DTC_THROW("Visitor2x2:in2 already connected");
  }

  _in2 = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 
    D2 din;
    while(is(din) != -1) {
      if constexpr(std::is_same_v<void, R2>) {
        _sync2(std::any_cast<T&>(v.any), din);
      }
      else {
        if(R2 dout = _sync2(std::any_cast<T&>(v.any), din); dout) {
          v.broadcast_to(_out2.keys(), *dout);
        }
      }
    }
    return Event::DEFAULT;
  });

  return *this;
}

// Function: synchronized
template <typename F1, typename F2>
Visitor2x2<F1, F2>& Visitor2x2<F1, F2>::synchronized(bool flag) {
  _synchronized = flag;
  return *this;
}

// Deduction guide
template <typename F1, typename F2, typename... ArgsT>
Visitor2x2(Graph*, F1&&, F2&&, ArgsT&&...) -> Visitor2x2<F1, F2>; */

};  // end of namespace dtc::cell. ----------------------------------------------------------------

#endif











