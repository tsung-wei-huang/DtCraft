// Program: debs18

#include <app/debs18/debs18.hpp>

int main(int argc, char* argv[]) {

  if(argc != 2) {
    std::cout << "Usage: ./debs18 <filename>.csv\n";
    std::exit(EXIT_FAILURE);
  }
  
  // Here we use the simplest mapping method for example.
  debs18_dnn(argv[1]);

  return 0;
}




