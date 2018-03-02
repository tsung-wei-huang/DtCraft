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

#ifndef DTC_IPC_NOTIFIER_HPP_
#define DTC_IPC_NOTIFIER_HPP_

#include <sys/eventfd.h>
#include <dtc/device.hpp>

namespace dtc {

// Class: Notifier
class Notifier : public Device {
  
  public:
    
    template <typename... Ts>
    Notifier(Ts&&...);

    Notifier(Notifier&&) = delete;
    Notifier(const Notifier&) = delete;
    
    Notifier& operator = (Notifier&&) = delete;
    Notifier& operator = (const Notifier&) = delete;
  
    ~Notifier() = default;
};

// Constructor
template <typename... Ts>
Notifier::Notifier(Ts&&... ts) : Device {std::forward<Ts>(ts)...} {}

// Function: make_notifier
std::shared_ptr<Notifier> make_notifier(unsigned int = 0);

};  // end of namespace dtc. ----------------------------------------------------------------------

#endif
