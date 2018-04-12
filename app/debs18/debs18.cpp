// Program: debs18

#include <app/debs18/debs18.hpp>

int main(int argc, char* argv[]) {
  
  // Usage.
  if(argc < 3) {
    std::cout << "Usage: ./debs18 <filename>.csv [model_file]\n";
    std::exit(EXIT_FAILURE);
  }

  debs18::regression_dnn(argv[1], argv[2]);

  return 0;
}




