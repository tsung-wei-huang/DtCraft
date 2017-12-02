/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#include <dtc/event/event.hpp>

namespace dtc {

// Function: empty
bool TimeoutEventHeap::empty() const {
  return _array.empty();
}

// Function: top 
const Event* TimeoutEventHeap::top() const {
  return empty() ? nullptr : _array[0];
}

// Function: size
const size_t TimeoutEventHeap::size() const {
  return _array.size();
}

// Procedure: clear 
void TimeoutEventHeap::clear() {
  for(auto e : _array) {
    e->_descriptor = -1;
  }
  _array.clear();
}

// Procedure: remove   
void TimeoutEventHeap::remove(Event* e) {

  // Invalid removal.
  if(e->_descriptor == -1) return;

  // Event exists in the heap.
  auto last = _array.back();
  _array.pop_back();

  if(e->_descriptor && _less(last, _array[(e->_descriptor-1) >> 1])) {
    _bubble_up(e->_descriptor, last);
  }
  else {
    _bubble_down(e->_descriptor, last);
  }
  e->_descriptor = -1;
}

// Function: insert 
void TimeoutEventHeap::insert(Event* e) {
  if(e->_descriptor != -1) return;
  _array.push_back(e);
  _bubble_up(_array.size()-1, e);
}

// Function: pop 
Event* TimeoutEventHeap::pop() {
  if(_array.size()) {
    auto e = _array[0]; 
    auto last = _array.back();
    _array.pop_back();
    _bubble_down(0, last);
    e->_descriptor = -1;
    return e;
  }
  return nullptr;
}

// Function: _less
bool TimeoutEventHeap::_less(Event* l, Event* r) const {
  return l->_timeout < r->_timeout;
}

// Procedure: _bubble_up
void TimeoutEventHeap::_bubble_up(size_t idx, Event* e) {
  while(idx) {
    size_t parent = (idx - 1) >> 1;
    if(!_less(e, _array[parent])) {
      break;
    }
    (_array[idx] = _array[parent])->_descriptor = idx;
    idx = parent;
  }
  (_array[idx] = e)->_descriptor = idx;
}

// Procedure: _bubble_down 
void TimeoutEventHeap::_bubble_down(size_t idx, Event* e) {
  while(1) {
    size_t min_child = (idx + 1) << 1;
    if(min_child >= _array.size() || _less(_array[min_child-1], _array[min_child])) {
      --min_child;
    }
    if(min_child >= _array.size() || !_less(_array[min_child], e)) break;
    (_array[idx] = _array[min_child])->_descriptor = idx;
    idx = min_child;
  }
  (_array[idx] = e)->_descriptor = idx;
}



};  // End of namespace dtc. ----------------------------------------------------------------------
