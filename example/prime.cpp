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

int is_prime(int n){
  int square_root = sqrt(n);
  for(int i = 2; i<=square_root; ++i){
    if(n%i == 0){
      return 0;
    }
  }
  return 1;
}

struct Result {
  std::atomic<int> num_primes {0};
  std::atomic<int> count {0};
  Result() = default;
  Result(const Result& rhs) : num_primes {rhs.num_primes.load()}, count {rhs.count.load()} { }
};


int main(int argc, char* argv[]) {

  using namespace dtc::literals;

  dtc::Graph G;

  constexpr int num_slaves = 3;
  constexpr int range = 10000000;  // count primes between 1 and range

  std::vector<dtc::VertexBuilder> slaves;
  std::vector<dtc::StreamBuilder> m2s, s2m;
  auto master = G.vertex();

  for(auto i=0; i<num_slaves; ++i) {
    slaves.emplace_back(G.vertex());
    m2s.push_back(G.stream(master,slaves.back()));
    s2m.push_back(G.stream(slaves.back(),master));
  }

  // Master send the data to slaves.
  master.on(
    [&] (dtc::Vertex& v) {
      v.any = Result();
      int stride = (range + num_slaves - 1)/num_slaves; // get the ceiling
      for(const auto& s : m2s) {
        v.ostream(s)(stride);
      }
    }
  );


  // Stream: master to slave
  for(int i=0; i<num_slaves; ++i) {
    m2s[i].on(
      [other=s2m[i], range, start=i,num_slaves] (dtc::Vertex& slave, dtc::InputStream& is) {
        if(int stride; is(stride) != -1)  {
          int count = 0;
          for(int i=start;i<stride;i+=num_slaves){
            for(int j=0;j<num_slaves;++j){
              if(auto number=i+j*stride; number<=range && number>=2 && is_prime(number)){
                ++count;
              }
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
      [num_slaves] (dtc::Vertex& master, dtc::InputStream& is) {
        if(int num_primes = 0; is(num_primes) != -1) {
          auto& result = std::any_cast<Result&>(master.any);
          result.num_primes += num_primes;
          if(++result.count == num_slaves) {
            std::cout << "# primes up to " << range << ": " << result.num_primes << '\n';
          }
          return dtc::Stream::CLOSE;
        }
        return dtc::Stream::DEFAULT;
      }
    );
  }


  G.container().add(master).num_cpus(1).memory_limit_in_bytes(1_MB);
  G.container().add(slaves[0]).num_cpus(1).memory_limit_in_bytes(1_MB);
  G.container().add(slaves[1]).num_cpus(1).memory_limit_in_bytes(1_MB);
  G.container().add(slaves[2]).num_cpus(1).memory_limit_in_bytes(1_MB);

  dtc::Executor(G).run(); 

  return 0;
};


