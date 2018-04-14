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

inline static const std::unordered_map<std::string_view, float> port_ids {
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
  {"YALOVA",            24},
};

inline static const std::unordered_map<std::string_view, float> port_longitudes {
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

inline static const std::unordered_map<std::string_view, float> port_latitudes {
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
};

// ------------------------------------------------------------------------------------------------

// Struct Trip
struct Trip {

  std::string id;

  Eigen::MatrixXf route;

  Trip(const std::string i, size_t r, size_t c) : id {i}, route {r, c} {}

  auto rows() const { return route.rows(); }
  auto cols() const { return route.cols(); }
};

// ------------------------------------------------------------------------------------------------

// Utility
std::chrono::system_clock::time_point timestamp_to_timepoint(std::string_view);

float scale_type(float);
float scale_speed(float);
float scale_longitude(float);
float scale_latitude(float);
float scale_course(float);
float scale_heading(float);
float scale_draught(float);
float scale_timestamp(std::string_view);
float scale_departure_port(std::string_view);
float port_to_id(std::string_view);
float timestamp_to_minutes(std::string_view);
float deg2rad(float);
float rad2deg(float);
float distance_on_earth(float, float, float, float);
float port_latitude(std::string_view);
float port_longitude(std::string_view);

std::string minutes_to_timestamp(int);

std::string_view nearest_port(float, float);

Eigen::VectorXf make_type(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_speed(const dtc::CsvFrame&, bool=false);
Eigen::VectorXf make_longitude(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_latitude(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_course(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_heading(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_draught(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_timestamp(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_departure_port(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_arrival_time(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_arrival_port(const dtc::CsvFrame&, bool = false);
Eigen::VectorXf make_ship_data(const dtc::CsvFrame&, int, bool = false);

Eigen::MatrixXf make_regression_features(const std::string&);

void remove_NaN_rows(Eigen::MatrixXf&);
void remove_invalid_timestamp_rows(Eigen::MatrixXf&);

Eigen::MatrixXf stack(const std::vector<Trip>&);

std::tuple<int, int, int, int> statistics(const std::vector<Trip>&);

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
      PLACEHOLDER
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

    std::vector<Trip> trips(const std::vector<Index>&) const;

  private:

    Eigen::MatrixXf _data;
    
    float _scale_timestamp(float) const;
};


};  // end of namespace dtc::debs18. --------------------------------------------------------------

#endif
