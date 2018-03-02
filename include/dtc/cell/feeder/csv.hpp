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

#ifndef DTC_CELL_FEEDER_CSV_HPP_
#define DTC_CELL_FEEDER_CSV_HPP_

#include <dtc/csv/csv.hpp>
#include <dtc/kernel/graph.hpp>

namespace dtc::cell {

// Class: CsvStreamFeeder
template <typename F>
class CsvStreamFeeder {

  static_assert(closure_traits<F>::arity::value == 1, "CsvStreamFeeder must take one argument");
  
  using T = std::decay_t<typename closure_traits<F>::template arg<0>>;
  using R = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;
  
  static_assert(is_eigen_matrix_v<T>);

  struct Storage {

    mutable std::ifstream ifstream;
    mutable std::string line;
    mutable std::string token;
    mutable std::stringstream lss;
    mutable std::stringstream ss;

    mutable size_t num_rows;
    mutable size_t num_cols;

    mutable T values;
    
    Storage() = default;
    Storage(const Storage&);
  };

  private:

    Graph* const _graph {nullptr};

    VertexBuilder _vertex;
    ProberBuilder _prober;

    PlaceHolder _out;
    
    F _op;

    Event::Signal _next_batch(Vertex&);

    char _delimiter {','};

    size_t _frequency {1};
    
  public:
    
    CsvStreamFeeder(Graph*, std::filesystem::path, F&&);
    
    CsvStreamFeeder(const CsvStreamFeeder&) = delete;
    CsvStreamFeeder(CsvStreamFeeder&&) = delete;
    CsvStreamFeeder& operator = (const CsvStreamFeeder&) = delete;
    CsvStreamFeeder& operator = (CsvStreamFeeder&&) = delete;

    operator key_type() const;

    PlaceHolder& out();

    template <typename D>
    CsvStreamFeeder& duration(D&&);

    CsvStreamFeeder& delimiter(char);
    CsvStreamFeeder& frequency(size_t);
};

// Storage copy constructor
template <typename F>
CsvStreamFeeder<F>::Storage::Storage(const Storage& rhs) : 
  ifstream {std::move(rhs.ifstream)},
  line     {std::move(rhs.line)},
  token    {std::move(rhs.token)},
  lss      {std::move(rhs.lss)},
  ss       {std::move(rhs.ss)},
  num_rows {rhs.num_rows},
  num_cols {rhs.num_cols},
  values   {std::move(rhs.values)} {
}

// Constructor
template <typename F>
CsvStreamFeeder<F>::CsvStreamFeeder(Graph* g, std::filesystem::path p, F&& f) :
  _graph  {g},
  _vertex {_graph->vertex()},
  _prober {_graph->prober(_vertex)},
  _out    {_vertex, {}},
  _op     {std::forward<F>(f)} {

  _vertex.on([this, _path=std::move(p)] (Vertex& v) { 
    
    auto& s = v.any.emplace<Storage>(); 

    std::tie(s.num_rows, s.num_cols) = csv_size(_path, _delimiter); 

    if(s.ifstream.open(_path); !s.ifstream.good()) {
      DTC_THROW("Failed to open ", _path);
    }

    s.values.resize(_frequency, s.num_cols);

  });

  _prober.on([this] (Vertex& v) { 
    return _next_batch(v); 
  });
}

// Operator
template <typename F>
CsvStreamFeeder<F>::operator key_type() const {
  return _vertex;
}

// Function: duration
template <typename F>
template <typename D>
CsvStreamFeeder<F>& CsvStreamFeeder<F>::duration(D&& d) {
  _prober.duration(std::forward<D>(d));
  return *this;
}

// Fucntion: delimiter    
template <typename F>
CsvStreamFeeder<F>& CsvStreamFeeder<F>::delimiter(char d) {
  _delimiter = d;
  return *this;
}

// Function: frequency
template <typename F>
CsvStreamFeeder<F>& CsvStreamFeeder<F>::frequency(size_t n) {
  _frequency = n;
  return *this;
} 

// Function: _next_batch
template <typename F>
Event::Signal CsvStreamFeeder<F>::_next_batch(Vertex& v) {

  Storage& s = std::any_cast<Storage&>(v.any);

  size_t row = 0;

  // Case 1: Read the csv to Eigen
  while(std::getline(s.ifstream, s.line)) {
    
    if(s.line.empty()) continue;
    
    s.lss.clear();
    s.lss.str(s.line);
    
    size_t col = 0;
    
    while(std::getline(s.lss, s.token, _delimiter)) {

      if(!std::is_signed_v<typename T::Scalar> && !s.token.empty() && s.token[0] == '-') {
        s.values(row, col) = 0;
      }
      else {

        s.ss.clear();
        s.ss.str(s.token);
        
        typename T::Scalar val = 0;
        s.ss >> val;

        if(!s.ss.fail()) {
          s.values(row, col) = val;
        }
        else {
          s.values(row, col) = to_nan_or_inf<typename T::Scalar>(s.token);
        }
      }
      ++col; 
    }
    ++row;

    if(row >= _frequency) break;
  }

  if(row < _frequency) {
    assert(!s.ifstream.good());
    s.values = s.values.middleRows(0, row);
  }
  
  if(s.values.rows() > 0) { 
    if constexpr(std::is_same_v<void, R>) {
      _op(s.values);
    }
    else {
      if(R dout = _op(s.values); dout) {
        v.broadcast_to(_out.keys(), *dout);
      }
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
PlaceHolder& CsvStreamFeeder<F>::out() {
  return _out;
} 

// Deduction guide
template <typename F>
CsvStreamFeeder(Graph*, std::filesystem::path, F&&) -> CsvStreamFeeder<F>;

};  // end of namespace dtc::cell. ----------------------------------------------------------------


#endif







