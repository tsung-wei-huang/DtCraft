// Program: operator
// Author: Tsung-Wei Huang
// 
// This program demonstrates how to use the operator interface.

#include <dtc/dtc.hpp>
#include <dtc/cell/operator.hpp>

int main(int argc, char* argv[]) {

  dtc::Graph G;

  constexpr int N = 10;

  auto v1 = G.vertex();
  auto v2 = G.vertex();

  auto addone = G.insert<dtc::cell::Operator1x1>([](int n) { return n+1; });

  addone.in(v1);

  v1.on([v12a=addone.in()] (dtc::Vertex& v1) {
    for(int i=0; i<N; ++i) {
      int val = dtc::random<int>(0, 10);
      std::cout << "v1 sent a random int (0~10): " << val << std::endl;
      v1.broadcast(val);
    }
    v1.remove_ostream(v12a);
  });

  G.stream(addone.out(), v2).on([n=0] (dtc::Vertex& v2, dtc::InputStream& is) mutable {
    int r;
    while(is(r) != -1) {
      std::cout << "v2 received " << r << " from addone" << std::endl;
      ++n;
    }
    return n == N ? dtc::Event::REMOVE : dtc::Event::DEFAULT;
  });

  G.container().add(v1);
  G.container().add(v2);
  
  dtc::Executor(G).run(); 

  return 0;
}







