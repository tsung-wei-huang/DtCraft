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

#ifndef DTC_ML_LOSS_HPP_
#define DTC_ML_LOSS_HPP_

#include <dtc/headerdef.hpp>

namespace dtc::ml {

enum class Loss {
  MEAN_SQUARED_ERROR,
  MEAN_ABSOLUTE_ERROR,
  SOFTMAX_CROSS_ENTROPY
};

};  // end of namespace dtc::ml -------------------------------------------------------------------

#endif
