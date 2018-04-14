// Program: feeder
// Creator: Tsung-Wei Huang
// Date   : 2018/02/09

#include <dtc/dtc.hpp>

// Procedure: text_stream_feeder
// Create a feeder that generates a discrete text stream line by line from a given file
// and create an operator that parses each line to a set of words.
void text_stream_feeder() {

  using namespace std::literals;

  dtc::Graph G;
  
  // Create a text stream feeder
  auto tsf = G.insert<dtc::cell::TextStreamFeeder>(
    DTC_HOME "/Makefile.am",
    [] (std::string& line) -> std::optional<std::string> { 
      if(line.empty()) return {};
      else return std::move(line); 
    }
  );
  
  tsf.duration(10ms);

  // Create a word spliter.
  auto spliter = G.insert<dtc::cell::Operator1x1>(
    [l=0] (std::string& line) mutable {
      const static std::regex we_re("\\w+");
      auto ptr = std::sregex_token_iterator(line.begin(), line.end(), we_re);
      std::cout << "Words at line " << l++ << ": ";
      while(ptr != std::sregex_token_iterator()) {
        std::cout << ptr->str() << " ";
        ptr++;
      }
      std::cout << std::endl;
    }
  );
  
  // Connect the stream feeder to the feeder.
  spliter.in(tsf.out()); 
  
  // Partition the feeder and spliter.
  G.container().add(tsf);
  G.container().add(spliter);   

  // Dispatch the graph.
  dtc::Executor(G).run(); 
}

// ------------------------------------------------------------------------------------------------

// Procedure: csv_stream_feeder
// Create a feeder that generates a discrete csv stream from a given file. The CSV data
// is represented by Eigen matrix.
void csv_stream_feeder() {

  /*using namespace std::literals;

  dtc::Graph G;
  
  // Create a text stream feeder
  auto csf = G.insert<dtc::cell::CsvStreamFeeder>(
    DTC_HOME "/benchmark/debs/debs_2018.csv",
    [] (Eigen::MatrixXf& rows) { 
      std::cout << "num data points = " << rows.rows() << std::endl;
      std::cout << rows << std::endl;
    }
  );
  
  csf.duration(500ms).frequency(2);

  // Partition the feeder and spliter.
  G.container().add(csf);

  // Dispatch the graph.
  dtc::Executor(G).run(); */
}

// ------------------------------------------------------------------------------------------------

// Function: main
int main(int argc, char* argv[]) {

  //text_stream_feeder();
  csv_stream_feeder();
  
  return 0;
}







