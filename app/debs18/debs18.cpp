// Program: debs18

#include <dtc/contrib/debs18/debs18.hpp>

// Procedure: benchmark
void benchmark(const std::filesystem::path& file, const std::filesystem::path& model) {

  using namespace std::literals::string_literals;

  dtc::Graph G;

  auto feeder = G.insert<dtc::debs18::StreamFeeder>(file);

  //auto predictor = G.insert<dtc::cell::Visitor1x1>(
  //  [P=dtc::debs18::ArrivalTimePredictor{model}] (dtc::debs18::ArrivalTimePredictor& pred) {
  //    dnn.load(model);
  //  },
  //  [] (dtc::ml::DnnRegressor& dnn, std::tuple<std::string, std::string>& envelope) {
  //    const auto& [taskid, content] = envelope; 
  //    Eigen::MatrixXf infer = dtc::debs18::make_regression_features(content);
  //    //std::cout << infer.rows() << "x" << infer.cols() << std::endl;
  //    int pred_at = (dnn.infer(infer))(0, 0);
  //    return std::make_tuple(taskid, dtc::debs18::minutes_to_timestamp(pred_at) + ',');
  //  }
  //);

  dtc::debs18::ArrivalTimePredictor P{model};

  auto predictor = G.insert<dtc::cell::Operator1x1>([&] (std::tuple<std::string, std::string>& envelope) {
    const auto& [taskid, content] = envelope; 
    return std::make_tuple(taskid, P.infer(content));
  });
  
  feeder.in(predictor.out());
  predictor.in(feeder.out());

  dtc::Executor(G).run();

};

// ------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  
  // Usage.
  if(argc != 4) {
    std::cout << "Usage: ./debs18 <filename>.csv method modelfile\n";
    std::exit(EXIT_FAILURE);
  }
  
  if(auto method = std::string_view(argv[2]); method == "train") {
    dtc::debs18::regression_dnn(argv[1], argv[3]);
  }
  else if(method == "benchmark"){
    benchmark(argv[1], argv[3]);
  }
  else {
    std::cout << "invalid method\n";
    std::exit(EXIT_FAILURE);
  }

  //benchmark(argv[1], argv[3]);

  return 0;
}




