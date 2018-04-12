#include <app/debs18/debs18.hpp>

namespace debs18 {

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
    
    case PLACEHOLDER:
      return 0.0f;
    break;

    default:
      assert(false);
    break;
  };

}

// Function: trips
std::vector<Trip> DataFrame::trips(const std::vector<Index>& ids) const {

  assert(ids.size() >= 2);

  std::unordered_map<std::string, std::vector<int>> ships;
  for(int i=0; i<_data.rows(); ++i) {
    ships[(*this)(i, SHIP_ID)].push_back(i);
  }

  std::vector<Trip> trips;
  
  for(auto& [key, rows] : ships) {

    // Sort according to timestamp.
    std::sort(rows.begin(), rows.end(), [&] (int a, int b) {
      return _data(a, SHIP_TIMESTAMP) < _data(b, SHIP_TIMESTAMP);
    });

    // Generate trips for this ship
    float preport = -1.0f;
    size_t count {0};

    auto add_trip = [&] (size_t i, size_t count) {
      Eigen::MatrixXf& mat = trips.emplace_back(key, count, ids.size()).route; 
      for(size_t j=i-count, k=0; j<i; ++j, ++k) {
        //mat.row(k) = _data.row(rows[j]);
        for(size_t f=0; f<ids.size(); ++f) {
          if(ids[f] == CUMULATIVE_DISTANCE) {
            if(k==0) {
              mat(k, f) = distance_from_departure(rows[j]);
            }
            else {
              float clng = get(rows[j], LONGITUDE);
              float clat = get(rows[j], LATITUDE);
              float plng = get(rows[j-1], LONGITUDE);
              float plat = get(rows[j-1], LATITUDE);
              mat(k, f) = distance_on_earth(clat, clng, plat, plng) + mat(k-1, f);
            }
          }
          else {
            mat(k, f) = get(rows[j], ids[f]);
          }
        }
      }
    };

    for(size_t i=0; i<rows.size(); ++i) {
      if(_data(rows[i], SHIP_DEPARTURE_PORT) != preport && count != 0) {
        add_trip(i, count);
        count = 0;
      }
      preport = _data(rows[i], SHIP_DEPARTURE_PORT);
      count++;
    }

    if(count != 0) {
      add_trip(rows.size(), count);
      count = 0;
    }
  }
  
  return trips;
}

// Function: data
const Eigen::MatrixXf& DataFrame::data() const {
  return _data;
}

}; // end of namespace debs18 ---------------------------------------------------------------------
