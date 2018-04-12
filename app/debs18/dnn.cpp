#include <app/debs18/debs18.hpp>

namespace debs18 {

// Procedure: regression_dnn
void regression_dnn(const std::filesystem::path& path, const std::filesystem::path& model) {

  DataFrame df(path);

  auto trips = df.trips({
    debs18::DataFrame::TIMESTAMP,
    debs18::DataFrame::TYPE,
    debs18::DataFrame::SPEED,
    debs18::DataFrame::LONGITUDE,
    debs18::DataFrame::LATITUDE,
    debs18::DataFrame::COURSE,
    debs18::DataFrame::DISTANCE_FROM_DEPARTURE,
    debs18::DataFrame::CUMULATIVE_DISTANCE,
    debs18::DataFrame::DEPARTURE_PORT,
    //debs18::DataFrame::HEADING,
    //debs18::DataFrame::DRAUGHT,
    debs18::DataFrame::ARRIVAL_TIME
  });

  dtc::cout(
    "Regression : DNN\n",
    "Data       : ", path, '\n',
    "# rows     : ", df.num_rows(), '\n',
    "# cols     : ", df.num_cols(), '\n',
    "# trips    : ", trips.size(), '\n'
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

  // Extract the training data.
  std::cout << "Ship data (top 30 rows):\n";
  if(ship.rows() >= 30) std::cout << ship.topRows(30) << " ... (more)\n";
  else std::cout << ship << std::endl;

  assert(ship.hasNaN() == false);

  const int N = ship.rows();
  const int num_infers = N/10;
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
  
  Eigen::MatrixXf comp(Dte.rows(), 3);    
  
  // Train
  dtc::ml::DnnRegressor dnn;
  dnn.fully_connected_layer(Dtr.cols(), 30, dtc::ml::Activation::RELU)
     .fully_connected_layer(30, 1, dtc::ml::Activation::NONE)
     .loss<dtc::ml::MeanAbsoluteError>();
  
  // Record the prediction before training.
  comp.col(0) = dnn.infer(Dte);
  
  // Perform training.
  dnn.train(Dtr, Ltr, 10, 128, 0.01f, [&, i=0] (dtc::ml::DnnRegressor& dnn) mutable {
         float Etr = (dnn.infer(Dtr) - Ltr).array().abs().sum() / (static_cast<float>(Dtr.rows()));
         float Ete = (dnn.infer(Dte) - Lte).array().abs().sum() / (static_cast<float>(Dte.rows()));
         printf("Epoch %d: Etr=%.4f, Ete=%.4f\n", ++i, Etr, Ete);
       });
  
  // Infer
  comp.col(1) = dnn.infer(Dte);
  comp.col(2) = Lte;
  
  std::cout << "Prediction [before | after | golden] (top 30 rows):\n";
  if(comp.rows() < 30) std::cout << comp << std::endl;
  else std::cout << comp.topRows(30) << " ... (more)\n";

  // Evaluate per trip accuracy
  dtc::LOGI("Evaluating per trip accuracy ...");
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
    std::cout << "trips[" << trips[tid].id << "] MSE=" << err << ", "
              << "LEN=" << trips[tid].rows() << std::endl;
  }

  // Save the model
  if(!model.empty()) {
    dtc::LOGI("Saving model to ", model, " ...");
    dnn.save(model);
  }
  
  // End regression
  dtc::LOGI("Successfully performed DNN regression");
}

  

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


};  // end of namespace debs18 --------------------------------------------------------------------
