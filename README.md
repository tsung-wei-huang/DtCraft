<img src="https://github.com/twhuang-uiuc/DtCraft/blob/master/logo.jpg" width="50%">

## What's DtCraft?
DtCraft is a C++-based programming engine to streamline cluster computing. It offers a new powerful programming model called *stream graph* to describe a parallel and distributed workload in terms of streams. Once an application is cast into this framework, the kernel transparently performs the job execution, process communication, and concurrency controls for you. You don't have to worry about DevOps and can focus on high-level development.

<p align="center"><img src="http://dtcraft.web.engr.illinois.edu/images/stream_graph.jpg" width="50%"></p>

## Design Goal
The goal of DtCraft is to help you write simple, easy, and effective code at cluster scale. The example below demonstrates a simple application to run two programs on two machines sending each other a message.

```cpp
#include <dtc/dtc.hpp>
  
using namespace std::literals;  // for the use of string literal
using namespace dtc::literals;  // for the use of memory literal

int main(int argc, char* argv[]) {

  dtc::Graph G;

  auto A = G.vertex();
  auto B = G.vertex();

  auto lambda = [] (dtc::Vertex& v, dtc::InputStream& is) {
    if(std::string s; is(s) != -1) {
      std::cout << "Received: " << s << '\n';
      return dtc::Event::REMOVE;
    }
    return dtc::Event::DEFAULT;
  };

  auto AB = G.stream(A, B).on(lambda);
  auto BA = G.stream(B, A).on(lambda); 

  A.on([&AB] (dtc::Vertex& v) { (*v.ostream(AB))("hello world from A"s); });  
  B.on([&BA] (dtc::Vertex& v) { (*v.ostream(BA))("hello world from B"s); });
  
  G.container().add(A).cpu(1).memory(1_GB);
  G.container().add(B).cpu(1).memory(1_GB);

  dtc::Executor(G).run();
}
```

There are myriads of cluster computing frameworks such as Hadoop MapReduce, Apache Spark, Dryad, and Ray. We believe each has its reason to exist. DtCraft targets at:

- **Programming model**. DtCraft leverages modern C++17 to offer a new programming model for cluster computing. Our model is very general and can implement most distributed computing patterns.

- **Performance**. DtCraft is designed completely from the ground up using advanced software techniques in order to deliver the best performance. 

- **Productivity**. DtCraft allows you to easily bring up a parallel and distributed workload in only a few lines of code. It takes only a few steps to set up a cluster and get things up and running.  

## Learn More about DtCraft
Please visit the official <a href="http://dtcraft.web.engr.illinois.edu/">website</a> to learn more about DtCraft.
