#include <app/debs18/debs18.hpp>

// Mapper to store the mapping.
struct Mapper {
  std::unordered_map<size_t, std::string> tid2sid;
  std::unordered_map<std::string, std::string> sid2time;
  std::unordered_map<std::string, std::string> sid2port;
};

// Procedure: debs18_map
void debs18_map(const std::string& file) {
  
  dtc::Graph G;
  
  auto debs18 = G.insert<dtc::cell::Debs18StreamFeeder>(
    file,
    [] (size_t& tid, std::string& record) -> std::tuple<size_t, std::string> {
      // Preprocessing here.
      return std::forward_as_tuple(tid, record);
    }
  );
  
  auto learner = G.insert<dtc::cell::Operator1x1>(
    [mapper=Mapper()] (std::tuple<size_t, std::string>& task) mutable -> std::optional<std::tuple<size_t, std::string>> {

      auto& [taskid, record] = task;
      
      if(auto num_commas = std::count(record.begin(), record.end(), ','); num_commas == 9) {
        auto sid = record.substr(0, record.find_first_of(','));
        mapper.tid2sid[taskid] = sid;
        return std::make_tuple(taskid, mapper.sid2time[sid] + ',' + mapper.sid2port[sid]);
      }
      else {
        dtc::cout("Invalid task_", taskid, ": ", record, '\n');
        assert(false);
      }
    }
  );

  learner.in(debs18.out());
  debs18.in(learner.out());

  dtc::Executor(G).run();

};


