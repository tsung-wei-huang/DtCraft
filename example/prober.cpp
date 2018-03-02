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

// Program: prober
//
// This program demonstrates how to create a prober vertex to simulate the streaming
// environment where data keeps coming at a given duration of time.
//

#include <dtc/dtc.hpp>

int main() {

  using namespace dtc::literals;
  using namespace std::literals;

  dtc::Graph G;

  auto A = G.vertex();
  auto B = G.vertex();
  auto S = G.stream(A, B);

  // Build up the receiver.
  S.on([](dtc::Vertex& B, dtc::InputStream& is) {
    if(std::string msg; is(msg) != -1) {
      if(msg == "remove") {
        return dtc::Event::REMOVE;
      }
      std::cout << "B received message: " << msg << std::endl;
    }
    return dtc::Event::DEFAULT;
  });

  // Build up the prober on A
  A.on([](dtc::Vertex& v) {
    v.any = std::vector<std::string> {"hello", "from", "the", "server", "remove"};
  });

  // Service.
  G.prober(A)
   .on([i=0u, S] (dtc::Vertex& v) mutable {
     if(auto& data = std::any_cast<std::vector<std::string>&>(v.any); i < data.size()) {
       (*v.ostream(S))(data[i++]);
       return dtc::Event::DEFAULT;
     }
     else {
       return dtc::Event::REMOVE;
     }
   })
   .duration(1s);
  
  // Partition the graph
  G.container().add(A).cpu(1).memory(1_GB);
  G.container().add(B).cpu(1).memory(1_GB);
  
  // Dispatch the graph.
  dtc::Executor(G).run(); 

  return 0;
}












