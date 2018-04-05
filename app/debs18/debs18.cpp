// Program: debs18

#include <app/debs18/debs18.hpp>

namespace debs18 {

// Class: DataFrame
class DataFrame : public dtc::CsvFrame {

  public:

    DataFrame(const std::filesystem::path&);

    const Eigen::MatrixXf& data() const;
    
    Eigen::MatrixXf get(const std::vector<int>&);

    Eigen::VectorXf id() const;
    Eigen::VectorXf type() const;
    Eigen::VectorXf speed() const;
    Eigen::VectorXf longitude() const;
    Eigen::VectorXf latitude() const;
    Eigen::VectorXf course() const;
    Eigen::VectorXf heading() const;
    Eigen::VectorXf timestamp(bool=false) const;
    Eigen::VectorXf departure_port() const;
    Eigen::VectorXf draught() const;
    Eigen::VectorXf arrival_time(bool=false) const;
    Eigen::VectorXf arrival_port() const;
    Eigen::VectorXf distance_from_departure() const;
    Eigen::VectorXf time_to_arrive() const;
    Eigen::VectorXf arrival_port_longitude() const;
    Eigen::VectorXf arrival_port_latitude() const;

    float scale_timestamp(float) const;

  private:

    Eigen::MatrixXf _data;
};

// Constructor
DataFrame::DataFrame(const std::filesystem::path& path) : dtc::CsvFrame(path) {

  if(num_cols() != NUM_SHIP_ATTRIBUTES) {
    throw std::invalid_argument("# ship attributes doesn't match");
  }
  
  _data.resize(num_rows(), num_cols());
  
  // Process the ship id
  auto ship_ids = col(SHIP_ID);
  std::unordered_map<std::string, int> ship_id_map;
  int id {0};
  for(size_t i=0; i<num_rows(); ++i) {
    if(ship_id_map.find(ship_ids[i]) == ship_id_map.end()) {
      ship_id_map[ship_ids[i]] = id++;
    }
    _data(i, SHIP_ID) = ship_id_map[ship_ids[i]];
  }

  for(size_t i=0; i<num_cols(); ++i) {
    if(i == SHIP_ID) continue;
    _data.col(i) = debs18::make_ship_data(*this, i, false);
  }
}

// Function: scale_timestamp
float DataFrame::scale_timestamp(float v) const {
  static const auto min_timepoint = timestamp_to_timepoint(min_timestamp);
  static const auto max_timepoint = timestamp_to_timepoint(max_timestamp);
  static const auto dif_timepoint = max_timepoint - min_timepoint;
  auto de = std::chrono::duration_cast<std::chrono::minutes>(dif_timepoint).count();
  return v / de;
}

// Function: get
Eigen::MatrixXf DataFrame::get(const std::vector<int>& ids) {
  Eigen::MatrixXf data(num_rows(), ids.size());
  int c = 0;
  for(const auto& id : ids) {
    data.col(c++) = _data.col(id);
  }
  return data;
}

Eigen::VectorXf DataFrame::id() const {
  return _data.col(SHIP_ID);
}

Eigen::VectorXf DataFrame::type() const {
  return _data.col(SHIP_TYPE);
}

Eigen::VectorXf DataFrame::speed() const {
  return _data.col(SHIP_SPEED);
}

Eigen::VectorXf DataFrame::longitude() const {
  return _data.col(SHIP_LONGITUDE);
}

Eigen::VectorXf DataFrame::latitude() const {
  return _data.col(SHIP_LATITUDE);
}

Eigen::VectorXf DataFrame::course() const {
  return _data.col(SHIP_COURSE);
}

Eigen::VectorXf DataFrame::heading() const {
  return _data.col(SHIP_HEADING);
}

Eigen::VectorXf DataFrame::timestamp(bool scale) const {
  Eigen::VectorXf t(_data.rows());
  for(int i=0; i<t.rows(); ++i) {
    if(scale) {
      t(i) = scale_timestamp(_data(i, SHIP_TIMESTAMP));
    }
    else {
      t(i) = _data(i, SHIP_TIMESTAMP);
    }
  }
  return t;
}

Eigen::VectorXf DataFrame::departure_port() const {
  return _data.col(SHIP_DEPARTURE_PORT);
}

Eigen::VectorXf DataFrame::draught() const {
  return _data.col(SHIP_DRAUGHT);
}

Eigen::VectorXf DataFrame::arrival_time(bool scale) const {
  Eigen::VectorXf t(_data.rows());
  for(int i=0; i<t.rows(); ++i) {
    if(scale) {
      t(i) = scale_timestamp(_data(i, SHIP_ARRIVAL_TIME));
    }
    else {
      t(i) = _data(i, SHIP_ARRIVAL_TIME);
    }
  }
  return t;
}

Eigen::VectorXf DataFrame::arrival_port() const {
  return _data.col(SHIP_ARRIVAL_PORT);
}

Eigen::VectorXf DataFrame::distance_from_departure() const {

  const auto ports = col_view(SHIP_DEPARTURE_PORT);

  Eigen::VectorXf dis(ports.size());

  for(size_t i=0; i<ports.size(); ++i) {
    float slon = _data(i, SHIP_LONGITUDE);
    float slat = _data(i, SHIP_LATITUDE);
    float plon = port_longitude(ports[i]);
    float plat = port_latitude(ports[i]);
    dis(i) = distance_on_earth(slat, slon, plat, plon);
  }
  
  return dis;
}

Eigen::VectorXf DataFrame::arrival_port_longitude() const {
  const auto ports = col_view(SHIP_ARRIVAL_PORT);
  Eigen::VectorXf lon(ports.size());
  for(size_t i=0; i<ports.size(); ++i) {
    lon(i) = port_longitude(ports[i]);     
  }
  return lon;
}

Eigen::VectorXf DataFrame::arrival_port_latitude() const {
  const auto ports = col_view(SHIP_ARRIVAL_PORT);
  Eigen::VectorXf lat(ports.size());
  for(size_t i=0; i<ports.size(); ++i) {
    lat(i) = port_latitude(ports[i]);     
  }
  return lat;
}

// function: time_to_arrive
Eigen::VectorXf DataFrame::time_to_arrive() const {
  return _data.col(SHIP_ARRIVAL_TIME) - _data.col(SHIP_TIMESTAMP);
}

// Function: data
const Eigen::MatrixXf& DataFrame::data() const {
  return _data;
}


};  // end of namespace debs18. -------------------------------------------------------------------


int main(int argc, char* argv[]) {
  
  // Usage.
  if(argc < 2) {
    std::cout << "Usage: ./debs18 <filename>.csv\n";
    std::exit(EXIT_FAILURE);
  }

  // Create a dataframe for debs18
  debs18::DataFrame df(argv[1]);
  
  dtc::cout(
    "DEBS18 data: ", argv[1], '\n',
    "# rows     : ", df.num_rows(), '\n',
    "# cols     : ", df.num_cols(), '\n'
  );

  // Extract the data for arrival time training
  dtc::LOGI("Preparing training data ...");

  Eigen::VectorXf type                    = df.type();
  Eigen::VectorXf speed                   = df.speed();
  Eigen::VectorXf longitude               = df.longitude();
  Eigen::VectorXf latitude                = df.latitude();
  Eigen::VectorXf course                  = df.course();
  Eigen::VectorXf heading                 = df.heading();
  Eigen::VectorXf timestamp               = df.timestamp();
  Eigen::VectorXf departure_port          = df.departure_port();
  Eigen::VectorXf draught                 = df.draught();
  Eigen::VectorXf arrival_time            = df.arrival_time();
  Eigen::VectorXf distance_from_departure = df.distance_from_departure();
  Eigen::VectorXf time_to_arrive          = df.time_to_arrive();

  Eigen::MatrixXf raw(df.num_rows(), 9);
  raw << type, 
         speed, 
         longitude, 
         latitude, 
         course, 
         //heading,
         timestamp, 
         departure_port, 
         distance_from_departure, 
         arrival_time;
         //time_to_arrive;

  std::cout << "Raw data (top 10 raws):\n";

  if(raw.rows() >= 10) std::cout << raw.topRows(10) << " ... (more)\n";
  else std::cout << raw << std::endl;

  // Purifying the data
  dtc::LOGI("Purifying the data (remove NaN rows) ...");
  debs18::remove_NaN_rows(raw);
  
  // Extract the training data.
  std::cout << "Training Data (top 10 rows):\n";
  auto& ship = raw;
  if(ship.rows() >= 10) std::cout << ship.topRows(10) << " ... (more)\n";
  else std::cout << ship << std::endl;

  assert(ship.hasNaN() == false);

  const int N = ship.rows();
  const int num_infers = N/10;
  const int num_trains = N - num_infers;
    
  Eigen::MatrixXf Dtr = ship.leftCols(ship.cols()-1).middleRows(0, num_trains);
  Eigen::MatrixXf Dte = ship.leftCols(ship.cols()-1).middleRows(num_trains, num_infers);

  Eigen::VectorXf Ltr = ship.rightCols(1).middleRows(0, num_trains);
  Eigen::VectorXf Lte = ship.rightCols(1).middleRows(num_trains, num_infers);
  
  dtc::cout(
    "Dtr: ", Dtr.rows(), "x", Dtr.cols(), '\n',
    "Ltr: ", Ltr.rows(), "x", Ltr.cols(), '\n',
    "Dte: ", Dte.rows(), "x", Dte.cols(), '\n',
    "Lte: ", Lte.rows(), "x", Lte.cols(), '\n'
  );
  
  Eigen::MatrixXf comp(Dte.rows(), 3);    

  // Train
  dtc::ml::DnnRegressor dnng;
  dnng.fully_connected_layer(Dtr.cols(), 30, dtc::ml::Activation::RELU)
      .fully_connected_layer(30, 1, dtc::ml::Activation::NONE)
      .loss(dtc::ml::Loss::MEAN_ABSOLUTE_ERROR)
      .optimizer<dtc::ml::AdamOptimizer>();
  
  // Record the prediction before training.
  comp.col(0) = dnng.infer(Dte);
  
  // Perform training.
  dnng.train(Dtr, Ltr, 32, 256, 0.01f, [&, i=0] (dtc::ml::DnnRegressor& dnng) mutable {
         float Etr = (dnng.infer(Dtr) - Ltr).array().abs().sum() / (static_cast<float>(Dtr.rows()));
         float Ete = (dnng.infer(Dte) - Lte).array().abs().sum() / (static_cast<float>(Dte.rows()));
         printf("Epoch %d: Etr=%.4f, Ete=%.4f\n", ++i, Etr, Ete);
       });
  
  // Infer
  comp.col(1) = dnng.infer(Dte);
  comp.col(2) = Lte;
  
  std::cout << "Prediction [before | after | golden] (top 30 rows):\n";
  if(comp.rows() < 30) std::cout << comp << std::endl;
  else std::cout << comp.topRows(30) << " ... (more)\n";

  return 0;
}




