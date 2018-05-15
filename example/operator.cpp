// Program: operator
// Author: Tsung-Wei Huang
// 
// This program demonstrates how to use the operator interface.

#include <dtc/dtc.hpp>
#include <dtc/cell/operator.hpp>

int main(int argc, char* argv[]) {

  using namespace std::literals;

  dtc::Graph G;

  constexpr int N = 10;

  auto v1 = G.vertex();
  auto v2 = G.vertex();

  dtc::cell::Operator1x1 addone(G,
    [] (int n) -> std::variant<int, dtc::Event::Signal> {
      printf("op received %d\n", n);
      if(n == N) {
        return dtc::Event::Signal::REMOVE;
      }
      else {
        return n+1;
      }
    }
  );

  addone.in(v1);

  v1.on([] (dtc::Vertex& v1) {
    for(int i=1; i<=N; ++i) {
      printf("v1 sent %d\n", i);
      v1.broadcast(i);
    }
  });

  G.stream(addone.out(), v2).on([] (dtc::Vertex& v2, dtc::InputStream& is) {
    int i;
    while(is(i) != -1) {
      printf("v2 received %d\n", i);
    }
    return dtc::Event::DEFAULT;
  });

  G.container().add(v1);
  G.container().add(v2);
  
  dtc::Executor(G).run(); 

  return 0;
}







