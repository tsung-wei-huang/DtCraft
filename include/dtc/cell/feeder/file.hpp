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

#ifndef DTC_CELL_FEEDER_FILE_HPP_
#define DTC_CELL_FEEDER_FILE_HPP_

#include <dtc/kernel/graph.hpp>

namespace dtc::cell {

/*// Class: FileStreamFeeder
template <typename F>
class FileStreamFeeder {
  
  static_assert(closure_traits<F>::arity::value == 1, "FileStreamFeeder must take one argument");
  
  using T = std::decay_t<typename closure_traits<F>::template arg<0>>;
  using R = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;

  struct Storage {

    mutable std::ifstream ifstream;
    
    Storage(auto&&);
    Storage(const Storage&);
  };

  private:

    Graph& _graph;

    VertexBuilder _vertex;
    ProberBuilder _prober;

    PlaceHolder _out;
    
    F _op;

    Event::Signal _ifstream(Vertex&);
    
  public:
    
    FileStreamFeeder(Graph*, auto&&, F&&);
    
    FileStreamFeeder(const FileStreamFeeder&) = delete;
    FileStreamFeeder(FileStreamFeeder&&) = delete;
    FileStreamFeeder& operator = (const FileStreamFeeder&) = delete;
    FileStreamFeeder& operator = (FileStreamFeeder&&) = delete;

    operator key_type() const;

    PlaceHolder& out();

    template <typename D>
    FileStreamFeeder& duration(D&&);
};

// Storage contructor
template <typename F>
FileStreamFeeder<F>::Storage::Storage(auto&& p) : 
  ifstream {p, std::ios_base::in} {
  
  if(!ifstream) {
    DTC_THROW("Failed to open ", p);
  }
}

// Storage copy constructor
template <typename F>
FileStreamFeeder<F>::Storage::Storage(const Storage& rhs) : ifstream {std::move(rhs.ifstream)} {
}

// Constructor
template <typename F>
FileStreamFeeder<F>::FileStreamFeeder(Graph* g, auto&& p, F&& f) :
  _graph  {g},
  _vertex {_graph.vertex()},
  _prober {_graph.prober(_vertex)},
  _out    {_vertex, {}},
  _op     {std::forward<F>(f)} {

  _vertex.on([p=std::forward<decltype(p)>(p)] (Vertex& v) { 
    v.any.emplace<Storage>(p); 
  });

  _prober.on([this] (Vertex& v) { 
    return _ifstream(v); 
  });
}

// Operator
template <typename F>
FileStreamFeeder<F>::operator key_type() const {
  return _vertex;
}

// Function: duration
template <typename F>
template <typename D>
FileStreamFeeder<F>& FileStreamFeeder<F>::duration(D&& d) {
  _prober.duration(std::forward<D>(d));
  return *this;
}

// Function: _getline
template <typename F>
Event::Signal FileStreamFeeder<F>::_ifstream(Vertex& v) {

  Storage& s = std::any_cast<Storage&>(v.any);

  if constexpr(std::is_same_v<void, R>) {
    _op(s.ifstream);
  }
  else {
    if(R dout = _op(s.ifstream); dout) {
      v.broadcast_to(_out.keys(), *dout);
    }
  }

  if(!s.ifstream.good()) {
    for(const auto& k : _out.keys()) {
      v.remove_ostream(k);
    }
    v.any.reset();
    return Event::REMOVE;
  }
    
  return Event::DEFAULT;
}

// Function: out
template <typename F>
PlaceHolder& FileStreamFeeder<F>::out() {
  return _out;
} 

// Deduction guide
template <typename F>
FileStreamFeeder(Graph*, auto&&, F&&) -> FileStreamFeeder<F>;

// ------------------------------------------------------------------------------------------------

// Class: TextStreamFeeder
template <typename F>
class TextStreamFeeder {

  static_assert(closure_traits<F>::arity::value == 1, "TextStreamFeeder must take one argument");
  
  using T = std::decay_t<typename closure_traits<F>::template arg<0>>;
  using R = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;

  struct Storage {

    mutable std::ifstream ifstream;
    mutable std::string buffer;
    
    Storage(auto&&);
    Storage(const Storage&);
  };

  private:

    Graph& _graph;

    VertexBuilder _vertex;
    ProberBuilder _prober;

    PlaceHolder _out;
    
    F _op;

    Event::Signal _next_line(Vertex&);

  public:
    
    TextStreamFeeder(Graph*, auto&&, F&&);
    
    TextStreamFeeder(const TextStreamFeeder&) = delete;
    TextStreamFeeder(TextStreamFeeder&&) = delete;
    TextStreamFeeder& operator = (const TextStreamFeeder&) = delete;
    TextStreamFeeder& operator = (TextStreamFeeder&&) = delete;

    operator key_type() const;

    PlaceHolder& out();

    template <typename D>
    TextStreamFeeder& duration(D&&);
};

// Storage contructor
template <typename F>
TextStreamFeeder<F>::Storage::Storage(auto&& p) : ifstream(p) {
  
  if(!ifstream) {
    DTC_THROW("Failed to open ", p);
  }
}

// Storage copy constructor
template <typename F>
TextStreamFeeder<F>::Storage::Storage(const Storage& rhs) : 
  ifstream {std::move(rhs.ifstream)},
  buffer {std::move(rhs.buffer)} {
}

// Constructor
template <typename F>
TextStreamFeeder<F>::TextStreamFeeder(Graph* g, auto&& p, F&& f) :
  _graph  {g},
  _vertex {_graph.vertex()},
  _prober {_graph.prober(_vertex)},
  _out    {_vertex, {}},
  _op     {std::forward<F>(f)} {

  _vertex.on([p=std::forward<decltype(p)>(p)] (Vertex& v) { 
    v.any.emplace<Storage>(p); 
  });

  _prober.on([this] (Vertex& v) { 
    return _next_line(v); 
  });
}

// Operator
template <typename F>
TextStreamFeeder<F>::operator key_type() const {
  return _vertex;
}

// Function: duration
template <typename F>
template <typename D>
TextStreamFeeder<F>& TextStreamFeeder<F>::duration(D&& d) {
  _prober.duration(std::forward<D>(d));
  return *this;
}

// Function: _next_line
template <typename F>
Event::Signal TextStreamFeeder<F>::_next_line(Vertex& v) {

  Storage& s = std::any_cast<Storage&>(v.any);

  while(std::getline(s.ifstream, s.buffer)) {

    if constexpr(std::is_same_v<void, R>) {
      _op(s.buffer);
    }
    else {
      if(R dout = _op(s.buffer); dout) {
        v.broadcast_to(_out.keys(), *dout);
      }
    }
    return Event::DEFAULT;
  }
  
  v.any.reset();

  for(const auto& k : _out.keys()) {
    v.remove_ostream(k);
  }

  return Event::REMOVE;
}

// Function: out
template <typename F>
PlaceHolder& TextStreamFeeder<F>::out() {
  return _out;
} 

// Deduction guide
template <typename F>
TextStreamFeeder(Graph*, auto&&, F&&) -> TextStreamFeeder<F>;
*/

};  // end of namespace dtc::cell. ----------------------------------------------------------------

#endif







