#include <dtc/contrib/debs18/query2.hpp>

namespace dtc::debs18 {

struct HyperParameters {
  std::filesystem::path input;
  std::filesystem::path model;
  float lrate = 0.01f;
  size_t num_neurons = 10;
  size_t mini_batch = 64;
  size_t num_epochs = 10000;
  float MAE = std::numeric_limits<float>::max();
};

std::ostream& operator << (std::ostream& os, const HyperParameters& hp) {
  os << "input=" << hp.input << ", "
     << "model=" << hp.model << ", "
     << "[lrate|neurons|mini-batch|epochs] = [" 
     << hp.lrate       << "|" 
     << hp.num_neurons << "|" 
     << hp.mini_batch  << "|"
     << hp.num_epochs  << "], MAE=" << hp.MAE;
  return os;
}

// Total available ship types:
// 0, 20, 30, 32, 34, 36, 37, 51, 52, 53, 60, 66, 69, 70, 71, 72, 73, 74, 79,
// 80, 81, 82, 83, 84, 85, 89, 90, 99
std::tuple<Eigen::MatrixXf, Eigen::MatrixXf, Eigen::MatrixXf, Eigen::MatrixXf, Eigen::MatrixXf> 
make_data(const std::filesystem::path& path) {
  
  DataFrame df(path);

  auto trips = df.trips({
    debs18::DataFrame::TIMESTAMP,
    debs18::DataFrame::TYPE,
    debs18::DataFrame::SPEED,
    debs18::DataFrame::LONGITUDE,
    debs18::DataFrame::LATITUDE,
    debs18::DataFrame::COURSE,
    //debs18::DataFrame::DISTANCE_FROM_DEPARTURE,
    debs18::DataFrame::CUMULATIVE_DISTANCE,
    debs18::DataFrame::CUMULATIVE_TIME,
    //debs18::DataFrame::DEPARTURE_PORT,
    debs18::DataFrame::HEADING,
    //debs18::DataFrame::DRAUGHT,
    debs18::DataFrame::BEARING,
    //debs18::DataFrame::ARRIVAL_PORT,
    //debs18::DataFrame::DELTA_TIMESTAMP,
    //debs18::DataFrame::DELTA_LONGITUDE,
    //debs18::DataFrame::DELTA_LATITUDE,
    debs18::DataFrame::ARRIVAL_TIME
  });

  dtc::cout(
    "Regression : DNN\n",
    "Data       : ", path, '\n',
    "# rows     : ", df.num_rows(), '\n',
    "# cols     : ", df.num_cols(), '\n',
    "# trips    : ", trips.size(), '\n',
    "# types    : ", df.types().size(), '\n'
  );

  // Purify the data
  dtc::LOGI("Removing rows with NaN ...");
  for(size_t i=0; i<trips.size(); ++i) {
    remove_NaN_rows(trips[i].route);  // arrival time might be missing.
  }

  // Remove rows with invalid timestamp
  dtc::LOGI("Removing rows with redundant timestamp ...");
  for(size_t i=0; i<trips.size(); ++i) {
    remove_invalid_timestamp_rows(trips[i].route);
  }
  
  // Remove empty trip
  dtc::LOGI("Removing empty trips ...");
  trips.erase(std::remove_if(trips.begin(), trips.end(), [] (auto& t) {
    return t.rows() == 0;
  }), trips.end());

  // Stack all trips to a big Eigen Matrix
  dtc::LOGI("Stacking trips ...");
  Eigen::MatrixXf ship = stack(trips);

  assert(ship.rows() != 0);

  // Shuffling the ship
  dtc::LOGI("Shuffling ship ...");
  shuffle(ship);

  // Extract the training data.
  std::cout << "Ship data (top 30 rows):\n";
  if(ship.rows() >= 30) std::cout << ship.topRows(30) << " ... (more)\n";
  else std::cout << ship << std::endl;

  assert(ship.hasNaN() == false);

  const int N = ship.rows();
  const int num_infers = N*0.05f;
  const int num_trains = N - num_infers;
    
  Eigen::MatrixXf Dtr = ship.leftCols(ship.cols()-1).middleRows(0, num_trains);
  Eigen::MatrixXf Dte = ship.leftCols(ship.cols()-1).middleRows(num_trains, num_infers);
  Eigen::MatrixXf Ltr = ship.rightCols(1).middleRows(0, num_trains);
  Eigen::MatrixXf Lte = ship.rightCols(1).middleRows(num_trains, num_infers);
  
  dtc::cout(
    "Dtr: ", Dtr.rows(), "x", Dtr.cols(), ", ",
    "Ltr: ", Ltr.rows(), "x", Ltr.cols(), '\n',
    "Dte: ", Dte.rows(), "x", Dte.cols(), ", ",
    "Lte: ", Lte.rows(), "x", Lte.cols(), '\n'
  );

  return {ship, Dtr, Dte, Ltr, Lte};
}

// Procedure: train
void try_train(HyperParameters& hp) {


  
  auto [ship, Dtr, Dte, Ltr, Lte] = make_data(hp.input);

  std::cout << "HP=" << hp << std::endl;
  
  ml::DnnRegressor dnn;

  dnn.layer<dtc::ml::FullyConnectedLayer>(Dtr.cols(), hp.num_neurons, dtc::ml::Activation::RELU);
  dnn.layer<dtc::ml::FullyConnectedLayer>(hp.num_neurons, 1);
  dnn.loss<dtc::ml::MeanAbsoluteError>();

  // Perform training.
  dnn.train(Dtr, Ltr, hp.num_epochs, hp.mini_batch, hp.lrate, [&, i=0] (dtc::ml::DnnRegressor& dnn) mutable {
    if(++i % 100 == 0) {
      float Etr = (dnn.infer(Dtr) - Ltr).array().abs().sum() / (static_cast<float>(Dtr.rows()));
      float Ete = (dnn.infer(Dte) - Lte).array().abs().sum() / (static_cast<float>(Dte.rows()));
      printf("Epoch %d: Etr=%.4f, Ete=%.4f\n", i, Etr, Ete);
    }
  });
  
  // Infer
  Eigen::MatrixXf comp(Dte.rows(), 2);    
  comp.col(0) = dnn.infer(Dte);
  comp.col(1) = Lte;
  
  std::cout << "Prediction [predict <-> golden] (top 30 rows):\n";
  if(comp.rows() < 30) std::cout << comp << std::endl;
  else std::cout << comp.topRows(30) << " ... (more)\n";

  auto total_mae = (dnn.infer(ship.leftCols(ship.cols()-1)) - ship.rightCols(1)).array().abs().sum() / ship.rows();
  
  std::cout << "Total MAE=" << total_mae << std::endl;

  hp.MAE = total_mae;
}

// Procedure: regression_dnn
void regression_dnn(const std::filesystem::path& path, const std::filesystem::path& model) {

  HyperParameters hp_curr {path, model};
  HyperParameters hp_best {path, model};

  for(size_t N=8; N<=32; N+=2) {
    hp_curr.num_neurons = N;
    for(size_t B=16; B<=128; B=B<<1) {
      hp_curr.mini_batch = B;
      for(size_t E=2000; E<=10000; E+=1000) {
        hp_curr.num_epochs = E;
        hp_curr.MAE = std::numeric_limits<float>::max();
        try_train(hp_curr);
        if(hp_curr.MAE < hp_best.MAE) {
          hp_best = hp_curr;
          std::ofstream ofs(model);
          ofs << hp_best << '\n';
        }
      }
    }
  }

  // End regression
  dtc::LOGI("Successfully performed DNN regression");
  std::cout << hp_best << std::endl;
}
  
  // Evaluate per trip accuracy
  /*dtc::LOGI("Evaluating per trip accuracy ...");
  std::vector<std::tuple<size_t, float>> tripinfo;
  for(size_t i=0; i<trips.size(); ++i) {
    Eigen::MatrixXf D = trips[i].route.leftCols(trips[i].route.cols()-1);
    Eigen::MatrixXf L = trips[i].route.rightCols(1);
    auto err = (dnn.infer(D) - L).array().abs().sum() / (static_cast<float>(D.rows()));
    tripinfo.push_back({i, err});
  }

  std::sort(tripinfo.begin(), tripinfo.end(), [] (const auto& a, const auto& b) {
    return std::get<1>(a) > std::get<1>(b);
  });

  for(const auto [tid, err] : tripinfo) {
    std::cout << "trips[" << trips[tid].id << "] MAE=" << err << ", "
              << "LEN=" << trips[tid].rows() << ", "
              << "TYPE=" << trips[tid].route(0, 1) << " vs " << trips[tid].type << std::endl;
  } */

// ------------------------------------------------------------------------------------------------

/*// Procedure: debs18_dnn
void debs18_dnn(const std::string& file) {

  // TODO (Guaannan)
  // raining here
  dtc::Graph G;

  
  auto debs18 = G.insert<dtc::cell::Debs18StreamFeeder>(
    file,
    [] (size_t& tid, std::string& record) -> std::tuple<size_t, std::string> {
      // Preprocessing here.
      return std::forward_as_tuple(tid, record);
    }
  );
  
  auto learner = G.insert<dtc::cell::Operator1x1>(
    [] (std::tuple<size_t, std::string>& task) mutable -> std::optional<std::tuple<size_t, std::string>> {
      auto& [taskid, record] = task;
      dtc::LOGD("received task[", taskid, "]: ", record);
      // Inferring data (10 features).
      if(auto num_commas = std::count(record.begin(), record.end(), ','); num_commas == 9) {

        // TODO (Guannan)
        // Infer the predicted arrival time and port.
        return std::make_tuple(taskid, std::string{"14-03-15 15:19"} + ',' + "GIBRALTAR");
      }
      else {
        dtc::cout("Invalid task_", taskid, ": ", record, '\n');
        assert(false);
      }
    }
  );

  learner.in(debs18.out());
  debs18.in(learner.out());

  dtc::Executor(G).run();
}*/



};  // end of namespace dtc::debs18. --------------------------------------------------------------
