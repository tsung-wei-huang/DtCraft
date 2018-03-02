/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang, Chun-Xun Lin and Martin D. F. Wong,   *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_STATGRAB_STATGRAB_HPP_
#define DTC_STATGRAB_STATGRAB_HPP_

#include <dtc/utility/singleton.hpp>
#include <dtc/headerdef.hpp>

namespace dtc {

class Statgrab : public EnableSingletonFromThis <Statgrab> {

  friend class EnableSingletonFromThis<Statgrab>;

  public:

  struct LoadInfo {
    double min1;
    double min5;
    double min15;
  };

    uintmax_t num_cpus() const;
    uintmax_t memory_limit_in_bytes() const;
    uintmax_t space_limit_in_bytes() const;

    LoadInfo cpu_load_avg() const;
};

/*// Class: Statgrab
class Statgrab : public EnableSingletonFromThis <Statgrab> {

  public:

  friend class EnableSingletonFromThis<Statgrab>;

  // Function: os_name, os_nodename, os_version, os_release, os_machine, 
  //           num_max_cpus, num_cpus
  //
  // Query the static information of the host, including OS info and cpu info. 
  //
  // @return: return the statistics.
  //
  inline void update_os_info();
  inline auto os_name();
  inline auto os_release();
  inline auto os_version();
  inline auto os_hostname();
  inline auto os_bitwidth();
  inline auto num_max_cpus();
  inline auto num_cpus();

  // Function: cpuloadavg_min1, cpuloadavg_min5, cpuloadavg_min15
  //
  // Query the cpuload information. Flushing the status must be called through update_cpuloadavg.
  //
  // @return: return the cpu load
  //
  inline void update_cpuloadavg();
  inline auto cpuloadavg_min1();
  inline auto cpuloadavg_min5();
  inline auto cpuloadavg_min15();

  std::tuple<double, double, double> loadavg() const noexcept;

  // Memory info.
  // 
  // Query the memory statistics. 
  // - total : the total memory in the system
  // - used  : all memory currently in use/reserved by running process and OS
  // - free  : total - used
  // - buffer: memory reserved by OS to alloc as buffers when process need them
  // - cached: recently used files being stored in ram
  //
  // used - (buffer + cached) is the amount of memory being actually used
  // free + (buffer + cached) is the amount of memory being actually free
  //
  inline void update_mem_info();
  inline auto mem_total();
  inline auto mem_free();
  inline auto mem_cached();
  inline auto mem_used();

  // Swap info.
  inline void update_swap_info();
  inline auto swap_total();
  inline auto swap_used();
  inline auto swap_free();

  // Page info.
  inline void update_page_info();
  inline void update_page_diff_info();
  inline auto num_pageins();
  inline auto num_pageouts();
  inline auto num_pageins_diff();
  inline auto num_pageouts_diff();

  // Update all info.
  inline void update_all();
  
  private:
  
  // Constructor/Destructor.
  inline Statgrab();
  inline ~Statgrab(); 
  
  // Thunk pointers which we use to access the system information.
  decltype(sg_get_host_info(nullptr)) _host_info;
  decltype(sg_get_load_stats(nullptr)) _load_info;
  decltype(sg_get_mem_stats(nullptr)) _mem_info;
  decltype(sg_get_swap_stats(nullptr)) _swap_info;
  decltype(sg_get_page_stats(nullptr)) _page_info;
  decltype(sg_get_page_stats_diff(nullptr)) _page_diff_info;
};

// Constructor.
inline Statgrab::Statgrab(){
  sg_init(1);
  update_all();
}

// Destructor
inline Statgrab::~Statgrab(){
  sg_shutdown();
}

//--------------------  OS information -------------------------------

// Function: os_name
inline auto Statgrab::os_name() {
  return _host_info->os_name;
}

// Function: os_release
inline auto Statgrab::os_release() {
  return _host_info->os_release;
}

// Function: os_version
inline auto Statgrab::os_version() {
  return _host_info->os_version;
}

// Function: hostname
inline auto Statgrab::os_hostname() {
  return _host_info->hostname;
}

// Function: bitwidth
inline auto Statgrab::os_bitwidth() {
  return _host_info->bitwidth;
}

// Function: num_cpus
inline auto Statgrab::num_cpus() {
  return _host_info->ncpus;
}

// Function: num_max_cpus
inline auto Statgrab::num_max_cpus() {
  return _host_info->maxcpus;
}

//-------------  CPU Average loading information --------------------

// Function: cpuloadavg_min1
inline auto Statgrab::cpuloadavg_min1(){
  return _load_info->min1;
}

// Function: cpuloadavg_min5
inline auto Statgrab::cpuloadavg_min5(){
  return _load_info->min5;
}

// Function: cpuloadavg_min15
inline auto Statgrab::cpuloadavg_min15(){
  return _load_info->min15;
}

//---------------- Memory information -------------------------------

// Function: mem_total
inline auto Statgrab::mem_total(){
  return _mem_info->total;
}

// Function: mem_free
inline auto Statgrab::mem_free(){
  return _mem_info->free;
}

// Function: mem_cached
inline auto Statgrab::mem_cached(){
  return _mem_info->cache;
}

// Function: mem_used
inline auto Statgrab::mem_used(){
  return _mem_info->used;
}

//---------------- SWAP information -------------------------------

// Function: swap_total
inline auto Statgrab::swap_total(){
  return _swap_info->total;
}

// Function: swap_used
inline auto Statgrab::swap_used(){
  return _swap_info->used;
}

// Function: swap_free
inline auto Statgrab::swap_free(){
  return _swap_info->free;
}

//---------------- Page information -------------------------------

// Function: num_pageins_diff
inline auto Statgrab::num_pageins_diff(){
  return _page_diff_info->pages_pagein;
}

// Function: num_pageouts_diff
inline auto Statgrab::num_pageouts_diff(){
  return _page_diff_info->pages_pageout;
}

// Function: num_pageins
inline auto Statgrab::num_pageins(){
  return _page_info->pages_pagein;
}

// Function: num_pageouts
inline auto Statgrab::num_pageouts(){
  return _page_info->pages_pageout;
}

//-----------------------------------------------------------------

// Procedure: _udate_all
inline void Statgrab::update_all() {
  update_os_info();
  update_cpuloadavg();
  update_mem_info();
  update_swap_info();
  update_page_info();
  update_page_diff_info();
}

// Procedure: update_os_info
inline void Statgrab::update_os_info() {
  _host_info = sg_get_host_info(nullptr);
}

// Procedure: update_cpuloadavg
// Update average CPU loading in 1 min, 5 mins, 15 mins
inline void Statgrab::update_cpuloadavg() {
  _load_info = sg_get_load_stats(nullptr);
}

// Procedure: update_mem_info
// Update the mem information.
inline void Statgrab::update_mem_info() {
  _mem_info = sg_get_mem_stats(nullptr);
}

// Procedure: update_swap_info
// Update the swap information.
inline void Statgrab::update_swap_info() {
  _swap_info = sg_get_swap_stats(nullptr);
}

// Procedure: update_page_info
// Update the page information.
inline void Statgrab::update_page_info() {
  _page_info = sg_get_page_stats(nullptr);
}

// Procedure: update_page_diff_info
// Update the page information.
inline void Statgrab::update_page_diff_info() {
  _page_diff_info = sg_get_page_stats_diff(nullptr);
}*/


};  // End of namespace dtc. --------------------------------------------------------------


#endif


