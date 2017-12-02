/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

// TODO:
// 1. Enable the randome traversal over the selected file descriptor set.
// 2. Implement other demuxing mechanisms (epoll, kqueue, etc.).

#ifndef DTC_EVENT_DEMUX_HPP_
#define DTC_EVENT_DEMUX_HPP_

#include <dtc/event/event.hpp>

namespace dtc {

// Class: DemuxIX
// The gateway class for demultiplexer.
template <typename DemuxT>
class DemuxIX {
  
  friend class Reactor;
    
  protected:
    

  
  public:
  
    // Static polymorphism
    inline DemuxT* demux();
    inline const DemuxT* demux() const;
    
    // Gateway call.
    inline void insert(Event*);
    inline void remove(Event*);
    inline void freeze(Event*);
    inline void thaw(Event*);

    template <typename DurationT>
    inline void poll(DurationT&&);

    // Query
    inline bool is_valid(const Event*) const;

  private:

    // Default implementation.
    inline void _insert(Event*);
    inline void _remove(Event*);
    inline void _freeze(Event*);
    inline void _thaw(Event*);

    template <typename DurationT>
    inline void _poll(DurationT&&);
};

// Function: demux
template <typename DemuxT>
inline DemuxT* DemuxIX<DemuxT>::demux() {
  return static_cast <DemuxT*> (this);
}

// Function: demux
template <typename DemuxT>
inline const DemuxT* DemuxIX<DemuxT>::demux() const {
  return static_cast <const DemuxT*> (this);
}

// Function: is_valid
template <typename DemuxT>
inline bool DemuxIX<DemuxT>::is_valid(const Event* event) const {
  if(event == nullptr) return false;
  if(event->type != Event::READ && event->type != Event::WRITE) return false;
  if(!is_fd_valid(event->descriptor())) return false;
  return true;
}

// Procedure: insert
template <typename DemuxT>
inline void DemuxIX<DemuxT>::insert(Event* event) {
  if(!is_valid(event)) return;
  demux()->_insert(event);
}

// Procedure: remove
template <typename DemuxT>
inline void DemuxIX<DemuxT>::remove(Event* event) {
  if(!is_valid(event)) return;
  demux()->_remove(event);
}

// Procedure: freeze
template <typename DemuxT>
inline void DemuxIX<DemuxT>::freeze(Event* event) {
  if(!is_valid(event)) return;
  demux()->_freeze(event);
}

// Procedure: thaw
template <typename DemuxT>
inline void DemuxIX<DemuxT>::thaw(Event* event) {
  if(!is_valid(event)) return;
  demux()->_thaw(event);
}

// Procedure: poll
template <typename DemuxT>
template <typename DurationT>
inline void DemuxIX<DemuxT>::poll(DurationT&& d) {
  demux()->_poll(std::forward<DurationT>(d));
}

// Procedure: _insert
template <typename DemuxT>
inline void DemuxIX<DemuxT>::_insert(Event* event) {
  throw std::runtime_error("insert method is not implemented in Demux class");
}

// Procedure: _remove
template <typename DemuxT>
inline void DemuxIX<DemuxT>::_remove(Event* event) {
  throw std::runtime_error("remove method is not implemented in Demux class");
}

// Procedure: _freeze
template <typename DemuxT>
inline void DemuxIX<DemuxT>::_freeze(Event* event) {
  throw std::runtime_error("freeze method is not implemented in Demux class");
}

// Procedure: _thaw
template <typename DemuxT>
inline void DemuxIX<DemuxT>::_thaw(Event* event) {
  throw std::runtime_error("thaw method is not implemented in Demux class");
}

// Procedure: _poll
template <typename DemuxT>
template <typename DurationT>
inline void DemuxIX<DemuxT>::_poll(DurationT&& d) {
  throw std::runtime_error("poll method is not implemented in Demux class");
}


};  // End of namespace dtc. --------------------------------------------------------------

#endif






