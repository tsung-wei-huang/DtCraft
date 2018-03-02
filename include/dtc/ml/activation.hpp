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

#ifndef DTC_ML_ACTIVATION_HPP_
#define DTC_ML_ACTIVATION_HPP_

#include <dtc/headerdef.hpp>

namespace dtc::ml {

enum class Activation {
  SIGMOID,
  TANH,
  RELU,
  NONE
};

};  // end of namespace dtc::ml -------------------------------------------------------------------

#endif
