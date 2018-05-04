#ifndef DTC_CONTRIB_DEBS18_QUERY2_HPP_
#define DTC_CONTRIB_DEBS18_QUERY2_HPP_

#include <dtc/contrib/debs18/dataframe.hpp>

namespace dtc::debs18 {

// Procedure: regression_dnn
void regression_dnn(const std::filesystem::path&, const std::filesystem::path&);


// Class: ArrivalTimePredictor
//    debs18::DataFrame::TIMESTAMP,
//    debs18::DataFrame::TYPE,
//    debs18::DataFrame::SPEED,
//    debs18::DataFrame::LONGITUDE,
//    debs18::DataFrame::LATITUDE,
//    debs18::DataFrame::COURSE,
//    debs18::DataFrame::CUMULATIVE_DISTANCE,
//    debs18::DataFrame::CUMULATIVE_TIME,
//    debs18::DataFrame::HEADING,
//    debs18::DataFrame::BEARING,
//    debs18::DataFrame::ARRIVAL_TIME
//

class ArrivalTimePredictor {

  struct Feature {
    std::string id;
    int type;
    float speed;
    float longitude;
    float latitude;
    float course;
    float heading;
    float timestamp;
    std::string departure_port;
    float draught;
  };
  
  struct Ship {
    Feature feature;
    Eigen::MatrixXf data{1, 10};
  };
  
  public:

    ArrivalTimePredictor(const std::filesystem::path&);
    ~ArrivalTimePredictor() = default;
    
    std::string infer(const std::string&);

  private:
    
    std::mutex _mutex;
    std::unordered_map<int, ml::DnnRegressor> _regressors;
    std::unordered_map<std::string, Ship> _ships;

    ml::DnnRegressor _default_regressor;

    Feature _extract_feature(const std::string&) const;
};

};  // end of namespace dtc::debs18. --------------------------------------------------------------


#endif



