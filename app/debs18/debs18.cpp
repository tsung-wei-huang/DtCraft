// Program: debs18

#include <app/debs18/debs18.hpp>

namespace debs18 {

// Class: DataFrame
class DataFrame : public dtc::CsvFrame {

  public:

    enum Index {
      ID = 0,
      TYPE,
      SPEED,
      LONGITUDE,
      LATITUDE,
      COURSE,
      HEADING,
      TIMESTAMP,
      DEPARTURE_PORT,
      DRAUGHT,
      ARRIVAL_TIME,
      ARRIVAL_PORT,
      DISTANCE_FROM_DEPARTURE,
      TIME_TO_ARRIVE,
      DEPARTURE_PORT_LONGITUDE,
      DEPARTURE_PORT_LATITUDE,
      ARRIVAL_PORT_LONGITUDE,
      ARRIVAL_PORT_LATITUDE
    };

    DataFrame(const std::filesystem::path&);

    const Eigen::MatrixXf& data() const;
    
    Eigen::MatrixXf get(const std::vector<Index>&);
    Eigen::MatrixXf sort_on_trip() const;

    Eigen::VectorXf get(Index) const;
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
    Eigen::VectorXf departure_port_longitude() const;
    Eigen::VectorXf departure_port_latitude() const;
    Eigen::VectorXf arrival_port_longitude() const;
    Eigen::VectorXf arrival_port_latitude() const;
    
    float get(size_t, Index) const;
    float id(size_t) const;
    float type(size_t) const;
    float speed(size_t) const;
    float longitude(size_t) const;
    float latitude(size_t) const;
    float course(size_t) const;
    float heading(size_t) const;
    float timestamp(size_t) const;
    float departure_port(size_t) const;
    float draught(size_t) const;
    float arrival_time(size_t) const;
    float arrival_port(size_t) const;
    float distance_from_departure(size_t) const;
    float time_to_arrive(size_t) const;
    float departure_port_longitude(size_t) const;
    float departure_port_latitude(size_t) const;
    float arrival_port_longitude(size_t) const;
    float arrival_port_latitude(size_t) const;

    std::vector<Eigen::MatrixXf> trips(const std::vector<Index>&) const;

  private:

    Eigen::MatrixXf _data;
    
    float _scale_timestamp(float) const;
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

// Proceedure: sort_on_trip
Eigen::MatrixXf DataFrame::sort_on_trip() const {

  std::vector<int> indices(num_rows());
  for(int i=0; i<(int)indices.size(); ++i) {
    indices[i] = i;
  }

  std::sort(indices.begin(), indices.end(), [&] (int a, int b) {
    if(_data(a, SHIP_ID) == _data(b, SHIP_ID)) {
      return _data(a, SHIP_TIMESTAMP) < _data(b, SHIP_TIMESTAMP);
    }
    return _data(a, SHIP_ID) < _data(b, SHIP_ID);
  });
  
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(indices.size());

  for(int i=0; i<(int)indices.size(); ++i) {
    p.indices()[indices[i]] = i;
  }

  return p * _data;
}

// Function: _scale_timestamp
float DataFrame::_scale_timestamp(float v) const {
  static const auto min_timepoint = timestamp_to_timepoint(min_timestamp);
  static const auto max_timepoint = timestamp_to_timepoint(max_timestamp);
  static const auto dif_timepoint = max_timepoint - min_timepoint;
  auto de = std::chrono::duration_cast<std::chrono::minutes>(dif_timepoint).count();
  return v / de;
}

// Function: get
Eigen::VectorXf DataFrame::get(Index i) const {
  
  switch(i) {

    case ID:
      return id();
    break;

    case TYPE:
      return type();
    break;

    case SPEED:
      return speed();
    break;
    
    case LONGITUDE:
      return longitude();
    break;

    case LATITUDE:
      return latitude();
    break;

    case COURSE:
      return course();
    break;
    
    case HEADING:
      return heading();
    break;

    case TIMESTAMP:
      return timestamp();
    break;
    
    case DEPARTURE_PORT:
      return departure_port();
    break;

    case DRAUGHT:
      return draught();
    break;

    case ARRIVAL_TIME:
      return arrival_time();
    break;

    case ARRIVAL_PORT:
      return arrival_port();
    break;

    case DISTANCE_FROM_DEPARTURE:
      return distance_from_departure();
    break;

    case TIME_TO_ARRIVE:
      return time_to_arrive();
    break;

    case DEPARTURE_PORT_LONGITUDE:
      return departure_port_longitude();
    break;

    case DEPARTURE_PORT_LATITUDE:
      return departure_port_latitude();
    break;

    case ARRIVAL_PORT_LONGITUDE:
      return arrival_port_longitude();
    break;

    case ARRIVAL_PORT_LATITUDE:
      return arrival_port_latitude();
    break;

    default:
      assert(false);
    break;
  };

}

// Function: get
Eigen::MatrixXf DataFrame::get(const std::vector<Index>& ids) {
  Eigen::MatrixXf data(num_rows(), ids.size());
  for(size_t i=0; i<ids.size(); ++i) {
    data.col(i) = get(ids[i]);
  }
  return data;
}

Eigen::VectorXf DataFrame::id() const {
  return _data.col(SHIP_ID);
}

float DataFrame::id(size_t r) const {
  return _data(r, SHIP_ID);
}

Eigen::VectorXf DataFrame::type() const {
  return _data.col(SHIP_TYPE);
}

float DataFrame::type(size_t r) const {
  return _data(r, SHIP_TYPE);
}

Eigen::VectorXf DataFrame::speed() const {
  return _data.col(SHIP_SPEED);
}

float DataFrame::speed(size_t r) const {
  return _data(r, SHIP_SPEED);
}

Eigen::VectorXf DataFrame::longitude() const {
  return _data.col(SHIP_LONGITUDE);
}

float DataFrame::longitude(size_t r) const {
  return _data(r, SHIP_LONGITUDE);
}

Eigen::VectorXf DataFrame::latitude() const {
  return _data.col(SHIP_LATITUDE);
}

float DataFrame::latitude(size_t r) const {
  return _data(r, SHIP_LATITUDE);
}

Eigen::VectorXf DataFrame::course() const {
  return _data.col(SHIP_COURSE);
}

float DataFrame::course(size_t r) const {
  return _data(r, SHIP_COURSE);
}

Eigen::VectorXf DataFrame::heading() const {
  return _data.col(SHIP_HEADING);
}

float DataFrame::heading(size_t r) const {
  return _data(r, SHIP_HEADING);
}

Eigen::VectorXf DataFrame::timestamp(bool scale) const {
  Eigen::VectorXf t(_data.rows());
  for(int i=0; i<t.rows(); ++i) {
    if(scale) {
      t(i) = _scale_timestamp(_data(i, SHIP_TIMESTAMP));
    }
    else {
      t(i) = _data(i, SHIP_TIMESTAMP);
    }
  }
  return t;
}

float DataFrame::timestamp(size_t r) const {
  return _data(r, SHIP_TIMESTAMP);
}

Eigen::VectorXf DataFrame::departure_port() const {
  return _data.col(SHIP_DEPARTURE_PORT);
}

float DataFrame::departure_port(size_t r) const {
  return _data(r, SHIP_DEPARTURE_PORT);
}

Eigen::VectorXf DataFrame::draught() const {
  return _data.col(SHIP_DRAUGHT);
}

float DataFrame::draught(size_t r) const {
  return _data(r, SHIP_DRAUGHT);
}

Eigen::VectorXf DataFrame::arrival_time(bool scale) const {
  Eigen::VectorXf t(_data.rows());
  for(int i=0; i<t.rows(); ++i) {
    if(scale) {
      t(i) = _scale_timestamp(_data(i, SHIP_ARRIVAL_TIME));
    }
    else {
      t(i) = _data(i, SHIP_ARRIVAL_TIME);
    }
  }
  return t;
}

float DataFrame::arrival_time(size_t r) const {
  return _data(r, SHIP_ARRIVAL_TIME);
}

Eigen::VectorXf DataFrame::arrival_port() const {
  return _data.col(SHIP_ARRIVAL_PORT);
}

float DataFrame::arrival_port(size_t r) const {
  return _data(r, SHIP_ARRIVAL_PORT);
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

float DataFrame::distance_from_departure(size_t r) const {
  float slon = _data(r, SHIP_LONGITUDE);
  float slat = _data(r, SHIP_LATITUDE);
  float plon = port_longitude((*this)(r, SHIP_DEPARTURE_PORT));
  float plat = port_latitude((*this)(r, SHIP_DEPARTURE_PORT));
  return distance_on_earth(slat, slon, plat, plon);
}

Eigen::VectorXf DataFrame::departure_port_longitude() const {
  const auto ports = col_view(SHIP_DEPARTURE_PORT);
  Eigen::VectorXf lon(ports.size());
  for(size_t i=0; i<ports.size(); ++i) {
    lon(i) = port_longitude(ports[i]);     
  }
  return lon;
}

float DataFrame::departure_port_longitude(size_t r) const {
  return port_longitude((*this)(r, SHIP_DEPARTURE_PORT));
}

Eigen::VectorXf DataFrame::departure_port_latitude() const {
  const auto ports = col_view(SHIP_DEPARTURE_PORT);
  Eigen::VectorXf lat(ports.size());
  for(size_t i=0; i<ports.size(); ++i) {
    lat(i) = port_latitude(ports[i]);     
  }
  return lat;
}

float DataFrame::departure_port_latitude(size_t r) const {
  return port_latitude((*this)(r, SHIP_DEPARTURE_PORT));
}

Eigen::VectorXf DataFrame::arrival_port_longitude() const {
  const auto ports = col_view(SHIP_ARRIVAL_PORT);
  Eigen::VectorXf lon(ports.size());
  for(size_t i=0; i<ports.size(); ++i) {
    lon(i) = port_longitude(ports[i]);     
  }
  return lon;
}

float DataFrame::arrival_port_longitude(size_t r) const {
  return port_longitude((*this)(r, SHIP_ARRIVAL_PORT));
}

Eigen::VectorXf DataFrame::arrival_port_latitude() const {
  const auto ports = col_view(SHIP_ARRIVAL_PORT);
  Eigen::VectorXf lat(ports.size());
  for(size_t i=0; i<ports.size(); ++i) {
    lat(i) = port_latitude(ports[i]);     
  }
  return lat;
}

float DataFrame::arrival_port_latitude(size_t r) const {
  return port_latitude((*this)(r, SHIP_ARRIVAL_PORT));
}

// function: time_to_arrive
Eigen::VectorXf DataFrame::time_to_arrive() const {
  return _data.col(SHIP_ARRIVAL_TIME) - _data.col(SHIP_TIMESTAMP);
}

float DataFrame::time_to_arrive(size_t r) const {
  return _data(r, SHIP_ARRIVAL_TIME) - _data(r, SHIP_TIMESTAMP);
}

// Function: get
float DataFrame::get(size_t r, Index i) const {
  
  switch(i) {

    case ID:
      return id(r);
    break;

    case TYPE:
      return type(r);
    break;

    case SPEED:
      return speed(r);
    break;
    
    case LONGITUDE:
      return longitude(r);
    break;

    case LATITUDE:
      return latitude(r);
    break;

    case COURSE:
      return course(r);
    break;
    
    case HEADING:
      return heading(r);
    break;

    case TIMESTAMP:
      return timestamp(r);
    break;
    
    case DEPARTURE_PORT:
      return departure_port(r);
    break;

    case DRAUGHT:
      return draught(r);
    break;

    case ARRIVAL_TIME:
      return arrival_time(r);
    break;

    case ARRIVAL_PORT:
      return arrival_port(r);
    break;

    case DISTANCE_FROM_DEPARTURE:
      return distance_from_departure(r);
    break;

    case TIME_TO_ARRIVE:
      return time_to_arrive(r);
    break;

    case DEPARTURE_PORT_LONGITUDE:
      return departure_port_longitude(r);
    break;

    case DEPARTURE_PORT_LATITUDE:
      return departure_port_latitude(r);
    break;

    case ARRIVAL_PORT_LONGITUDE:
      return arrival_port_longitude(r);
    break;

    case ARRIVAL_PORT_LATITUDE:
      return arrival_port_latitude(r);
    break;

    default:
      assert(false);
    break;
  };

}

// Function: trips
std::vector<Eigen::MatrixXf> DataFrame::trips(const std::vector<Index>& ids) const {

  std::unordered_map<std::string, std::vector<int>> ships;
  for(int i=0; i<_data.rows(); ++i) {
    ships[(*this)(i, SHIP_ID)].push_back(i);
  }

  std::vector<Eigen::MatrixXf> trips;
  
  for(auto& kvp : ships) {

    auto& rows = kvp.second;

    // Sort according to timestamp.
    std::sort(rows.begin(), rows.end(), [&] (int a, int b) {
      return _data(a, SHIP_TIMESTAMP) < _data(b, SHIP_TIMESTAMP);
    });

    // Generate trips for this ship
    float preport = -1.0f;
    size_t count {0};
    for(size_t i=0; i<rows.size(); ++i) {
      if(_data(rows[i], SHIP_DEPARTURE_PORT) != preport && count != 0) {
        Eigen::MatrixXf& mat = trips.emplace_back(count, ids.size()); 
        for(size_t j=i-count, k=0; j<i; ++j, ++k) {
          //mat.row(k) = _data.row(rows[j]);
          for(size_t f=0; f<ids.size(); ++f) {
            mat(k, f) = get(rows[j], ids[f]);
          }
        }
        count = 0;
      }
      preport = _data(rows[i], SHIP_DEPARTURE_PORT);
      count++;
    }

    if(count != 0) {
      Eigen::MatrixXf& mat = trips.emplace_back(count, ids.size());
      for(size_t j=rows.size()-count, k=0; j<rows.size(); ++j, ++k) {
        //mat.row(k) = _data.row(rows[j]);
        for(size_t f=0; f<ids.size(); ++f) {
          mat(k, f) = get(rows[j], ids[f]);
        }
      }
      count = 0;
    }
  }
  
  return trips;
}

// Function: data
const Eigen::MatrixXf& DataFrame::data() const {
  return _data;
}

// ------------------------------------------------------------------------------------------------

void regression_per_trip(const std::filesystem::path& path) {

  DataFrame df(path);
  
  auto trips = df.trips({
    debs18::DataFrame::TIMESTAMP,
    debs18::DataFrame::TYPE,
    debs18::DataFrame::SPEED,
    debs18::DataFrame::LONGITUDE,
    debs18::DataFrame::LATITUDE,
    debs18::DataFrame::COURSE,
    debs18::DataFrame::DEPARTURE_PORT,
    debs18::DataFrame::ARRIVAL_TIME
  });
  
  dtc::cout(
    "DEBS18 data: ", path, '\n',
    "# rows     : ", df.num_rows(), '\n',
    "# cols     : ", df.num_cols(), '\n',
    "# trips    : ", trips.size(), '\n'
  );

  // Purify the data
  dtc::LOGI("Removing rows with NaN ...");
  for(size_t i=0; i<trips.size(); ++i) {
    remove_NaN_rows(trips[i]);
  }

  // Remove rows with invalid timestamp
  // TODO

  for(const auto& t : trips) {
    std::cout << t << "\n\n";
  }

  // Create a regressor
  //dtc::ml::DnnRegressor dnng;
  //dnng.fully_connected_layer(7, 30, dtc::ml::Activation::RELU)
  //    .fully_connected_layer(30, 1, dtc::ml::Activation::NONE)
  //    //.loss(dtc::ml::Loss::MEAN_ABSOLUTE_ERROR)
  //    .loss<dtc::ml::MeanAbsoluteError>()
  //    //.loss<dtc::ml::HuberLoss>(720.0f)
  //    .optimizer<dtc::ml::AdamOptimizer>();

  /*Eigen::MatrixXf D;
  Eigen::VectorXf L;

  for(size_t itr=0; itr<10; ++itr) {
    auto tbeg = std::chrono::steady_clock::now();
    for(const auto& [key, routes] : trips) {
      //dtc::cout("Ship ", key, " [# routes=", routes.size(), "]\n"); 
      for(size_t i=0; i<routes.size(); ++i) {
        //dtc::cout("----> training route[", i, "]: data (", routes[i].rows(), 'x', routes[i].cols(), ") ... ");
        D = routes[i].leftCols(routes[i].cols()-1);
        L = routes[i].rightCols(1);
        float mae_b = (dnng.infer(D) - L).array().abs().sum() / (static_cast<float>(D.rows()));
        dnng.train(D, L, 64, 256, 0.01f, [&, i=0] (dtc::ml::DnnRegressor& dnng) mutable {
           //float mae = (dnng.infer(D) - L).array().abs().sum() / (static_cast<float>(D.rows()));
           //printf("Epoch %d: mae=%.4f\n", ++i, mae);
        });
        float mae_a = (dnng.infer(D) - L).array().abs().sum() / (static_cast<float>(D.rows()));
        //dtc::cout("mae(before, after)=(", mae_b, ',', mae_a, ")\n");
      }
    }
    auto tend = std::chrono::steady_clock::now();

    D = W.leftCols(W.cols()-1);
    L = W.rightCols(1);  
    std::cout << "Global mae=" << (dnng.infer(D) - L).array().abs().sum() / (static_cast<float>(D.rows())) << std::endl;
  }*/
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

  Eigen::MatrixXf raw = df.get({
    debs18::DataFrame::TIMESTAMP,
    debs18::DataFrame::TYPE,
    debs18::DataFrame::SPEED,
    debs18::DataFrame::LONGITUDE,
    debs18::DataFrame::LATITUDE,
    debs18::DataFrame::COURSE,
    debs18::DataFrame::DEPARTURE_PORT,
    debs18::DataFrame::ARRIVAL_TIME
  });

  std::cout << "Raw data (top 30 raws):\n";
  if(raw.rows() >= 30) std::cout << raw.topRows(30) << " ... (more)\n";
  else std::cout << raw << std::endl;

  // Remove NaN rows
  dtc::LOGI("Removing rows with NaN ...");
  debs18::remove_NaN_rows(raw);

  // Remove redundant
  dtc::LOGI("Removing rows with redundant timestamp ...");
  {
    int rows = 0;
    for(int i=0; i<raw.rows(); ++i) {
      if(raw(i, 0) <= raw(i, 7)) {
        rows++;
      }
    }

    Eigen::MatrixXf mat(rows, raw.cols());

    for(int i=0, j=0; i<raw.rows(); ++i) {
      if(raw(i, 0) <= raw(i, 7)) {
        mat.row(j++) = raw.row(i);
      }
    }

    assert(mat.rows() == rows);
    raw = mat;
  }
  
  // Extract the training data.
  std::cout << "Ship data (top 30 rows):\n";
  auto& ship = raw;
  if(ship.rows() >= 30) std::cout << ship.topRows(30) << " ... (more)\n";
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
    "Dtr: ", Dtr.rows(), "x", Dtr.cols(), ", ",
    "Ltr: ", Ltr.rows(), "x", Ltr.cols(), '\n',
    "Dte: ", Dte.rows(), "x", Dte.cols(), ", ",
    "Lte: ", Lte.rows(), "x", Lte.cols(), '\n'
  );
  
  Eigen::MatrixXf comp(Dte.rows(), 3);    
  
  // Train
  dtc::ml::DnnRegressor dnng;
  dnng.fully_connected_layer(Dtr.cols(), 30, dtc::ml::Activation::RELU)
      .fully_connected_layer(30, 1, dtc::ml::Activation::NONE)
      .loss<dtc::ml::MeanAbsoluteError>();

  dnng.optimizer<dtc::ml::AdamOptimizer>();
  
  // Record the prediction before training.
  comp.col(0) = dnng.infer(Dte);
  
  // Perform training.
  dnng.train(Dtr, Ltr, 256, 256, 0.01f, [&, i=0] (dtc::ml::DnnRegressor& dnng) mutable {
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




