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

#ifndef DTC_ML_MNIST_HPP_
#define DTC_ML_MNIST_HPP_

#include <dtc/headerdef.hpp>

namespace dtc::ml {

template <typename D>
auto read_mnist_image(const std::filesystem::path& path) {
  
  // Helper lambda.
	auto reverse_int = [] (int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i         & 255;
    c2 = (i >> 8)  & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
  };
  
  // Read the image.
  std::ifstream ifs(path, std::ios::binary);

  if(!ifs) {
    DTC_THROW("Failed to read mnist image file ", path);
  }

  int magic_number = 0;
  int num_imgs = 0;
  int num_rows = 0;
  int num_cols = 0;

  ifs.read((char*)&magic_number, sizeof(magic_number));
  magic_number = reverse_int(magic_number);

  ifs.read((char*)&num_imgs, sizeof(num_imgs));
  num_imgs = reverse_int(num_imgs);

  ifs.read((char*)&num_rows, sizeof(num_rows));
  num_rows = reverse_int(num_rows);

  ifs.read((char*)&num_cols, sizeof(num_cols));
  num_cols = reverse_int(num_cols);
  
  // Case 1: Eigen::MatrixXf or Eigen::MatrixXd
  if constexpr(std::is_same_v<D, Eigen::MatrixXf> || std::is_same_v<D, Eigen::MatrixXd>) {
    D images(num_imgs, num_rows*num_cols);
    for(int i = 0; i < num_imgs; ++i) {
      int j = 0;
      for(int r = 0; r < num_rows; ++r) {
        for(int c = 0; c < num_cols; ++c) {
          unsigned char p = 0;  // must use unsigned
          ifs.read((char*)&p, sizeof(p));
          images(i, j++) = static_cast<typename D::Scalar>(p);
        }
      }
    }
    return images;
  }
  // Case 2: vector
  else if constexpr(is_std_vector_v<D>) {
    using R = typename D::value_type;
    using V = typename R::value_type;
    D images(num_imgs);
    for(int i = 0; i < num_imgs; ++i) {
      int j = 0;
      images[i].resize(num_rows*num_cols);
      for(int r = 0; r < num_rows; ++r) {
        for(int c = 0; c < num_cols; ++c) {
          unsigned char p = 0;  // must use unsigned
          ifs.read((char*)&p, sizeof(p));
          images[i][j++] = static_cast<V>(p);
        }
      }
    }
    return images;
  }
  else static_assert(dependent_false_v<D>);
}

// Function: read_mnist_label
template <typename D>
auto read_mnist_label(const std::filesystem::path& path) {
  
  // Helper lambda.
	auto reverse_int = [](int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i         & 255;
    c2 = (i >> 8)  & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
  };
  
  // Read the image.
  std::ifstream ifs(path, std::ios::binary);
  
  if(!ifs) {
    DTC_THROW("Failed to read mnist image file ", path);
  }

  int magic_number = 0;
  int num_imgs = 0;

  ifs.read((char*)&magic_number, sizeof(magic_number));
  magic_number = reverse_int(magic_number);

  ifs.read((char*)&num_imgs, sizeof(num_imgs));
  num_imgs = reverse_int(num_imgs);
  
  // Case 1: Eigen
  if constexpr(is_eigen_matrix_v<D>) {
    D labels(num_imgs, 1);
    for (int i = 0; i<num_imgs; ++i) {
      unsigned char temp = 0;  // must use unsigned
      ifs.read((char*)&temp, sizeof(temp));
      labels(i) = static_cast<typename D::Scalar>(temp);
    }
    return labels;
  }
  // Case 2: vector
  else if constexpr(is_std_vector_v<D>) {
    D labels(num_imgs);
    for (int i = 0; i<num_imgs; ++i) {
      unsigned char temp = 0;  // must use unsigned
      ifs.read((char*)&temp, sizeof(temp));
      labels[i] = static_cast<typename D::value_type>(temp);
    }
    return labels;
  }
  else static_assert(dependent_false_v<D>);
}

//// Function: read_mnist
//template <typename D>
//auto read_mnist(auto&& m, auto&& l, bool f) {
//  D images = read_mnist_image<D>(std::forward<decltype(m)>(m), f);
//  D labels = read_mnist_label<D>(std::forward<decltype(l)>(l));
//  D data(images.rows(), images.cols() + labels.cols());
//  data << images, labels;
//  return data;
//}
//
//// Function: read_mnist
//template <typename D1, typename D2>
//auto read_mnist(auto&& m, auto&& l) {
//
//}

};  // end of namespace dtc::ml -------------------------------------------------------------------




#endif








