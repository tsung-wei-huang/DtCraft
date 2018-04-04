#include <app/debs18/debs18.hpp>

namespace debs18 {

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
    return std::numeric_limits<float>::quiet_NaN();
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
    return std::numeric_limits<float>::quiet_NaN();
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
  return std::chrono::system_clock::from_time_t(std::mktime(&tm));
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

};  // end of namespace debs18. -------------------------------------------------------------------
