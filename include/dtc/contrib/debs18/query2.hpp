#ifndef DTC_CONTRIB_DEBS18_QUERY2_HPP_
#define DTC_CONTRIB_DEBS18_QUERY2_HPP_

#include <dtc/contrib/debs18/dataframe.hpp>

namespace dtc::debs18 {

// Procedure: regression_dnn
void regression_dnn(const std::filesystem::path&, const std::filesystem::path& = "");

// Procedure: regression_rnn
void regression_rnn(const std::filesystem::path&, const std::filesystem::path& = "");



};  // end of namespace dtc::debs18. --------------------------------------------------------------


#endif
