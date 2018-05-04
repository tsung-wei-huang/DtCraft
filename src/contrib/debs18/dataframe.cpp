#include <dtc/contrib/debs18/dataframe.hpp>

namespace dtc::debs18 {

// Function: timestamp_to_timepoint
std::chrono::system_clock::time_point timestamp_to_timepoint(const std::string& time) {
  //assert(!time.empty());
  std::tm tm = {};
  if(::strptime(time.data(), "%d-%m-%y %H:%M", &tm) == nullptr) {
    throw std::invalid_argument("Failed on strptime");
  }
  return std::chrono::system_clock::from_time_t(::timegm(&tm));
}

// Function: minutes_to_timestamp
std::string minutes_to_timestamp(float minutes) {
  static const auto min_timepoint = timestamp_to_timepoint(min_timestamp);
  auto cur_timepoint = min_timepoint + std::chrono::minutes((int)(minutes));
  auto tt = std::chrono::system_clock::to_time_t(cur_timepoint);
  char buffer[128];
  if(::std::strftime(buffer, sizeof(buffer),"%d-%m-%y %H:%M", std::gmtime(&tt)) == 0){ 
    throw std::invalid_argument("Failed on strftime");
  }
  return std::string(buffer);
}

// Function: timestamp_to_minutes
float timestamp_to_minutes(const std::string& ts) {
  static auto min_timepoint = timestamp_to_timepoint(min_timestamp);
  auto cur_timepoint = timestamp_to_timepoint(ts);
  return std::chrono::duration_cast<std::chrono::minutes>(cur_timepoint - min_timepoint).count();
}

// Procedure: shuffle
void shuffle(Eigen::MatrixXf& D) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(D.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), this_thread::random_engine());
  D = p * D;
}

// Function: stack
Eigen::MatrixXf stack(const std::vector<Trip>& trips) {
  
  Eigen::MatrixXf::Index num_rows {0}, num_cols {0}, k=0;

  for(size_t i=0; i<trips.size(); ++i) {
    num_rows += trips[i].rows();
    if(num_cols == 0) {
      num_cols = trips[i].cols();
    }
    else {
      assert(num_cols == trips[i].cols());
    }
  }

  Eigen::MatrixXf stk(num_rows, num_cols);
  
  for(size_t i=0; i<trips.size(); ++i) {
    for(Eigen::MatrixXf::Index r=0; r<trips[i].rows(); ++r) {
      stk.row(k++) = trips[i].route.row(r);
    }
  }

  assert(k == num_rows);
  
  return stk;
}

// Function: stack_on_type
Eigen::MatrixXf stack_on_type(const std::vector<Trip>& trips, int type) {
  
  assert(10<=type and type <100);

  Eigen::MatrixXf::Index num_rows {0}, num_cols {0}, k=0;

  for(size_t i=0; i<trips.size(); ++i) {
    if(trips[i].type != type) {
      continue;
    }
    num_rows += trips[i].rows();
    if(num_cols == 0) {
      num_cols = trips[i].cols();
    }
    else {
      assert(num_cols == trips[i].cols());
    }
  }

  Eigen::MatrixXf stk(num_rows, num_cols);
  
  for(size_t i=0; i<trips.size(); ++i) {
    if(trips[i].type != type) {
      continue;
    }
    for(Eigen::MatrixXf::Index r=0; r<trips[i].rows(); ++r) {
      stk.row(k++) = trips[i].route.row(r);
    }
  }
  
  assert(k == num_rows);
  
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(k);
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), this_thread::random_engine());
  stk = p * stk;
  
  return stk;
}

// Function: remove_NaN_rows
void remove_NaN_rows(Eigen::MatrixXf& raw) {

  // Remove rows with NaN
  int num_rows_with_NaN {0};
  for(int i=0; i<raw.rows(); ++i) {
    if(raw.row(i).hasNaN()) {
      num_rows_with_NaN++;
    }
  }

  Eigen::MatrixXf purified(raw.rows() - num_rows_with_NaN, raw.cols());

  for(int i=0, j=0; i<raw.rows(); ++i) {
    if(raw.row(i).hasNaN()) {
      continue;
    }
    purified.row(j++) = raw.row(i);
  }
  
  raw = purified;
}

// Function: remove_invalid_timestamp_rows
void remove_invalid_timestamp_rows(Eigen::MatrixXf& raw) {

  assert(raw.cols() >= 2);

  int atid = raw.cols() - 1;
  int tsid = 0;

  int rows = 0;
  for(int i=0; i<raw.rows(); ++i) {
    if(raw(i, tsid) <= raw(i, atid)) {
      rows++;
    }
  }

  Eigen::MatrixXf purified(rows, raw.cols());

  for(int i=0, j=0; i<raw.rows(); ++i) {
    if(raw(i, tsid) <= raw(i, atid)) {
      purified.row(j++) = raw.row(i);
    }
  }

  assert(purified.rows() == rows);

  raw = purified;
}

// Function: make_type
float make_type(const std::string& v) {
  try {
    return std::stof(v);
  }catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_speed
float make_speed(const std::string& v) {
  try {
    return std::stof(v);
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_longitude
float make_longitude(const std::string& v) {
  try {
    return std::stof(v);
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_latitude
float make_latitude(const std::string& v) {
  try {
    return std::stof(v);
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_course
float make_course(const std::string& v) {
  try {
    return std::stof(v);
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_heading
float make_heading(const std::string& v) {
  try {
    return std::stof(v);
  }
  catch(...) {
    return 511.0f;  // unknown
    //return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_draught
float make_draught(const std::string& v) {
  try {
    return std::stof(v);
  }
  catch(...) {
    //return std::numeric_limits<float>::quiet_NaN();
    return 0.0f;
  }
}

// Function: make_timestamp
float make_timestamp(const std::string& v) {
  try {
    return timestamp_to_minutes(v);
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_port
float make_port(const std::string& v) {
  try {
    return ports.at(v).id;
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: port_longitude
float port_longitude(const std::string& port) {
  if(ports.find(port) == ports.end()) {
    //dtc::LOGE("Port ", port, "'s longitude not found");
    return std::numeric_limits<float>::quiet_NaN();
  }
  return ports.at(port).longitude;
}

// Function: port_latitude
float port_latitude(const std::string& port) {
  if(ports.find(port) == ports.end()) {
    //dtc::LOGE("Port ", port, "'s latitude not found");
    return std::numeric_limits<float>::quiet_NaN();
  }
  return ports.at(port).latitude;
}

//// Function: scale_departure_port
//float scale_departure_port(const std::string& port) {
//  return (port_to_id(port) - min_port_id) / (max_port_id - min_port_id);
//}
//
//// Function: scale_course
//float scale_course(float d) {
//  static auto _dif_course = max_course - min_course;
//  return (d - min_course) / _dif_course;
//}
//
//// Function: scale_heading
//float scale_heading(float d) {
//  static auto _dif_heading = max_heading - min_heading;
//  return (d - min_heading) / _dif_heading;
//}
//
//// Function: scale_type
//float scale_type(float d) {
//  static auto _dif_type = max_type - min_type;
//  return (d - min_type) / _dif_type;
//}
//
//// Function: scale_speed
//float scale_speed(float d) {
//  static auto _dif_speed = max_speed - min_speed;
//  return (d - min_speed) / _dif_speed;
//}
//
//// Function: scale_draught
//float scale_draught(float d) {
//  static auto _dif_draught = max_draught - min_draught;
//  return (d - min_draught) / _dif_draught;
//}
//
//// Function: scale_longitude
//float scale_longitude(float l) {
//  static auto _dif_longitude = max_longitude - min_longitude; 
//  return (l - min_longitude) / _dif_longitude;
//}
//
//// Function: scale_latitude
//float scale_latitude(float l) {
//  static auto _dif_latitude = max_latitude - min_latitude;
//  return (l - min_latitude) / _dif_latitude;
//}
//
//// Function: scale_timestamp
//float scale_timestamp(const std::string& ts) {
//  static auto _min_timepoint = timestamp_to_timepoint(min_timestamp);
//  static auto _max_timepoint = timestamp_to_timepoint(max_timestamp);
//  static auto _dif_timepoint = _max_timepoint - _min_timepoint;
//	auto tp = timestamp_to_timepoint(ts);
//  auto nu = std::chrono::duration_cast<std::chrono::minutes>(tp - _min_timepoint).count();
//  auto de = std::chrono::duration_cast<std::chrono::minutes>(_dif_timepoint).count();
//  return nu / static_cast<float>(de);
//}

float nearest_port(float lat, float lon) {

  auto itr = std::min_element(ports.begin(), ports.end(), [&] (const auto& p, const auto& s) {
    auto dp = distance_on_earth(lat, lon, p.second.latitude, p.second.longitude);
    auto ds = distance_on_earth(lat, lon, s.second.latitude, s.second.longitude);
    return dp < ds; 
  });

  assert(itr != ports.end());
  
  return itr->second.id;
}

// This function converts decimal degrees to radians
float deg2rad(float deg) {
  return (deg * M_PI / 180);
}

//  This function converts radians to decimal degrees
float rad2deg(float rad) {
  return (rad * 180 / M_PI);
}

//  Returns the distance between two points on the Earth.
//  Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
//  @param lat1d Latitude of the first point in degrees
//  @param lon1d Longitude of the first point in degrees
//  @param lat2d Latitude of the second point in degrees
//  @param lon2d Longitude of the second point in degrees
//  @return The distance between the two points in kilometers
float distance_on_earth(float lat1d, float lon1d, float lat2d, float lon2d) {
  constexpr auto earthRadiusKm = 6371.0f;
  float lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(lat1d);
  lon1r = deg2rad(lon1d);
  lat2r = deg2rad(lat2d);
  lon2r = deg2rad(lon2d);
  u = std::sin((lat2r - lat1r)/2);
  v = std::sin((lon2r - lon1r)/2);
  return 2.0 * earthRadiusKm * std::asin(std::sqrt(u * u + std::cos(lat1r) * std::cos(lat2r) * v * v));
}

// Function: bearing_on_earth
float bearing_on_earth(float lat1d, float lon1d, float lat2d, float lon2d) {                                                                      
  float lat1r, lat2r;
  lat1r = deg2rad(lat1d);
  lat2r = deg2rad(lat2d);
  auto diffLong = deg2rad(lon2d - lon1d);
  auto x = std::sin(diffLong)*std::cos(lat2r);
  auto y = std::cos(lat1r)*std::sin(lat2r) - std::sin(lat1r)*std::cos(lat2r)*std::cos(diffLong);
  return std::fmod((rad2deg(std::atan2(x,y))+360.0f), 360.0f);
}

// Function: make_regression_features
Eigen::MatrixXf make_regression_features(const std::string& tuple) {

	Eigen::MatrixXf row(1, 9);

  std::istringstream ss(tuple);
  std::string token; 
    
  constexpr int timestamp_c = 0;
  constexpr int type_c = 1;
  constexpr int speed_c = 2;
  constexpr int longitude_c = 3;
  constexpr int latitude_c = 4;
  constexpr int course_c = 5;
  constexpr int departure_port_c = 6;
  constexpr int heading_c = 7;
  constexpr int draught_c = 8;  

  std::getline(ss, token, ',');       // ID (ignore)

  std::getline(ss, token, ',');       // TYPE 
  row(0, type_c) = make_type(token);

  std::getline(ss, token, ',');       // SPEED
  row(0, speed_c) = make_speed(token);

  std::getline(ss, token, ',');       // LON
  row(0, longitude_c) = make_longitude(token);

  std::getline(ss, token, ',');       // LAT
  row(0, latitude_c) = make_latitude(token);

  std::getline(ss, token, ',');       // COURSE
  row(0, course_c) = make_course(token);

  std::getline(ss, token, ',');       // HEADING 
  row(0, heading_c) = make_heading(token);

  std::getline(ss, token, ',');       // timestamp
  row(0, timestamp_c) = make_timestamp(token);

  std::getline(ss, token, ',');       // departure port
  row(0, departure_port_c) = make_port(token);

  std::getline(ss, token, ',');       // draugh
  row(0, draught_c) = make_draught(token);

  return row;
}


// ------------------------------------------------------------------------------------------------

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

  for(size_t j=0; j<num_cols(); ++j) {
    if(j == SHIP_ID) {
      continue;
    }

    switch(j) {
      case SHIP_ID:
        continue;
      break;

      case SHIP_TYPE:
        _data.col(j) = _make_type();
      break;

      case SHIP_SPEED:
        _data.col(j) = _make_speed();
      break;

      case SHIP_LONGITUDE:
        _data.col(j) = _make_longitude();
      break;

      case SHIP_LATITUDE:
        _data.col(j) = _make_latitude();
      break;

      case SHIP_COURSE:
        _data.col(j) = _make_course();
      break;

      case SHIP_HEADING:
        _data.col(j) = _make_heading();
      break;

      case SHIP_TIMESTAMP:
        _data.col(j) = _make_timestamp();
      break;

      case SHIP_DEPARTURE_PORT:
        _data.col(j) = _make_departure_port();
      break;

      case SHIP_DRAUGHT:
        _data.col(j) = _make_draught();
      break;

      case SHIP_ARRIVAL_TIME:
        _data.col(j) = _make_arrival_time();
      break;

      case SHIP_ARRIVAL_PORT:
        _data.col(j) = _make_arrival_port();
      break;

      default:
        throw std::invalid_argument("invalid ship index "s + std::to_string(j));
      break;
    }
  }

}

float DataFrame::_id(size_t r) const {
  return _data(r, SHIP_ID);
}

float DataFrame::_type(size_t r) const {
  return _data(r, SHIP_TYPE);
}

float DataFrame::_speed(size_t r) const {
  return _data(r, SHIP_SPEED);
}

float DataFrame::_longitude(size_t r) const {
  return _data(r, SHIP_LONGITUDE);
}

float DataFrame::_latitude(size_t r) const {
  return _data(r, SHIP_LATITUDE);
}

float DataFrame::_course(size_t r) const {
  return _data(r, SHIP_COURSE);
}

float DataFrame::_heading(size_t r) const {
  return _data(r, SHIP_HEADING);
}

float DataFrame::_timestamp(size_t r) const {
  return _data(r, SHIP_TIMESTAMP);
}

float DataFrame::_departure_port(size_t r) const {
  return _data(r, SHIP_DEPARTURE_PORT);
}

float DataFrame::_draught(size_t r) const {
  return _data(r, SHIP_DRAUGHT);
}

float DataFrame::_arrival_time(size_t r) const {
  return _data(r, SHIP_ARRIVAL_TIME);
}

float DataFrame::_arrival_port(size_t r) const {
  return _data(r, SHIP_ARRIVAL_PORT);
}

float DataFrame::_distance_from_departure(size_t r) const {
  float slon = _data(r, SHIP_LONGITUDE);
  float slat = _data(r, SHIP_LATITUDE);
  float plon = port_longitude((*this)(r, SHIP_DEPARTURE_PORT));
  float plat = port_latitude((*this)(r, SHIP_DEPARTURE_PORT));
  return distance_on_earth(slat, slon, plat, plon);
}

float DataFrame::_departure_port_longitude(size_t r) const {
  return port_longitude((*this)(r, SHIP_DEPARTURE_PORT));
}

float DataFrame::_departure_port_latitude(size_t r) const {
  return port_latitude((*this)(r, SHIP_DEPARTURE_PORT));
}

float DataFrame::_arrival_port_longitude(size_t r) const {
  return port_longitude((*this)(r, SHIP_ARRIVAL_PORT));
}

float DataFrame::_arrival_port_latitude(size_t r) const {
  return port_latitude((*this)(r, SHIP_ARRIVAL_PORT));
}

float DataFrame::_time_to_arrive(size_t r) const {
  return _data(r, SHIP_ARRIVAL_TIME) - _data(r, SHIP_TIMESTAMP);
}

// Function: get
float DataFrame::_get(size_t r, Index i) const {
  
  switch(i) {

    case ID:
      return _id(r);
    break;

    case TYPE:
      return _type(r);
    break;

    case SPEED:
      return _speed(r);
    break;
    
    case LONGITUDE:
      return _longitude(r);
    break;

    case LATITUDE:
      return _latitude(r);
    break;

    case COURSE:
      return _course(r);
    break;
    
    case HEADING:
      return _heading(r);
    break;

    case TIMESTAMP:
      return _timestamp(r);
    break;
    
    case DEPARTURE_PORT:
      return _departure_port(r);
    break;

    case DRAUGHT:
      return _draught(r);
    break;

    case ARRIVAL_TIME:
      return _arrival_time(r);
    break;

    case ARRIVAL_PORT:
      return _arrival_port(r);
    break;

    case DISTANCE_FROM_DEPARTURE:
      return _distance_from_departure(r);
    break;

    case TIME_TO_ARRIVE:
      return _time_to_arrive(r);
    break;

    case DEPARTURE_PORT_LONGITUDE:
      return _departure_port_longitude(r);
    break;

    case DEPARTURE_PORT_LATITUDE:
      return _departure_port_latitude(r);
    break;

    case ARRIVAL_PORT_LONGITUDE:
      return _arrival_port_longitude(r);
    break;

    case ARRIVAL_PORT_LATITUDE:
      return _arrival_port_latitude(r);
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
    
    // Create a new trip
    auto add_trip = [&] (size_t i, size_t count) {
      Eigen::MatrixXf& mat = trips.emplace_back(key, count, ids.size()).route; 
      for(size_t j=i-count, k=0; j<i; ++j, ++k) {
        trips.back().type = _get(rows[j], TYPE);
        //mat.row(k) = _data.row(rows[j]);
        for(size_t f=0; f<ids.size(); ++f) {
          // Case 1: cumulative distance
          if(ids[f] == CUMULATIVE_DISTANCE) {
            if(k==0) {
              mat(k, f) = _distance_from_departure(rows[j]);
            }
            else {
              float clng = _get(rows[j], LONGITUDE);
              float clat = _get(rows[j], LATITUDE);
              float plng = _get(rows[j-1], LONGITUDE);
              float plat = _get(rows[j-1], LATITUDE);
              mat(k, f) = distance_on_earth(clat, clng, plat, plng) + mat(k-1, f);
            }
          }
          // Case 2: adjacent bearing
          else if(ids[f] == BEARING) {
            if(k==0) {
              mat(k, f) = 0.0f;
            }
            else {
              mat(k, f) = bearing_on_earth(
                _get(rows[j-1], LATITUDE), 
                _get(rows[j-1], LONGITUDE),
                _get(rows[j], LATITUDE), 
                _get(rows[j], LONGITUDE)                
              );
            }
          }
          // Case 3: delta longitude
          else if(ids[f] == DELTA_LONGITUDE) {
            if(k == 0) {
              mat(k, f) = 0.0f;
            }
            else {
              mat(k, f) = _get(rows[j], LONGITUDE) - _get(rows[j-1], LONGITUDE);
            }
          }
          // Case 4: delta latitude
          else if(ids[f] == DELTA_LATITUDE) {
            if(k == 0) {
              mat(k, f) = 0.0f;
            }
            else {
              mat(k, f) = _get(rows[j], LATITUDE) - _get(rows[j-1], LATITUDE);
            }
          }
          // Case 5: delta timestamp
          else if(ids[f] == DELTA_TIMESTAMP) {
            if(k == 0) {
              mat(k, f) = 0.0f;
            }
            else {
              mat(k, f) = _get(rows[j], TIMESTAMP) - _get(rows[j-1], TIMESTAMP);
            }
          }
          // Case 6: cumulative time
          else if(ids[f] == CUMULATIVE_TIME) {
            if(k==0) {
              mat(k, f) = 0.0f;
            }
            else {
              mat(k, f) = _get(rows[j], TIMESTAMP) - _get(rows[j-1], TIMESTAMP) + mat(k-1, f);
            }
          }
          // others
          else {
            mat(k, f) = _get(rows[j], ids[f]);
          }
        }
      }
    };

    for(size_t i=0; i<rows.size(); ++i) {
      if((_data(rows[i], SHIP_DEPARTURE_PORT) != preport || 
          std::fabs(_data(rows[i], TIMESTAMP) - _data(rows[i-1], TIMESTAMP)) >= 1440.0f) && count != 0) {
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

// Function: types
std::set<int> DataFrame::types() const {
  std::set<int> types;
  for(size_t r=0; r<num_rows(); ++r) {
    types.insert( std::stoi((*this)(r, SHIP_TYPE)) );
  }
  return types;
}

// Function: type
Eigen::VectorXf DataFrame::_make_type() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_type((*this)(r, SHIP_TYPE));
  }
  return vec;
}

// Function: speed
Eigen::VectorXf DataFrame::_make_speed() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_speed((*this)(r, SHIP_SPEED));
  }
  return vec;
}

// Function: longitude
Eigen::VectorXf DataFrame::_make_longitude() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_longitude((*this)(r, SHIP_LONGITUDE));
  }
  return vec;
}

// Function: latitude
Eigen::VectorXf DataFrame::_make_latitude() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_latitude((*this)(r, SHIP_LATITUDE));
  }
  return vec;
}

// Function: course
Eigen::VectorXf DataFrame::_make_course() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_course((*this)(r, SHIP_COURSE));
  }
  return vec;
}

// Function: heading
Eigen::VectorXf DataFrame::_make_heading() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_heading((*this)(r, SHIP_HEADING));
  }
  return vec;
}

// Function: timestamp
Eigen::VectorXf DataFrame::_make_timestamp() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_timestamp((*this)(r, SHIP_TIMESTAMP));
  }
  return vec;
}

// Function: departure_port
Eigen::VectorXf DataFrame::_make_departure_port() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_port((*this)(r, SHIP_DEPARTURE_PORT));
  }
  return vec;
}

// Function: draught
Eigen::VectorXf DataFrame::_make_draught() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_draught((*this)(r, SHIP_DRAUGHT));
  }
  return vec;
}

// Function: arrival_time
Eigen::VectorXf DataFrame::_make_arrival_time() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_timestamp((*this)(r, SHIP_ARRIVAL_TIME));
  }
  return vec;
}

// Function: arrival_port
Eigen::VectorXf DataFrame::_make_arrival_port() const {
  Eigen::VectorXf vec(num_rows());
  for(size_t r=0; r<num_rows(); ++r) {
    vec(r) = make_port((*this)(r, SHIP_ARRIVAL_PORT));
  }
  return vec;
}

};  // end of namespace dtc::debs18. --------------------------------------------------------------













