#include <app/debs18/debs18.hpp>

namespace debs18 {

// Procedure: regression_rnn
void regression_rnn(const std::filesystem::path& path, const std::filesystem::path& model) {

  //dtc::ml::RnnRegressorNx1 rnn;
  //rnn.cell(feature_columns, 20, 1, dtc::ml::Activation::RELU)
  //   .loss<dtc::ml::MeanAbsoluteError>();

  ////for(int itr=0; itr<100; ++itr) {
  ////  dtc::LOGD("Iteration ", itr);
  //  for(const Eigen::MatrixXf& t : trips) {

  //    if(t.rows() == 0) continue;

  //    std::cout << "TRIP\n" << t << "\n\n";

  //    auto max_time_steps = t.rows() > 2 ? 2 : t.rows();
  //    Eigen::MatrixXf Dtr(1, feature_columns*t.rows());
  //    Eigen::MatrixXf Ltr(1, 1);
  //    
  //    // Expand training
  //    for(int i=0; i<t.rows(); ++i) {
  //      Dtr.middleCols(i*feature_columns, feature_columns) = t.row(i).middleCols(0, feature_columns);
  //    }

  //    // Expand label
  //    Ltr = t.row(0).rightCols(1);
  //    
  //    for(int time_step=1; time_step<=max_time_steps; ++time_step) {
  //      for(int r=0; r+time_step<=t.rows(); ++r) {
  //        //std::cout << "training from " << r << " to " << r+time_step << " => "
  //        //          << r*feature_columns << " " << feature_columns*time_step <<std::endl; 
  //        //std::cout << Dtr.middleCols(r*feature_columns, feature_columns*time_step) << std::endl;
  //        Eigen::MatrixXf series = Dtr.middleCols(r*feature_columns, feature_columns*time_step);
  //        assert(series.rows() != 0);
  //        std::cout << "training series: " << series << std::endl;
  //        rnn.train(series, Ltr, 1, 1, 0.01f, [&] (auto& rnn) mutable {
  //          //std::cout << rnn.infer(series) << " " << Ltr << std::endl;
  //          std::cout << "training error=" << (rnn.infer(series) - Ltr).array().abs().sum() << std::endl;
  //        });
  //        // TODO debug
  //      }
  //    }
  //  }
  ////}
}


};  // end of namespace debs18 --------------------------------------------------------------------
