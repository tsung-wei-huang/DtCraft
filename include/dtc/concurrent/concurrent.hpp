/******************************************************************************
 *                                                                            *
 * Copyright (c) 2016, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_CONCURRENT_CONCURRENT_HPP_
#define DTC_CONCURRENT_CONCURRENT_HPP_

#include <dtc/concurrent/mutex.hpp>
#include <dtc/concurrent/queue.hpp>

namespace dtc {

// Class: Concurrent
//
// Concurrent wraps any object and serializes all calls to the object in an asynchronous fashion.
// The execution order of inserted calls follows the FIFO order. The result of each call to the
// underlying object is accessed by the returned future object.
//
template <typename T>
class Concurrent {

  private:
  
    std::deque<std::unique_ptr<std::function<void()>>> _tasks;
    std::mutex _mutex;
    std::condition_variable _cond;

    T _object;
    bool _done;

    // Worker is initialized in the constructor list. Must wait until _done is initialized.
    std::thread _worker;

  public:
    
    template <typename... ArgsT>
    Concurrent(ArgsT&&... args) :
      _object {std::forward<ArgsT>(args)...},
      _done   {false},
      _worker {
        [&]() {
          typename decltype(_tasks)::value_type task;
          while(!_done) {
            { 
              std::unique_lock<std::mutex> lock(_mutex);
              while(_tasks.empty()) {
                _cond.wait(lock);
              }
              task = std::move(_tasks.front());
              _tasks.pop_front();
            } 
            (*task)();
          }
        }
      } {
    }
   
    ~Concurrent() {
      {
        std::lock_guard<std::mutex> lock(_mutex);
        _tasks.push_back(std::make_unique<std::function<void()>>(
          [&](){_done = true;}
        ));
      }
      _cond.notify_one();
      _worker.join();           
    }
   
    template <typename F, typename R = std::result_of_t<F(T&)>>
    std::future<R> operator()(F&& f) {

      std::packaged_task<R(T&)> task(std::forward<F>(f)); 
      auto fu = task.get_future();
      
      {
        std::lock_guard<std::mutex> lock(_mutex);
        _tasks.push_back(std::make_unique<std::function<void()>>(
          [this, task=StrongMovable<decltype(task)>(std::move(task))]() { 
            task.object(this->_object); 
          }
        ));
      }
      _cond.notify_one();
        
      return fu;
    }
};

};  // End of namespace dtc -----------------------------------------------------------------------


#endif

