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

#include <dtc/dtc.hpp>

// k-means example
//
// Generate 4 centers out of a random point set in a square of unit 1000 
// Ideally, the output should locate at the center of each quadrant
//
// center 0: (500.844742, -501.310050)
// center 1: (-499.754750, -498.792104)
// center 2: (499.057231, 498.626793)
// center 3: (-500.640175, 501.107498)
//

constexpr size_t num_centers {4};
constexpr size_t num_points {2000000};
constexpr size_t num_slaves {2};

static_assert(num_slaves >= 1);
static_assert(num_points%num_slaves == 0);

struct Point{

  double x {dtc::random<double>(-1000.0, 1000.0)};
  double y {dtc::random<double>(-1000.0, 1000.0)};

  size_t nearest {0};
  
  // Include the data members to go through our stream interface.
  template <typename ArchiverT>
  std::streamsize archive( ArchiverT& ar ) { 
    return ar(x, y, nearest);
  } 
};


inline double square_distance(Point& p1, Point& p2){
  return (p1.x - p2.x)*(p1.x - p2.x)+ 
         (p1.y - p2.y)*(p1.y - p2.y);
}

inline double square_distance(std::vector<Point>& pts1, std::vector<Point>& pts2, const size_t num){
  double dis {.0};
  for(size_t i = 0; i < num; ++i){
    dis += square_distance(pts1[i], pts2[i]);
  }
  return dis;
}

// State enum to create iterative Map-Reduce control flow.
enum class State{
  WAIT_MAPPING,
  WAIT_CTS,
  WAIT_AVG
};

struct Result{
 std::vector<Point> cts;        // k centers
 std::vector<Point> next_cts;   // slave-private center storage (k*num_slaves)
 std::vector<size_t> mapping;   // num points mapped to a center.
 std::atomic<size_t> count {0};
 State state;

  Result() = default;

  Result(const Result& r) : 
    cts {r.cts},
    next_cts {r.next_cts},
    mapping {r.mapping},
    count {r.count.load()},
    state {r.state} {
  }
};

// Function: find_nearest
// Find the nearest center for each point.
auto find_nearest(std::vector<Point>& pts, std::vector<Point>& cts){
  std::vector<size_t> mapping(pts.size(), 0);
  for(size_t i = 0 ;i < pts.size(); ++i){
    auto dis = square_distance(pts[i], cts[0]);
    pts[i].nearest = 0;
    for( size_t j = 1 ; j < cts.size() ; ++j){
      if(auto tmp = square_distance(pts[i], cts[j]); tmp <= dis){
        dis = tmp;
        pts[i].nearest = j;
      }
    }
    ++mapping[pts[i].nearest];
  }
  return mapping;
}


int main(int argc, char* argv[]) {

  using namespace dtc::literals;
  using namespace std::literals;

  dtc::Graph G;


  auto A = G.vertex();
  // Cannot use array here, becuz B[i] = G.vertex() is deleted
  std::vector<dtc::VertexBuilder> B;
  std::vector<dtc::StreamBuilder> AtoB; 
  std::vector<dtc::StreamBuilder> BtoA; 
  for(size_t i = 0 ; i < num_slaves ; ++i){
    B.emplace_back(G.vertex());
    AtoB.emplace_back(G.stream(A, B.back()));
    BtoA.emplace_back(G.stream(B.back(), A));
  }

  A.on(
    [&AtoB](dtc::Vertex &v){
       // Randomly generate centers
       v.any = Result();
       auto &r = std::any_cast<Result&>(v.any);
       r.cts.resize(num_centers); 

       r.next_cts.resize(num_centers*AtoB.size());
       r.mapping.resize(num_centers*AtoB.size());
       r.state = State::WAIT_MAPPING;

       for(size_t i = 0 ; i < AtoB.size(); ++ i){
         v.ostream(AtoB[i])(r.cts);
       }
    }
  );

  // Each slave randomly generates a point subset.
  for(size_t i=0 ; i<num_slaves; ++i ){
    B[i].on(
      [](dtc::Vertex &v){
        v.any = std::vector<Point>(num_points/num_slaves);
      }
    );
  }
  
  // Create a stateful lambda for each iostream between the master and slaves.
  for( size_t i = 0 ; i < AtoB.size(); ++i){
    AtoB[i].on(
      [other=BtoA[i], state=State::WAIT_CTS] (dtc::Vertex& v, dtc::InputStream& is) mutable {
        if(state == State::WAIT_CTS){
          if(std::vector<Point> cts; is(cts) != -1){
            if(cts.size() == 0){
              v.ostream(other)(std::vector<size_t>());
              return dtc::Stream::CLOSE;
            }
            else{
              auto &pts = std::any_cast<std::vector<Point>&>(v.any);
              auto mapping = find_nearest(pts, cts); 
              state = State::WAIT_MAPPING;
              v.ostream(other)(mapping);
            }
          }
        }
        else{
          if(std::vector<size_t> mapping; is(mapping) != -1){
            auto &pts = std::any_cast<std::vector<Point>&>(v.any);
            std::vector<double> avg(2*num_centers, 0.0);
            for(const auto &pt : pts){
              avg[2*pt.nearest] += pt.x/(double)mapping[pt.nearest];       // Compute average x
              avg[2*pt.nearest+1] += pt.y/(double)mapping[pt.nearest];     // Compute average y
            }
            state = State::WAIT_CTS;
            v.ostream(other)(avg); 
          }
        }
        return dtc::Stream::DEFAULT;
      }
    );
  }
  
  // Create a stateful lambda for master vertex.
  for( size_t i = 0 ; i < BtoA.size(); ++i){
    BtoA[i].on(
      [&AtoB, eid=i] (dtc::Vertex& v, dtc::InputStream& is) mutable {
        auto &r = std::any_cast<Result&>(v.any);
        if(r.state == State::WAIT_MAPPING){
          if(std::vector<size_t> vec; is(vec) != -1) {
            if(vec.size() == 0){
              return dtc::Stream::CLOSE;
            }

            for(size_t i = 0 ;i < num_centers; ++i){
              r.mapping[eid*num_centers + i] = vec[i];
            }
            
            // Reduce the mapping and alter the state if receiving data from all slaves.
            if(++r.count == num_slaves ){
              for(size_t i = 1; i < AtoB.size(); ++i){
                for(size_t j = 0; j < num_centers; ++j){
                  r.mapping[j] += r.mapping[i*num_centers+j];
                }
              }

              r.count = 0;
              r.state = State::WAIT_AVG;
              for(auto &e: AtoB) {
                v.ostream(e)(r.mapping); 
              }
            }
          }
        }
        else{
          if(std::vector<double> vec; is(vec) != -1) {
            auto &r = std::any_cast<Result&>(v.any);

            for(size_t i = 0 ;i < num_centers; ++i){
              r.next_cts[eid*num_centers + i].x = vec[2*i];
              r.next_cts[eid*num_centers + i].y = vec[2*i+1];
            }

            // Reduce the center x/y values (average) to get the new centers for the next iteration.
            if(++r.count == num_slaves){
              for(size_t i = 1; i < num_slaves; ++i){
                for(size_t j = 0; j < num_centers; ++j){
                  r.next_cts[j].x += r.next_cts[i*num_centers+j].x;
                  r.next_cts[j].y += r.next_cts[i*num_centers+j].y;
                }
              }

              auto dis = square_distance(r.cts, r.next_cts, num_centers);
              r.state = State::WAIT_MAPPING;

              // Convergence check.
              if(dis < 0.01){
                for(size_t i=0; i<num_centers; ++i) {
                  printf("center %lu: (%lf, %lf)\n", i, r.next_cts[i].x, r.next_cts[i].y);
                }
                r.cts.clear();
              }
              else{
                for(size_t i = 0; i < num_centers; ++i){
                  r.cts[i] = r.next_cts[i];
                }
                r.count = 0;
              }

              for(auto &e: AtoB){
                v.ostream(e)(r.cts); 
              }
            }
          }         
        }
        return dtc::Stream::DEFAULT;
      }
    );
  }

  G.container().add(A).num_cpus(1);

  dtc::Executor(G).run(); 

  return 0;
};


