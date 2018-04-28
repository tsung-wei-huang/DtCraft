#ifndef DTC_CONTRIB_DEBS18_DATAFRAME_HPP_
#define DTC_CONTRIB_DEBS18_DATAFRAME_HPP_

#include <dtc/dtc.hpp>

namespace dtc::debs18 {

// Predefined variables
constexpr auto NUM_SHIP_ATTRIBUTES = 12;
constexpr auto SHIP_ID = 0;
constexpr auto SHIP_TYPE = 1;
constexpr auto SHIP_SPEED = 2;
constexpr auto SHIP_LONGITUDE = 3;
constexpr auto SHIP_LATITUDE = 4;
constexpr auto SHIP_COURSE = 5;
constexpr auto SHIP_HEADING = 6;
constexpr auto SHIP_TIMESTAMP = 7;
constexpr auto SHIP_DEPARTURE_PORT = 8;
constexpr auto SHIP_DRAUGHT = 9;
constexpr auto SHIP_ARRIVAL_TIME = 10;
constexpr auto SHIP_ARRIVAL_PORT = 11;

// Feature ranges.
constexpr auto min_type      = 0.0f;
constexpr auto max_type      = 99.0f;
constexpr auto min_course    = 0.0f;
constexpr auto max_course    = 360.0f;
constexpr auto min_heading   = 0.0f;
constexpr auto max_heading   = 511.0f;
constexpr auto min_speed     = 0.0f;
constexpr auto max_speed     = 103.0f;
constexpr auto min_timestamp = "10-03-15 0:0";
constexpr auto max_timestamp = "19-05-15 23:59";
constexpr auto min_longitude = -6.0f;
constexpr auto max_longitude = 37.0f;
constexpr auto min_latitude  = 32.0f;
constexpr auto max_latitude  = 45.0f;
constexpr auto min_draught   = 20.0f;
constexpr auto max_draught   = 255.0f;
constexpr auto min_port_id   = 0.0f;
constexpr auto max_port_id   = 24.0f;

struct Port {
  std::string name;
  int id;
  float longitude;
  float latitude;
};

inline static const std::unordered_map<std::string, Port> ports {
  {"ALEXANDRIA",        Port{"ALEXANDRIA",         0, 29.87560, 31.18424}},
  {"AUGUSTA",           Port{"AUGUSTA",            1, 15.21417, 37.19920}},
  {"BARCELONA",         Port{"BARCELONA",          2,  2.15844, 41.35238}},
  {"CARTAGENA",         Port{"CARTAGENA",          3, -0.96793, 37.58017}},
  {"CEUTA",             Port{"CEUTA",              4, -5.31317, 35.89361}},
  {"DAMIETTA",          Port{"DAMIETTA",           5, 31.76100, 31.46800}},
  {"DILISKELESI",       Port{"DILISKELESI",        6, 29.53376, 40.76661}},
  {"FOS SUR MER",       Port{"FOS SUR MER",        7,  4.86513, 43.42291}},
  {"GEMLIK",            Port{"GEMLIK",             8, 29.11552, 40.42740}},
  {"GENOVA",            Port{"GENOVA",             9,  8.90911, 44.40355}},
  {"GIBRALTAR",         Port{"GIBRALTAR",         10, -5.36475, 36.14380}},
  {"HAIFA",             Port{"HAIFA",             11, 35.00391, 32.83316}},
  {"ISKENDERUN",        Port{"ISKENDERUN",        12, 36.17972, 36.68401}},
  {"LIVORNO",           Port{"LIVORNO",           13, 10.30616, 43.56281}},
  {"MARSAXLOKK",        Port{"MARSAXLOKK",        14, 14.54345, 35.82770}},
  {"MONACO",            Port{"MONACO",            15,  7.42689, 43.73548}},
  {"NEMRUT",            Port{"NEMRUT",            16, 26.90016, 38.76553}},
  {"PALMA DE MALLORCA", Port{"PALMA DE MALLORCA", 17,  2.63237, 39.55724}},
  {"PIRAEUS",           Port{"PIRAEUS",           18, 23.61056, 37.94606}},
  {"PORT SAID",         Port{"PORT SAID",         19, 32.32300, 31.24478}},
  {"TARRAGONA",         Port{"TARRAGONA",         20,  1.22472, 41.10103}},
  {"TUZLA",             Port{"TUZLA",             21, 29.29471, 40.83438}},
  {"VALENCIA",          Port{"VALENCIA",          22, -0.31647, 39.44231}},
  {"VALLETTA",          Port{"VALLETTA",          23, 14.51505, 35.89301}},
  {"YALOVA",            Port{"YALOVA",            24, 29.47632, 40.71889}}
};

/*inline static const std::unordered_map<std::string, float> port_ids {
  {"ALEXANDRIA",         0},
  {"AUGUSTA",            1},
  {"BARCELONA",          2},
  {"CARTAGENA",          3},
  {"CEUTA",              4},
  {"DAMIETTA",           5},
  {"DILISKELESI",        6},
  {"FOS SUR MER",        7},
  {"GEMLIK",             8},
  {"GENOVA",             9},
  {"GIBRALTAR",         10},
  {"HAIFA",             11},
  {"ISKENDERUN",        12},
  {"LIVORNO",           13},
  {"MARSAXLOKK",        14},
  {"MONACO",            15},
  {"NEMRUT",            16},
  {"PALMA DE MALLORCA", 17},
  {"PIRAEUS",           18},
  {"PORT SAID",         19},
  {"TARRAGONA",         20},
  {"TUZLA",             21},
  {"VALENCIA",          22},
  {"VALLETTA",          23},
  {"YALOVA",            24}
};

inline static const std::unordered_map<std::string, float> port_longitudes {
  {"ALEXANDRIA",        29.87560},
  {"AUGUSTA",           15.21417},
  {"BARCELONA",          2.15844},
  {"CARTAGENA",         -0.96793},
  {"CEUTA",             -5.31317},
  {"DAMIETTA",          31.76100},
  {"DILISKELESI",       29.53376},
  {"FOS SUR MER",        4.86513},
  {"GEMLIK",            29.11552},
  {"GENOVA",             8.90911},
  {"GIBRALTAR",         -5.36475},
  {"HAIFA",             35.00391},
  {"ISKENDERUN",        36.17972},
  {"LIVORNO",           10.30616},
  {"MARSAXLOKK",        14.54345},
  {"MONACO",             7.42689},
  {"NEMRUT",            26.90016},
  {"PALMA DE MALLORCA",  2.63237},
  {"PIRAEUS",           23.61056},
  {"PORT SAID",         32.32300},
  {"TARRAGONA",          1.22472},
  {"TUZLA",             29.29471},
  {"VALENCIA",          -0.31647},
  {"VALLETTA",          14.51505},
  {"YALOVA",            29.47632}
};

inline static const std::unordered_map<std::string, float> port_latitudes {
  {"ALEXANDRIA",        31.18424}, 
  {"AUGUSTA",           37.19920},
  {"BARCELONA",         41.35238},
  {"CARTAGENA",         37.58017},
  {"CEUTA",             35.89361},
  {"DAMIETTA",          31.46800},
  {"DILISKELESI",       40.76661},
  {"FOS SUR MER",       43.42291},
  {"GEMLIK",            40.42740},
  {"GENOVA",            44.40355},
  {"GIBRALTAR",         36.14380},
  {"HAIFA",             32.83316},
  {"ISKENDERUN",        36.68401},
  {"LIVORNO",           43.56281},
  {"MARSAXLOKK",        35.82770},
  {"MONACO",            43.73548},
  {"NEMRUT",            38.76553},
  {"PALMA DE MALLORCA", 39.55724},
  {"PIRAEUS",           37.94606},
  {"PORT SAID",         31.24478},
  {"TARRAGONA",         41.10103},
  {"TUZLA",             40.83438},
  {"VALENCIA",          39.44231},
  {"VALLETTA",          35.89301},
  {"YALOVA",            40.71889}
};*/

// ------------------------------------------------------------------------------------------------

// Struct Trip
struct Trip {

  std::string id;
  int type;

  Eigen::MatrixXf route;

  inline Trip(const std::string&, size_t, size_t);

  inline auto rows() const;
  inline auto cols() const;
};

// Constructor
inline Trip::Trip(const std::string& i, size_t r, size_t c) : id {i}, route {r, c} {
}

// Length of the trip
inline auto Trip::rows() const {
  return route.rows();
}

// Data size of the trip
inline auto Trip::cols() const {
  return route.cols();
}

// ------------------------------------------------------------------------------------------------

// Utility
std::chrono::system_clock::time_point timestamp_to_timepoint(const std::string&);

//float scale_type(float);
//float scale_speed(float);
//float scale_longitude(float);
//float scale_latitude(float);
//float scale_course(float);
//float scale_heading(float);
//float scale_draught(float);
//float scale_timestamp(const std::string&);
//float scale_departure_port(const std::string&);
float make_type(const std::string&);
float make_speed(const std::string&);
float make_longitude(const std::string&);
float make_latitude(const std::string&);
float make_course(const std::string&);
float make_heading(const std::string&);
float make_draught(const std::string&);
float make_timestamp(const std::string&);
float make_port(const std::string&);
float nearest_port(float, float);
float timestamp_to_minutes(const std::string&);
float deg2rad(float);
float rad2deg(float);
float distance_on_earth(float, float, float, float);
float bearing_on_earth(float, float, float, float);
float port_latitude(const std::string&);
float port_longitude(const std::string&);

std::string minutes_to_timestamp(float);

Eigen::MatrixXf stack(const std::vector<Trip>&);
Eigen::MatrixXf stack_on_type(const std::vector<Trip>&, int);
Eigen::MatrixXf make_regression_features(const std::string&);

void remove_NaN_rows(Eigen::MatrixXf&);
void remove_invalid_timestamp_rows(Eigen::MatrixXf&);
void shuffle(Eigen::MatrixXf&);

// ------------------------------------------------------------------------------------------------

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
      ARRIVAL_PORT_LATITUDE,
      CUMULATIVE_DISTANCE,
      CUMULATIVE_TIME,
      BEARING,
      DELTA_LONGITUDE,
      DELTA_LATITUDE,
      DELTA_TIMESTAMP,
      PLACEHOLDER
    };

    DataFrame(const std::filesystem::path&);

    const Eigen::MatrixXf& data() const;

    std::vector<Trip> trips(const std::vector<Index>&) const;

    std::set<int> types() const;

  private:

    Eigen::MatrixXf _data;
    
    float _get(size_t, Index) const;
    float _id(size_t) const;
    float _type(size_t) const;
    float _speed(size_t) const;
    float _longitude(size_t) const;
    float _latitude(size_t) const;
    float _course(size_t) const;
    float _heading(size_t) const;
    float _timestamp(size_t) const;
    float _departure_port(size_t) const;
    float _draught(size_t) const;
    float _arrival_time(size_t) const;
    float _arrival_port(size_t) const;
    float _distance_from_departure(size_t) const;
    float _time_to_arrive(size_t) const;
    float _departure_port_longitude(size_t) const;
    float _departure_port_latitude(size_t) const;
    float _arrival_port_longitude(size_t) const;
    float _arrival_port_latitude(size_t) const;
    
    Eigen::VectorXf _make_type() const;
    Eigen::VectorXf _make_speed() const;
    Eigen::VectorXf _make_longitude() const;
    Eigen::VectorXf _make_latitude() const;
    Eigen::VectorXf _make_course() const;
    Eigen::VectorXf _make_heading() const;
    Eigen::VectorXf _make_timestamp() const;
    Eigen::VectorXf _make_departure_port() const;
    Eigen::VectorXf _make_draught() const;
    Eigen::VectorXf _make_arrival_time() const;
    Eigen::VectorXf _make_arrival_port() const;

};


// ------------------------------------------------------------------------------------------------

class MeditMap {
  
  private:

    std::vector<std::vector<int>> _grid;

    float _minlat;    // Y
    float _maxlat;
    float _minlon;
    float _maxlon;    // X
    float _scale;

    int _rows;
    int _cols;

  public:
    
    MeditMap(const std::filesystem::path&);

    inline float minlat() const { return _minlat; }
    inline float maxlat() const { return _maxlat; }
    inline float minlon() const { return _minlon; }
    inline float maxlon() const { return _maxlon; }
    inline float scale()  const { return _scale;  }

    inline int rows() const { return _rows; }
    inline int cols() const { return _cols; }

    int operator () (float, float) const;

    std::tuple<int, int> to_grid(float, float) const;

    int distance(float, float, float, float) const;
};


};  // end of namespace dtc::debs18. --------------------------------------------------------------

#endif








