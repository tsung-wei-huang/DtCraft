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

#ifndef DTC_CONTRIB_DEBS18_FEEDER_HPP_
#define DTC_CONTRIB_DEBS18_FEEDER_HPP_

#include <dtc/kernel/graph.hpp>

namespace dtc::debs18 {

// Class: StreamFeeder
//
// Infer send: a taskid and a string of ten fields
// Train send: a taskid and a string of two fields (correct)
// Verify get: a taskid and a string of two fields (predicted)
// 
// Each field is separated by a comma.
//
class StreamFeeder {

  struct Storage {
    
    mutable std::unordered_map<std::string, std::queue<std::string>> infers;
    mutable std::unordered_map<std::string, std::string> golden;
    
    size_t cursor    {0};
    size_t total     {0};
    size_t evaluated {0};
    double query1    {0};
    double query2    {0};
    
    Storage() = default;
    Storage(const Storage&);
  };

  private:
    
    Graph* const _graph {nullptr};

    VertexBuilder _vertex;
    
    key_type _in {-1};

    PlaceHolder _out;

    bool _verbose  {true};

    void _send_infer(Vertex&, const std::string&) const;
    void _evaluate(Vertex&, const std::string&, const std::string&) const;

    std::string _make_infer(const std::string&) const;
    std::string _make_train(const std::string&) const;


  public:

    StreamFeeder(Graph*, std::filesystem::path);
    
    StreamFeeder(const StreamFeeder&) = delete;
    StreamFeeder(StreamFeeder&&) = delete;
    StreamFeeder& operator = (const StreamFeeder&) = delete;
    StreamFeeder& operator = (StreamFeeder&&) = delete;
    
    operator key_type() const;

    key_type in() const;

    PlaceHolder& out();
    

    StreamFeeder& verbose(bool);
    
    template <typename T>
    StreamFeeder& in(T&&);
};

// Function: in
template <typename T>
StreamFeeder& StreamFeeder::in(T&& tail) {

  if(_in != -1) {
    DTC_THROW("StreamFeeder:in already connected");
  }
  
  _in = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 

    auto& s = std::any_cast<Storage&>(v.any);

    std::string taskid, result;

    while(is(taskid, result) != -1) {
      
      // Calculate the accuracy
      _evaluate(v, taskid, result);
    }
    
    // EOF
    if(s.evaluated == s.total) {

      assert(s.infers.size() == 0);

      if(_verbose) {
        cout(
          "-------------------- summary --------------------\n",
          "# tasks: ", s.total, "\n",
          "Query 1: ", s.query1 / s.total, "\n",
          "Query 2: ", s.query2 / s.total, "\n",
          "-------------------------------------------------\n"
        );
      }

      for(const auto& k : _out.keys()) {
        v.remove_ostream(k);
      }
      return Event::REMOVE;
    }

    return Event::DEFAULT;
  });
  
  return *this;
}

};  // end of namespace dtc::cell. ----------------------------------------------------------------


#endif







