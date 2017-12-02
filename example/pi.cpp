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
#include <random>

struct Result{
  std::atomic<int> value {0};
  std::atomic<int> count {0};
  Result() = default;
  Result(const Result& r) : value {r.value.load()}, count {r.count.load()} {
  }
};

int main(int argc, char* argv[]) {

  using namespace dtc::literals;

  constexpr int num_slaves = 3;
  constexpr int num_samples = 90000000;

  static_assert(num_samples%num_slaves==0);

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
      for(size_t i = 0; i < m2s.size(); ++i){
        v.ostream(m2s[i])(num_samples/num_slaves);
      }
    }
  );

  // Stream: master to slave
  for(int i=0; i<num_slaves; ++i) {
    m2s[i].on(
      [other=s2m[i]] (dtc::Vertex& slave, dtc::InputStream& is) {
        if(int num_samples=0, count=0; is(num_samples) != -1) {
          for(int i = 0; i < num_samples; ++i){
            auto x = dtc::random<double>(-1.0, 1.0);
            auto y = dtc::random<double>(-1.0, 1.0);
            if( x*x + y*y <= 1.0 ){
              ++count;
            }
          }
          slave.ostream(other)(count);         
          return dtc::Stream::CLOSE;         
        }
        return dtc::Stream::DEFAULT;         
      }
    );
  }

  // Stream: slave to master
  for(int i=0; i<num_slaves; ++i) {
    s2m[i].on(
      [&m2s, num_samples = num_samples] (dtc::Vertex& master, dtc::InputStream& is) {
        if(int value = 0; is(value) != -1) {
          auto& result = std::any_cast<Result&>(master.any);
          result.value += value;
          if(++result.count == num_slaves) {
            std::cout << "pi estimation: " << 4.0 * result.value/num_samples << '\n';
          } 
          return dtc::Stream::CLOSE;
        }
        return dtc::Stream::DEFAULT;
      }
    );
  }

  G.container().add(master);
  G.container().add(slaves[0]);
  G.container().add(slaves[1]);
  G.container().add(slaves[2]);

  dtc::Executor(G).run();

  return 0;
}


