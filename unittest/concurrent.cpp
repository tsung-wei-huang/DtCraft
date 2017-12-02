/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#define CATCH_CONFIG_MAIN 

#include <cassert>
#include <dtc/utility.hpp>
#include <dtc/unittest/catch.hpp>
#include <dtc/concurrent/fifo.hpp>
#include <dtc/concurrent/threadpool.hpp>
#include <dtc/concurrent/mutex.hpp>

// ---- SpinLock ----------------------------------------------------------------------------------

// Test case: SpinLock
TEST_CASE("SpinLock") {

  int cnt {0};

  const auto W = std::thread::hardware_concurrency();
  const auto N = 65536;

  dtc::SpinLock L;

  std::vector<std::thread> workers;
  for(unsigned w=0; w<W; w++) {
    workers.emplace_back(
      [&] () {
        for(int n=0; n<N; ++n) {
          std::lock_guard<dtc::SpinLock> lock(L); 
          cnt++;
        }
      }
    );
  }

  for(auto& w : workers) w.join();

  REQUIRE(cnt == N*W);
}

// ---- ConcurrentFIFO ----------------------------------------------------------------------------

// Procedure test_concurrent_fifo
template <typename T>
auto test_concurrent_fifo() {

	const auto C = 512;
  const auto N = 65536;

  std::array<T, N> golden;
  std::array<T, N> result;

  // Create a concurrent FIFO
  dtc::ConcurrentFIFO<T, C + 1> fifo;

	REQUIRE(fifo.empty());
	REQUIRE(fifo.size() == 0);
  REQUIRE(fifo.capacity() == C);

  // Push one item.
  auto t = dtc::random<T>();
  REQUIRE(!fifo.pop(t));
  REQUIRE(fifo.push(t));
	REQUIRE(!fifo.empty());
  REQUIRE(fifo.size() == 1);
  REQUIRE(fifo.capacity() == C - 1);
  
  // Pop one item.
  auto s = dtc::random<T>();
  REQUIRE(fifo.pop(s));
  REQUIRE(s == t);
  REQUIRE(fifo.empty());
  REQUIRE(fifo.size() == 0);
  REQUIRE(fifo.capacity() == C);

  // Generate the golden data.
  for(auto& i : golden) i = dtc::random<T>();

  //// Assign tasks to producers
  std::thread producer(
    [&] () mutable {
      for(const auto& g : golden) {
        while(!fifo.push(g));
      }
    }
  );

  // Assign tasks to consumers
  std::thread consumer(
    [&] () mutable {
      T item;
      for(auto& r : result) {
        while(!fifo.pop(r));
      }
    }
  );

  producer.join();
  consumer.join();

  REQUIRE(fifo.size() == 0);
  REQUIRE(fifo.empty());
  REQUIRE(fifo.capacity() == C);
  REQUIRE(result == golden);
}

// Test case: ConcurrentFIFO.char
TEST_CASE("ConcurrentFIFO.char") {
	test_concurrent_fifo<char>();
}

// Test case: ConcurrentFIFO.int
TEST_CASE("ConcurrentFIFO.int") {
	test_concurrent_fifo<int>();
}

// Test case: ConcurrentFIFO.float
TEST_CASE("ConcurrentFIFO.float") {
	test_concurrent_fifo<float>();
}

// Test case: ConcurrentFIFO.double
TEST_CASE("ConcurrentFIFO.double") {
	test_concurrent_fifo<double>();
}

// Test case: ConcurrentFIFO.string
TEST_CASE("ConcurrentFIFO.string") {
	test_concurrent_fifo<std::string>();
}

// ---- ConcurrentQueue ---------------------------------------------------------------------------

// Procedure: test_concurrent_queue
template <typename T>
auto test_concurrent_queue(size_t num_producers, size_t num_consumers) {

  std::array<T, 1024> producer;
  std::array<T, 1024> consumer;

  std::atomic<size_t> p {0};
  std::atomic<size_t> c {0};

  std::vector<std::thread> producers;
  for(int i=0; i<num_producers; ++i) {
    producers.emplace_back(
      [&] () {
      }
    );
  }


  std::vector<std::thread> consumers;
  for(int i=0; i<num_consumers; ++i) {
    consumers.emplace_back(
      [&] () {
      }
    );
  }

}

// Test case: ConcurrentQueue
TEST_CASE("ConcurrentQueue") {

}

// ---- Threadpool --------------------------------------------------------------------------------

// Test case: SpawnShutdown
TEST_CASE("ThreadpoolTest.SpwanShutdown") {

  dtc::Threadpool threadpool;

  REQUIRE(threadpool.num_workers() == 0);
  REQUIRE(threadpool.num_tasks() == 0);
  REQUIRE(threadpool.master_id() == std::this_thread::get_id());

  const auto W = std::max(1u, std::thread::hardware_concurrency());

  for(auto i=0; i<256; ++i) {
    threadpool.spawn(W);
    REQUIRE(threadpool.num_workers() == W);

    threadpool.shutdown();
    REQUIRE(threadpool.num_workers() == 0);
  }
}

// Test case: Task
TEST_CASE("ThreadpoolTest.Task") {

  dtc::Threadpool threadpool;

  const auto W = std::max(1u, std::thread::hardware_concurrency());

  threadpool.spawn(W);
  REQUIRE(threadpool.num_workers() == W);

  const auto N = 65536;

  std::atomic<size_t> counter {0};

  for(auto n=0; n<N; n++) {
    threadpool.push_back([&] () { counter++; });
  }

  threadpool.shutdown();
  REQUIRE(threadpool.num_workers() == 0);
  REQUIRE(counter == N);
}




