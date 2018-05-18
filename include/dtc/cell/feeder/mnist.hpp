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
class MnistStreamFeeder {

  struct Storage {

    mutable Eigen::MatrixXf images;
    mutable Eigen::VectorXi labels;

    int cursor {0};

    Storage(const std::filesystem::path&, const std::filesystem::path&);
    Storage(const Storage&);
  };

  private:
    
    Graph& _graph;

    VertexBuilder _vertex;

    key_type _in {-1};
    PlaceHolder _out;

    Event::Signal _next_batch(Vertex&, InputStream&);
    void _shuffle(Eigen::MatrixXf&, Eigen::VectorXi&);

  public:

    MnistStreamFeeder(Graph&, std::filesystem::path, std::filesystem::path);
    
    MnistStreamFeeder(const MnistStreamFeeder&) = delete;
    MnistStreamFeeder(MnistStreamFeeder&&) = delete;
    MnistStreamFeeder& operator = (const MnistStreamFeeder&) = delete;
    MnistStreamFeeder& operator = (MnistStreamFeeder&&) = delete;

    PlaceHolder& out();

    operator key_type () const;

    key_type in() const;
    key_type in(auto&&);
};

// Function: in
key_type MnistStreamFeeder::in(auto&& tail) {
  
  if(_in != -1) {
    DTC_THROW("MnistStreamFeeder:in already connected");
  }

  _in = _graph.stream(tail, _vertex).on([&] (Vertex& v, InputStream& is) mutable { 
    return _next_batch(v, is);
  });

  return _in;
}


};  // end of namespace dtc::cell. ----------------------------------------------------------------


#endif







