/******************************************************************************
 *                                                                            *
 * Copyright (c) 2018, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_UTILITY_ALLOCATOR_HPP_
#define DTC_UTILITY_ALLOCATOR_HPP_

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>  
#include <ostream>   
#include <new>       

namespace dtc {

// Class: SingularAllocator
// The memory pool based allocator of a single fixed data of type T.
// As an intermediate C++ programmer should know, the following code:
// 
//   T* t = new T(...);
//   delete t;
//
// is actually interpreted by the compiler as:
//
//   T* t = ::operator new (1)
//   t->T::T(...)
//   if(t != nullptr) {
//     ::operator delete (a)
//   }
//
template <typename T, size_t Cap = 8>
class SingularAllocator {

  static_assert(Cap >= 1, "Failed to compile SingularAllocator (capacity should be at least one)");
  
  struct MemBlock{
    T* block;                                               // Block memory.
    size_t size;                                            // Size of the block (count).
    struct MemBlock* next;                                  // Pointer to the next block.
  };

  struct Mempool{
    MemBlock* head;                                         // Head pool block.
    MemBlock* tail;                                         // Tail pool block.
    T** free_entries;                                       // Pointer array for recycle items.
    size_t num_free_entries;                                // Size of recycle box.
    size_t free_entry_cursor;                               // Cursor to next item from recycle.
    size_t pool_block_cursor;                               // Cursor to next item from block.
  };

  public:
    
    inline SingularAllocator();                             // Constructor.
    inline ~SingularAllocator();                            // Destructor.

    inline T* allocate(const size_t n=1) ;                  // Allocate an entry of type T.
    inline void deallocate(T*, const size_t n=1);           // Deallocate an entry of type T.
    
    template <typename... ArgsT>               
    inline void construct(T*, ArgsT&&...);                  // Construct an item.
    inline void destroy(T*);                                // Destroy an item.

  private:

    SingularAllocator & operator = (const SingularAllocator &) = delete;    // Disable copy assignment.

    Mempool _slot;                                          // Slot (meta data storage).

    inline MemBlock* _allocate_memblock(const size_t);      // Allocate a pool block.
    inline void _deallocate_memblock(MemBlock*);            // Deallocate a pool block.
};

// Constructor.
template <typename T, size_t Cap>
inline SingularAllocator<T, Cap>::SingularAllocator() {

  // Create the first memory pool block. By default, we create at least "n" items
  // for the first pool block.
  MemBlock* first_pool = _allocate_memblock(Cap);
  
  // Assign the meta-data to the slot.
  _slot.head = first_pool;
  _slot.tail = first_pool;
  _slot.num_free_entries = Cap;
  _slot.free_entries = static_cast<T**>(malloc(Cap*sizeof(T*)));
  _slot.free_entry_cursor = 0;
  _slot.pool_block_cursor = 0;
}

// Destructor.
template <typename T, size_t Cap>
inline SingularAllocator<T, Cap>::~SingularAllocator() {

  // Iterate the linked list and free each pool block.
  MemBlock* pre = nullptr;
  MemBlock* cur = _slot.head; 
  while(cur != nullptr) {
    pre = cur;
    cur = cur->next;
    _deallocate_memblock(pre);
  }

  // Free the memory of slot.
  free(_slot.free_entries);
}

// Function: _allocate_memblock
template <typename T, size_t Cap>
inline typename SingularAllocator<T, Cap>::MemBlock* SingularAllocator<T, Cap>::_allocate_memblock(const size_t n) {
  MemBlock* ptr = static_cast<MemBlock*>(malloc(sizeof(MemBlock)));
  ptr->block = static_cast<T*>(malloc(n*sizeof(T)));
  ptr->size = n;
  ptr->next = nullptr;
  return ptr;
}

// Procedure: _deallocate_memblock
template <typename T, size_t Cap>
inline void SingularAllocator<T, Cap>::_deallocate_memblock(MemBlock* ptr) {
  free(ptr->block);
  free(ptr);
}

// Procedure: construct
// Construct an item with placement new.
template <typename T, size_t Cap>
template <typename... ArgsT>
inline void SingularAllocator<T, Cap>::construct(T* ptr, ArgsT&&... args) {
  new (ptr) T(std::forward<ArgsT>(args)...); 
}

// Procedure: destroy
// Destroy an item
template <typename T, size_t Cap>
inline void SingularAllocator<T, Cap>::destroy(T* ptr) {
  ptr->~T();
}

// Function: allocate
// Allocate a memory piece of type T from the memory pool and return the T* to that memory.
template <typename T, size_t Cap>
inline T* SingularAllocator<T, Cap>::allocate(const size_t n) {

  assert(n == 1);

  // Allocate item from free entry if any.
  if(_slot.free_entry_cursor) {
    return _slot.free_entries[--_slot.free_entry_cursor];
  }
  // Allocate item from the block, where we have to create a new pool block if it is full.
  else {
    if(_slot.pool_block_cursor == _slot.tail->size) {
      MemBlock* ptr = _allocate_memblock((_slot.tail->size) << 1);
      _slot.tail->next = ptr;
      _slot.tail = ptr;
      _slot.pool_block_cursor = 0;
    }
    return &(_slot.tail->block[_slot.pool_block_cursor++]);
  }
}

// Function: deallocate
// Deallocate given memory piece of type T.
template <typename T, size_t Cap>
inline void SingularAllocator<T, Cap>::deallocate(T* ptr, const size_t n) {

  assert(n == 1);
  
  // Reallocate the free entry array if it is full.
  if(_slot.free_entry_cursor == _slot.num_free_entries) {
    _slot.num_free_entries = _slot.num_free_entries << 1;
    _slot.free_entries = static_cast<T**>(realloc(_slot.free_entries, sizeof(T*)*_slot.num_free_entries));
  }

  // Insert the deallocated item to the end of the free entry array.
  _slot.free_entries[_slot.free_entry_cursor++] = ptr;
}


}; // End of namespace dtc. ---------------------------------------------------------------

#endif







