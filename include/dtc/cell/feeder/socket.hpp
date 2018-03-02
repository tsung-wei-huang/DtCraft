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

#ifndef DTC_CELL_FEEDER_SOCKET_HPP_
#define DTC_CELL_FEEDER_SOCKET_HPP_

#include <dtc/csv/csv.hpp>
#include <dtc/kernel/graph.hpp>

namespace dtc::cell {

/*// Class: SocketStreamFeeder
template <typename F>
class SocketStreamFeeder {

  static_assert(closure_traits<F>::arity::value == 1, "SocketStreamFeeder must take one argument");
  
  using T = std::decay_t<typename closure_traits<F>::template arg<0>>;
  using R = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;
  
  private:

    Graph* const _graph {nullptr};

    VertexBuilder _vertex;
    ProberBuilder _prober;

    PlaceHolder _out;
    
    F _op;

    void _connect(const std::string&, const std::string&) const;

  public:
    
    SocketStreamFeeder(Graph*, std::string, std::string, F&&);
    
    SocketStreamFeeder(const SocketStreamFeeder&) = delete;
    SocketStreamFeeder(SocketStreamFeeder&&) = delete;
    SocketStreamFeeder& operator = (const SocketStreamFeeder&) = delete;
    SocketStreamFeeder& operator = (SocketStreamFeeder&&) = delete;

    operator key_type() const;

    PlaceHolder& out();

    template <typename D>
    SocketStreamFeeder& duration(D&&);

};

// Constructor
template <typename F>
SocketStreamFeeder<F>::SocketStreamFeeder(Graph* graph, std::string host, std::string port, F&& op) :
  _graph  {graph},
  _vertex {_graph->vertex()},
  _prober {_graph->prober(_vertex)},
  _out    {_vertex, {}},
  _op     {std::forward<F>(op)} {

  _vertex.on([this, host=std::move(host), port=std::move(port)](){

  });

} */


};  // end of namespace dtc::cell -----------------------------------------------------------------


#endif
