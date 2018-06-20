/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#define CATCH_CONFIG_MAIN 

#include <dtc/unittest/catch.hpp>
#include <dtc/dtc.hpp>

constexpr auto mnist_image_file = DTC_HOME "/benchmark/mnist/train-images.idx3-ubyte";
constexpr auto mnist_label_file = DTC_HOME "/benchmark/mnist/train-labels.idx1-ubyte";

// Function: parse_mnist
std::tuple<Eigen::MatrixXf, Eigen::VectorXi, Eigen::MatrixXf, Eigen::VectorXi> parse_mnist() {

  REQUIRE(std::filesystem::exists(mnist_image_file));
  REQUIRE(std::filesystem::exists(mnist_label_file));

  Eigen::MatrixXf images = dtc::ml::read_mnist_image<Eigen::MatrixXf>(mnist_image_file) / 255.0;
  Eigen::VectorXi labels = dtc::ml::read_mnist_label<Eigen::VectorXi>(mnist_label_file);
    
  REQUIRE(images.rows() == labels.rows());
  REQUIRE(images.cols() == 784);
  REQUIRE(labels.cols() == 1);

  const int N = images.rows();
  const int num_infers = std::max(10000, N/10);
  const int num_trains = N - num_infers;
    
  Eigen::MatrixXf Dtr = images.middleRows(0, num_trains);
  Eigen::MatrixXf Dte = images.middleRows(num_trains, num_infers);

  Eigen::VectorXi Ltr = labels.middleRows(0, num_trains);
  Eigen::VectorXi Lte = labels.middleRows(num_trains, num_infers);

  return {Dtr, Ltr, Dte, Lte};
}

// Function: regression_data
std::tuple<Eigen::MatrixXf, Eigen::MatrixXf> regression_data(size_t rows, size_t cols, size_t labels) {
	
	Eigen::MatrixXf X = Eigen::MatrixXf::Random(rows, cols);
  Eigen::MatrixXf Y = Eigen::MatrixXf::Random(rows, labels);

	return {X, Y};
}

// ------------------------------------------------------------------------------------------------

// Testcase: DnnClassifierTest
TEST_CASE("ClassifierTest.Dnn") {
  
  auto [Dtr, Ltr, Dte, Lte] = parse_mnist();
  
  dtc::ml::DnnClassifier nn1;

  nn1.layer<dtc::ml::FullyConnectedLayer>(784, 100, dtc::ml::Activation::RELU);
  nn1.layer<dtc::ml::BatchNormLayer>(100, 30, dtc::ml::Activation::RELU);
  nn1.layer<dtc::ml::FullyConnectedLayer>(30, 10);

  nn1.train(Dtr, Ltr, 20, 64, 0.01f, [&, i=0] (auto& nn1) mutable {
        auto c = ((nn1.infer(Dte) - Lte).array() == 0).count();
        auto t = Dte.rows();
        dtc::cout("[Accuracy at epoch ", i++, "]: ", c, "/", t, "=", c/static_cast<float>(t), '\n');
      });
  
  auto acc1 = ((nn1.infer(Dte) - Lte).array() == 0).count() / static_cast<float>(Lte.rows());

  REQUIRE(acc1 >= 0.9f);
  
  // Save the model.
  const auto model = std::filesystem::temp_directory_path() / "dnnc.mnist.model.dat";
  auto omsz = nn1.save(model);
  REQUIRE(omsz > 0);

  // Load the model
  dtc::ml::DnnClassifier nn2;
  auto imsz = nn2.load(model);
  REQUIRE(imsz == omsz);
  
  // Predict from the retrieved model
  auto acc2 = ((nn2.infer(Dte) - Lte).array() == 0).count() / static_cast<float>(Lte.rows());
  REQUIRE(acc1 == acc2);
}

// Testcase: DnnRegressorTest
TEST_CASE("RegressorTest.Dnn") {

	auto [X, Y] = regression_data(30, 5, 2);

  dtc::ml::DnnRegressor dnn;

  dnn.layer<dtc::ml::FullyConnectedLayer>(X.cols(), 30, dtc::ml::Activation::SIGMOID);
  dnn.layer<dtc::ml::BatchNormLayer>(30, 10, dtc::ml::Activation::SIGMOID);
  dnn.layer<dtc::ml::FullyConnectedLayer>(10, Y.cols());
  
  float pmse = (dnn.infer(X) - Y).array().square().sum() / (2.0f*X.rows());

  dnn.train(X, Y, 100, 15, 0.01f, [&, i=0] (dtc::ml::DnnRegressor& dnnr) mutable {
    printf("epoch %d: mse=%.4f\n", i++, (dnn.infer(X)-Y).array().square().sum() / (2.0f*X.rows()));
  });

  float cmse = (dnn.infer(X) - Y).array().square().sum() / (2.0f*X.rows());

  REQUIRE(cmse <= pmse);
}

// Testcase:: RnnRegressorNx1Test
TEST_CASE("RegressorTest.RnnRegressorNx1") {
  
  auto [X, Y] = regression_data(65536, 100, 4);

  dtc::ml::RnnRegressorNx1 rnn;

  rnn.cell(10, 30, 4, dtc::ml::Activation::RELU)
     .loss<dtc::ml::MeanSquaredError>();
  
  float pmse = (rnn.infer(X) - Y).array().square().sum() / (2.0f*X.rows());

  rnn.train(X, Y, 10, 64, 0.01f, [&, i=0] (dtc::ml::RnnRegressorNx1& rnn) mutable {
    //printf("epoch %d: mse=%.4f\n", i++, (rnn.infer(X) - Y).array().square().sum() / (2.0f * X.rows()));
  });

  float cmse = (rnn.infer(X) - Y).array().square().sum() / (2.0f*X.rows());
  
  REQUIRE(cmse <= pmse);
}

// Testcase: LinearRegressor
TEST_CASE("RegressorTest.Linear") {

  auto [X, Y] = regression_data(65536, 100, 4);

  dtc::ml::LinearRegressor lgr;
  
  lgr.dimension(100, 4);

  float pmse = (lgr.infer(X) - Y).array().square().sum() / (2.0f*X.rows());

  lgr.train(X, Y, 5, 64, 0.01f, [&, i=0] (dtc::ml::LinearRegressor& lgr) mutable {
    //printf("epoch %d: %.4f\n", i++, (lgr.infer(X)-Y).array().square().sum() / (2.0f*X.rows()));
  });

  float cmse = (lgr.infer(X) - Y).array().square().sum() / (2.0f*X.rows());

  REQUIRE(cmse <= pmse);
}






