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

#include <dtc/unittest/catch.hpp>
#include <dtc/dtc.hpp>

// TODO:
// 1. Use STL algorithm to simply codes
// 2. Hostinfo check?
// 3. Resource substraction.



//using key_type = dtc::key_type;
//
//class SchedulerTest : public testing::Test {
// protected:
//  virtual void SetUp() {
//    srand(time(NULL));
//    create_graph(100, 100);
//  }
//
//  virtual void TearDown() {
//  }
//
//	inline key_type gen_key(){
//		return (++_now_key);
//	}
//
//  bool verify(const std::vector<dtc::Bin>&, const std::vector<dtc::Packing>&, const dtc::pb::Topology);
//
//
//  void create_graph(size_t, size_t);
//	void create_bins(size_t, int);
//
//
//  dtc::pb::Topology graph{dtc::UUID(), 1}; 
//  std::vector<dtc::Bin> bins;
//
//  size_t _num_containers;
//  size_t _num_vertices;
//  size_t _num_bins;
//  key_type _now_key {1};
//};
//
//
//
//
//
//bool SchedulerTest::verify(const std::vector<dtc::Bin>& bins, 
//                           const std::vector<dtc::Packing>& packing, const dtc::pb::Topology graph){
//  // TODO
//  // If the scheduled number does not match the number of containers 
//  if( packing.size() != graph.containers.size() ){
//		std::cout << "Number of containers not match\n";
//    return false;
//	}
//
//  // Each packing has ONLY one container and the key should match one of the graph's containers
//  for( const auto &p : packing ){
//    if( p.topology.containers.size() != 1 || 
//        graph.containers.find(p.topology.containers.begin()->first) == std::end(graph.containers) ){
//			if( p.topology.containers.size() != 1 )
//    		std::cout << "More than one container " << p.topology.containers.size() << "\n";
//			else
//    		std::cout << "Key of containers and packing is not match\n";
//      return false;
//		}
//  }
//
//  // Check total number of vertices 
//  if( std::accumulate(std::begin(packing), std::end(packing), size_t(0) , 
//      [](size_t num, dtc::Packing p){ return num + p.topology.vertices.size();}) != graph.vertices.size() ){
//		std::cout << "Total number of vertices does not match\n";
//    return false;
//  }
//
//  // Each vertex should be ONLY in one bin
//  std::unordered_map<key_type,size_t> v2p;
//  for( size_t i = 0 ; i < packing.size() ; ++ i ){
//    for( const auto &v : packing[i].topology.vertices ){
//      // insert the key and check whether exists
//      bool has_insert = std::get<1>(v2p.emplace( v.first, i ));
//      if( !has_insert ){
//    		std::cout << "A vertex has been inserted more than once\n";
//        return false;
//			}
//    }
//  }
//
//	// All vertices should be assigned
//  for( auto &ekvp : graph.vertices ){
//    if( v2p.find(ekvp.first) == v2p.end() ){
//			std::cout << "A vertex has not been assigned\n";
//		  return false;
//		}
//	}
//
//  // Check streams in containers 
//  std::vector<size_t> stream_count(packing.size(), 0);
//  for( const auto &ekvp : graph.streams ){
//    if( packing[v2p[ekvp.second.tail]].topology.streams.find(ekvp.first) == 
//           std::end(packing[v2p[ekvp.second.tail]].topology.streams) || 
//        packing[v2p[ekvp.second.head]].topology.streams.find(ekvp.first) == 
//           std::end(packing[v2p[ekvp.second.head]].topology.streams)       
//      ){
//    	std::cout << "The stream is not in the packing\n";
//      return false;
//    }
//
//    ++ stream_count[v2p[ekvp.second.tail]];
//		if( v2p[ekvp.second.tail] != v2p[ekvp.second.head] )
//      ++ stream_count[v2p[ekvp.second.head]];
//  }
//
//	// Check the total number of streams
//  for( size_t i = 0 ; i < stream_count.size(); ++ i ){
//    if( stream_count[i] != packing[i].topology.streams.size() ){
//   		std::cout << "The stream count does not match\n";
//      return false;
//		}
//  }
//
//	dtc::pb::Resource zero = {0};
//  std::unordered_map<key_type, dtc::Bin>  bin_map;
//  for( size_t i = 0 ; i < bins.size() ; ++ i ) {
//    bin_map.insert( std::make_pair(bins[i].key, bins[i]) );
//	}
//
//	// Check streams hostnames are assigned
//	for( const auto &pvkp : packing ){
//  	for( const auto &evkp : pvkp.topology.streams ){
//      if( bin_map[pvkp.key].hostinfo.host != evkp.second.tail_host &&
//			    bin_map[pvkp.key].hostinfo.host != evkp.second.head_host	){
//				std::cout << "Stream hostname is not assigned\n";
//				return false;
//			}
//		}
//	}
//
//  for( size_t i = 0 ; i < packing.size() ; ++ i ){
//    // Check the key existence
//    if( bin_map.find(packing[i].key) == std::end(bin_map) ){
//    	std::cout << "The key does not exist\n";
//      return false;
//		}
//
//    // Check enough resource 
//    bin_map[packing[i].key].resource -= packing[i].topology.resource();
//
//    if( bin_map[packing[i].key].resource < zero ){
//     	std::cout << "Over use resources\n";
//      return false;
//		}
//  }
//
//  return true;
//}
//
//
//void SchedulerTest::create_graph(size_t num_vertices, size_t num_containers){
//  _num_vertices = num_vertices;
//	_num_containers = num_containers;
//	_now_key = 1;
//
//  graph.vertices.clear();
//	graph.streams.clear();
//	graph.containers.clear();
//
//  // Create containers 
//  std::vector<key_type> ckey;
//
//  for(size_t i = 0 ; i < _num_containers ; ++ i ){
//    dtc::pb::Topology::Container c(gen_key());
//    c.resource.num_cpus = 10;
//    c.resource.memory_limit_in_bytes = 1024;
//    graph.containers.insert( {c.key, c} );
//    ckey.emplace_back( c.key );
//  }
//
//  // Create vertices 
//  for( size_t i = 0 ; i < num_vertices ; ++ i ){
//    dtc::pb::Topology::Vertex v(gen_key());
//    v.container = ckey[ i%num_containers ];
//    graph.vertices.insert( {v.key, v} );
//  }
//
//  // Create directed streams
//  size_t num_streams = 0;
//  for( auto head = graph.vertices.begin(); head != graph.vertices.end(); ++ head ){
//    for( auto tail = std::next( head, 1); tail != graph.vertices.end(); ++ tail ){
//      //if( rand()%2 == 1 ){
//        ++ num_streams;
//        dtc::pb::Topology::Stream e(gen_key(), head->first, tail->first );
//        graph.streams.insert( {e.key, e} );
//      //}    
//    }
//  }
//  std::cout << "Total number of streams = " << num_streams << '\n';
//
//}
//
//
//
//
//
//
//void SchedulerTest::create_bins(size_t num_bins, int ratio ){
//	bins.clear();
//	_num_bins = num_bins;
//  // Create bins
//  for( size_t i = 0 ; i < _num_bins ; ++ i ){
//    bins.emplace_back( dtc::Bin{gen_key(), dtc::pb::HostInfo(), dtc::pb::Resource()} );
//    bins.back().resource.num_cpus = graph.containers[i].resource.num_cpus * ratio;
//    bins.back().resource.memory_limit_in_bytes = graph.containers[i].resource.memory_limit_in_bytes * ratio;
//  }
//}
//
//// Scheduler test.
//TEST_F(SchedulerTest, Functionality) {
//
//	for( int i = 0 ; i < 50 ; ++ i ){
//  	create_bins(i+1,10);
// 
//		dtc::BestFitBinPacking bfp( graph, bins );
//		auto packing = bfp();
//
//		if( packing.size() > 0 ){
//			EXPECT_EQ( verify(bins, packing, graph), true) << "A packing is not valid"; 
//		}
//		else{
//  		//std::cout << "No feasible scheduling \n";
//		}
//	}
//  
//}

