/******************************************************************************
 *                                                                            *
 * Copyright (c) 2015, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_COMMON_SINGLETON_HPP_
#define DTC_COMMON_SINGLETON_HPP_

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <memory>

namespace dtc {

// Class: EnableSingletonFromThis
// Guarantees that only a single instance of an object will exist throughout the
// lifetime of the program.
template <class Derived>
class EnableSingletonFromThis {
    
  public:

    inline static Derived& get(); 

  protected:
    
    EnableSingletonFromThis() = default; 
    ~EnableSingletonFromThis() = default;                                        

  private:
    
    //EnableSingletonFromthis(const EnableSingletonFromThis&) = delete;
    //EnableSingletonFromthis(EnableSingletonFromThis&&) = delete;
    EnableSingletonFromThis & operator = (const EnableSingletonFromThis&) = delete;      
    EnableSingletonFromThis & operator = (EnableSingletonFromThis&&) = delete;   
};

// Function: get
template <class Derived>
inline Derived& EnableSingletonFromThis<Derived>::get() {
  static Derived object;
  return object;
}

};  // End of dtc namespace. --------------------------------------------------------------

#endif


