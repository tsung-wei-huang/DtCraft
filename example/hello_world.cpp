/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/dtc.hpp>


int main(int argc, char* argv[]) {

  using namespace dtc::literals;
  using namespace std::literals;

  dtc::Graph G;
	
  auto A = G.vertex();
  auto B = G.vertex();

  G.container().add(A).num_cpus(1).memory_limit_in_bytes(1_MB);
  G.container().add(B).num_cpus(1).memory_limit_in_bytes(1_MB);

  auto AB = G.stream(A, B).on(
    [] (dtc::Vertex& B, dtc::InputStream& is) {
      if(std::string b; is(b) != -1) {
        std::cout << "Received: " << b << '\n';
        return dtc::Stream::CLOSE;
      }
      return dtc::Stream::DEFAULT;
    }
  );

  auto BA = G.stream(B, A).on(
    [] (dtc::Vertex& A, dtc::InputStream& is) {
      if(std::string a; is(a) != -1) {
        std::cout << "Received: " << a << '\n';
        return dtc::Stream::CLOSE;
      }
      return dtc::Stream::DEFAULT;
    }
  );
  
  A.on(
    [&AB] (dtc::Vertex& v) {
      v.ostream(AB)("hello world from A"s);
      std::cout << "Sent 'hello world from A' to stream " << AB << '\n';
    }
  );
 
  B.on(
    [&BA] (dtc::Vertex& v) {
      v.ostream(BA)("hello world from B"s);
      std::cout << "Sent 'hello world from B' to stream " << BA << '\n';
    }
  );


  dtc::Executor(G).run(); 

  return 0;
};


