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

// Testcase: DnnClassifierTest.Mnist
TEST_CASE("DnnClassifierTest.Mnist") {
  
  auto [Dtr, Ltr, Dte, Lte] = parse_mnist();
  
  dtc::ml::DnnClassifier nn1;

  nn1.fully_connected_layer(784, 30, dtc::ml::Activation::RELU)
     .fully_connected_layer(30, 10, dtc::ml::Activation::NONE)
     .train(Dtr, Ltr, 5, 64, 0.01f, [](){});
  
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

// Testcase: DnnRegressorTest.Mnist
TEST_CASE("DnnRegressorTest.Mnist") {

  auto [Dtr, Ltri, Dte, Ltei] = parse_mnist();

  Eigen::VectorXf Ltr = Ltri.cast<float>();
  Eigen::VectorXf Lte = Ltei.cast<float>();

  dtc::ml::DnnRegressor nn1;

  nn1.fully_connected_layer(784, 30, dtc::ml::Activation::RELU)
     .fully_connected_layer(30, 1, dtc::ml::Activation::NONE);
  
  float cmse = (nn1.infer(Dte) - Lte).array().square().sum() / (2.0f*Dte.rows());
  float pmse = cmse;

  nn1.train(Dtr, Ltr, 5, 64, 0.01f, [&, i=0] (dtc::ml::DnnRegressor& dnnr) mutable {
    cmse = (dnnr.infer(Dte) - Lte).array().square().sum() / (2.0f*Dte.rows());
    REQUIRE(cmse <= pmse);
    //printf("epoch %d: mse=%.4f\n", i++, cmse);
    pmse = cmse;
  });
}

// Testcase: LinearRegressorTest
TEST_CASE("LinearRegressorTest") {

	Eigen::MatrixXf X = Eigen::MatrixXf::Random(65536, 16);
  Eigen::VectorXf Y = Eigen::VectorXf::Random(65536);

  dtc::ml::LinearRegressor lgr;
  
  lgr.dimension(16);

  float pmse = (lgr.infer(X) - Y).array().square().sum() / (2.0f*X.rows());

  lgr.train(X, Y, 128, 64, 0.01f, [&, i=0] (dtc::ml::LinearRegressor& lgr) mutable {
    //printf("epoch %d: %.4f\n", i++, (lgr.infer(X)-Y).array().square().sum() / (2.0f*X.rows()));
  });

  float cmse = (lgr.infer(X) - Y).array().square().sum() / (2.0f*X.rows());

  REQUIRE(cmse <= pmse);
}






