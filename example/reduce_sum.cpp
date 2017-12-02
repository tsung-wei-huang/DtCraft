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

struct Result {
  std::atomic<int> value {0};
  std::atomic<int> count {0};
  Result() = default;
  Result(const Result& rhs) : value {rhs.value.load()}, count {rhs.count.load()} { }
};

int main(int argc, char* argv[]) {

  using namespace dtc::literals;

  constexpr int num_slaves = 3;

  dtc::Graph G;

  std::vector<dtc::VertexBuilder> slaves;
  std::vector<dtc::StreamBuilder> m2s, s2m;

  auto master = G.vertex();

  for(auto i=0; i<num_slaves; ++i) {
    auto v = G.vertex();
    auto a = G.stream(master, v);
    auto b = G.stream(v, master);
    slaves.push_back(v);
    m2s.push_back(a);
    s2m.push_back(b);
  }

  // Master send the data to slaves.
  master.on(
    [&] (dtc::Vertex& v) {
      v.any = Result();
      std::vector<int> send(1024, 1);
      for(const auto& s : m2s) {
        v.ostream(s)(send);
      }
    }
  );

  // Stream: master to slave
  for(int i=0; i<num_slaves; ++i) {
    m2s[i].on(
      [other=s2m[i]] (dtc::Vertex& slave, dtc::InputStream& is) {
        if(std::vector<int> recv; is(recv) != -1)  {
          slave.ostream(other)(std::accumulate(recv.begin(), recv.end(), 0)); 
          return dtc::Stream::CLOSE;
        }
        return dtc::Stream::DEFAULT;
      }
    );
  }

  // Stream: slave to master
  for(int i=0; i<num_slaves; ++i) {
    s2m[i].on(
      [] (dtc::Vertex& master, dtc::InputStream& is) {
        if(int value = 0; is(value) != -1) {
          auto& result = std::any_cast<Result&>(master.any);
          result.value += value;
          if(++result.count == num_slaves) {
            std::cout << "reduce sum: " << result.value << '\n';
          }
          return dtc::Stream::CLOSE;
        }
        return dtc::Stream::DEFAULT;
      }
    );
  }

  G.container().add(master).num_cpus(1);
  G.container().add(slaves[0]).num_cpus(1);
  G.container().add(slaves[1]).num_cpus(1);
  G.container().add(slaves[2]).num_cpus(1);

  dtc::Executor(G).run();

  return 0;
}
