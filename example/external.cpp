// Program: external
// Creator: Tsung-Wei Huang
// Date   : 2018/03/02
// 
// This program demonstrates how to create a vertex that links to an external program.

#include <dtc/dtc.hpp>

int main(int argc, char* argv[]) {

  using namespace dtc::literals;
  using namespace std::literals;

  dtc::Graph G;

  auto A = G.vertex().program("/usr/bin/printenv");
  auto B = G.vertex().program("/usr/bin/gcc -v");

  G.stream(A, B).tag("AB");

  G.container().add(A);
  G.container().add(B);

  dtc::Executor(G).run();

  return 0;

};










