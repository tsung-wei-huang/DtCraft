#include <dtc/contrib/debs18/dataframe.hpp>

namespace dtc::debs18 {

std::string minutes_to_timestamp(int minutes) {
  static const auto min_timepoint = timestamp_to_timepoint(min_timestamp);
  auto cur_timepoint = min_timepoint + std::chrono::minutes(minutes);
  auto tt = std::chrono::system_clock::to_time_t(cur_timepoint);
  char buffer[128];
  if(::std::strftime(buffer, sizeof(buffer),"%d-%m-%y %H:%M", std::gmtime(&tt)) == 0){ 
    throw std::invalid_argument("Failed on strftime");
  }
  return std::string(buffer);
}


//// Function: statistics
//std::tuple<int, int, int, int> statistics(const std::vector<Trip>& trips) {
//
//  int N {0};
//  int feature_columns {0};
//  int max_trip_length {0};
//  int min_trip_length {std::numeric_limits<int>::max()};
//
//  for(size_t i=0; i<trips.size(); ++i) {
//
//    assert(trips[i].rows() != 0 && trips[i].cols() >= 2);
//
//    if(feature_columns == 0) {
//      feature_columns = trips[i].cols() - 1;
//    }
//    else {
//      assert(feature_columns == trips[i].cols() - 1);
//    }
//
//    N += trips[i].rows();
//
//    max_trip_length = std::max(max_trip_length, (int)trips[i].rows());
//    min_trip_length = std::min(min_trip_length, (int)trips[i].rows());
//  }
//
//  return {N, feature_columns, max_trip_length, min_trip_length};
//}

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
float make_type(const std::string& v, bool scale) {
  try {
    if(float res = std::stof(v); scale == true) {
      return scale_type(res);
    }
    else {
      return res;
    }
  }catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_type
Eigen::VectorXf make_type(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_TYPE);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_type(view[i], scale);
  }
  return vec;
}

// Function: make_speed
float make_speed(const std::string& v, bool scale) {
  try {
    if(float res = std::stof(v); scale == true) {
      return scale_speed(res);
    }
    else {
      return res;
    }
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_speed
Eigen::VectorXf make_speed(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_SPEED);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_speed(view[i], scale);
  }
  return vec;
}

// Function: make_longitude
float make_longitude(const std::string& v, bool scale) {
  try {
    if(float res = std::stof(v); scale == true) {
      return scale_longitude(res);
    }
    else {
      return res;
    }
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_longitude
Eigen::VectorXf make_longitude(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_LONGITUDE);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_longitude(view[i], scale);
  }
  return vec;
}

// Function: make_latitude
float make_latitude(const std::string& v, bool scale) {
  try {
    if(float res = std::stof(v); scale == true) {
      return scale_latitude(res);
    }
    else {
      return res;
    }
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_latitude
Eigen::VectorXf make_latitude(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_LATITUDE);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_latitude(view[i], scale);
  }
  return vec;
}

// Function: make_course
float make_course(const std::string& v, bool scale) {
  try {
    if(float res = std::stof(v); scale == true) {
      return scale_course(res);
    }
    else {
      return res;
    }
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_course
Eigen::VectorXf make_course(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_COURSE);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_course(view[i], scale);
  }
  return vec;
}

// Function: make_heading
float make_heading(const std::string& v, bool scale) {
  try {
    if(float res = std::stof(v); scale == true) {
      return scale_heading(res);
    }
    else {
      return res;
    }
  }
  catch(...) {
    return 511.0f;  // unknown
    //return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_heading
Eigen::VectorXf make_heading(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_HEADING);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_heading(view[i], scale);
  }
  return vec;
}

// Function: make_draught
float make_draught(const std::string& v, bool scale) {
  try {
    if(float res = std::stof(v); scale == true) {
      return scale_draught(res);
    }
    else {
      return res;
    }
  }
  catch(...) {
    //return std::numeric_limits<float>::quiet_NaN();
    return 0.0f;
  }
}

// Function: make_draught
Eigen::VectorXf make_draught(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_DRAUGHT);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_draught(view[i], scale);
  }
  return vec;
}

// Function: make_timestamp
float make_timestamp(const std::string& v, bool scale) {
  try {
    if(scale == true) return scale_timestamp(v); 
    else return timestamp_to_minutes(v);
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_timestamp
Eigen::VectorXf make_timestamp(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_TIMESTAMP);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_timestamp(view[i], scale);
  }
  return vec;
}

// Function: make_departure_port
float make_port(const std::string& v, bool scale) {
  try {
    if(scale == true) return scale_departure_port(v);
    else return port_to_id(v);
  }
  catch(...) {
    return std::numeric_limits<float>::quiet_NaN();
  }
}

// Function: make_departure_port
Eigen::VectorXf make_departure_port(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_DEPARTURE_PORT);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_port(view[i], scale);
  }
  return vec;
}

// Function: make_arrival_time
Eigen::VectorXf make_arrival_time(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_ARRIVAL_TIME);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_timestamp(view[i], scale);
  }
  return vec;
}

// Function: make_arrival_port
Eigen::VectorXf make_arrival_port(const dtc::CsvFrame& csv, bool scale) {
  auto view = csv.col(SHIP_ARRIVAL_PORT);
  Eigen::VectorXf vec(view.size());
  for(int i=0; i<vec.size(); ++i) {
    vec(i) = make_port(view[i], scale);
  }
  return vec;
}

// Function: make_ship_data
Eigen::VectorXf make_ship_data(const dtc::CsvFrame& csv, int id, bool scale) {
  
  switch(id) {
    case SHIP_TYPE:
      return make_type(csv, scale); 
    break;

    case SHIP_SPEED:
      return make_speed(csv, scale);
    break;

    case SHIP_LONGITUDE:
      return make_longitude(csv, scale);
    break;
    
    case SHIP_LATITUDE:
      return make_latitude(csv, scale);
    break;

    case SHIP_COURSE:
      return make_course(csv, scale);
    break;

    case SHIP_HEADING:
      return make_heading(csv, scale);
    break;

    case SHIP_TIMESTAMP:
      return make_timestamp(csv, scale);
    break;

    case SHIP_DEPARTURE_PORT:
      return make_departure_port(csv, scale);
    break;

    case SHIP_DRAUGHT:
      return make_draught(csv, scale);
    break;

    case SHIP_ARRIVAL_TIME:
      return make_arrival_time(csv, scale);
    break;

    case SHIP_ARRIVAL_PORT:
      return make_arrival_port(csv, scale);
    break;

    default:
      throw std::runtime_error(std::string{"Unsupported ship data id "} + std::to_string(id));
  };

}

// Function: timestamp_to_timepoint
std::chrono::system_clock::time_point timestamp_to_timepoint(std::string_view time) {
  //assert(!time.empty());
  std::tm tm = {};
  if(::strptime(time.data(), "%d-%m-%y %H:%M", &tm) == nullptr) {
    assert(time.empty());
    throw std::invalid_argument("Failed on strptime");
  }
  return std::chrono::system_clock::from_time_t(::timegm(&tm));
}

// Function: scale_timestamp
float scale_timestamp(std::string_view ts) {
  static auto _min_timepoint = timestamp_to_timepoint(min_timestamp);
  static auto _max_timepoint = timestamp_to_timepoint(max_timestamp);
  static auto _dif_timepoint = _max_timepoint - _min_timepoint;
	auto tp = timestamp_to_timepoint(ts);
  auto nu = std::chrono::duration_cast<std::chrono::minutes>(tp - _min_timepoint).count();
  auto de = std::chrono::duration_cast<std::chrono::minutes>(_dif_timepoint).count();
  return nu / static_cast<float>(de);
}

// Function: timestamp_to_minutes
float timestamp_to_minutes(std::string_view ts) {
  static auto min_timepoint = timestamp_to_timepoint(min_timestamp);
  auto cur_timepoint = timestamp_to_timepoint(ts);
  return std::chrono::duration_cast<std::chrono::minutes>(cur_timepoint - min_timepoint).count();
}

// Function: port_longitude
float port_longitude(std::string_view port) {
  if(port_longitudes.find(port) == port_longitudes.end()) {
    dtc::LOGE("Port ", port, "'s longitude not found");
    return std::numeric_limits<float>::quiet_NaN();
  }
  return port_longitudes.at(port);
}

// Function: port_latitude
float port_latitude(std::string_view port) {
  if(port_latitudes.find(port) == port_latitudes.end()) {
    dtc::LOGE("Port ", port, "'s latitude not found");
    return std::numeric_limits<float>::quiet_NaN();
  }
  return port_latitudes.at(port);
}

// Function: port_to_id
float port_to_id(std::string_view port) {
  if(port_ids.find(port) == port_ids.end()) {
    //dtc::LOGE("Port ", port, "'s id not found");
    return std::numeric_limits<float>::quiet_NaN();
  }
	return port_ids.at(port);
}

// Function: scale_departure_port
float scale_departure_port(std::string_view port) {
  return (port_to_id(port) - min_port_id) / (max_port_id - min_port_id);
}

// Function: scale_course
float scale_course(float d) {
  static auto _dif_course = max_course - min_course;
  return (d - min_course) / _dif_course;
}

// Function: scale_heading
float scale_heading(float d) {
  static auto _dif_heading = max_heading - min_heading;
  return (d - min_heading) / _dif_heading;
}

// Function: scale_type
float scale_type(float d) {
  static auto _dif_type = max_type - min_type;
  return (d - min_type) / _dif_type;
}

// Function: scale_speed
float scale_speed(float d) {
  static auto _dif_speed = max_speed - min_speed;
  return (d - min_speed) / _dif_speed;
}

// Function: scale_draught
float scale_draught(float d) {
  static auto _dif_draught = max_draught - min_draught;
  return (d - min_draught) / _dif_draught;
}

// Function: scale_longitude
float scale_longitude(float l) {
  static auto _dif_longitude = max_longitude - min_longitude; 
  return (l - min_longitude) / _dif_longitude;
}

// Function: scale_latitude
float scale_latitude(float l) {
  static auto _dif_latitude = max_latitude - min_latitude;
  return (l - min_latitude) / _dif_latitude;
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
  u = sin((lat2r - lat1r)/2);
  v = sin((lon2r - lon1r)/2);
  return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
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
  row(0, type_c) = make_type(token, false);

  std::getline(ss, token, ',');       // SPEED
  row(0, speed_c) = make_speed(token, false);

  std::getline(ss, token, ',');       // LON
  row(0, longitude_c) = make_longitude(token, false);

  std::getline(ss, token, ',');       // LAT
  row(0, latitude_c) = make_latitude(token, false);

  std::getline(ss, token, ',');       // COURSE
  row(0, course_c) = make_course(token, false);

  std::getline(ss, token, ',');       // HEADING 
  row(0, heading_c) = make_heading(token, false);

  std::getline(ss, token, ',');       // timestamp
  row(0, timestamp_c) = make_timestamp(token, false);

  std::getline(ss, token, ',');       // departure port
  row(0, departure_port_c) = make_port(token, false);

  std::getline(ss, token, ',');       // draugh
  row(0, draught_c) = make_draught(token, false);

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






};  // end of namespace dtc::debs18. --------------------------------------------------------------
