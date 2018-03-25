// Program: hello_world
// Creator: Tsung-Wei Huang
// Date   : 2017/12/01
// 
// This program demonstrates how to create a loop where each end sends 
// a message to the other end.

#include <dtc/dtc.hpp>

int main(int argc, char* argv[]) {

  using namespace dtc::literals;
  using namespace std::literals;

  dtc::Graph G;

  auto A = G.vertex();
  auto B = G.vertex();

  G.container().add(A).cpu(1).memory(1_GB);

  auto AB = G.stream(A, B).on(
    [] (dtc::Vertex& B, dtc::InputStream& is) {
      if(std::string b; is(b) != -1) {
        dtc::cout("Received: ", b, '\n');
        return dtc::Event::REMOVE;
      }
      return dtc::Event::DEFAULT;
    }
  );

  auto BA = G.stream(B, A);
  
  A.on(
    [&AB] (dtc::Vertex& v) {
      (*v.ostream(AB))("hello world from A"s);
      dtc::cout("Sent 'hello world from A' to stream ", AB, "\n");
    }
  );
  
  G.container().add(B).cpu(1).memory(1_GB);
 
  B.on(
    [&BA] (dtc::Vertex& v) {
      (*v.ostream(BA))("hello world from B"s);
      dtc::cout("Sent 'hello world from B' to stream ", BA, "\n");
    }
  );
  
  BA.on(
    [] (dtc::Vertex& A, dtc::InputStream& is) {
      if(std::string a; is(a) != -1) {
        dtc::cout("Received: ", a, "\n");
        return dtc::Event::REMOVE;
      }
      return dtc::Event::DEFAULT;
    }
  );

  dtc::Executor(G).run(); 

  return 0;
};










