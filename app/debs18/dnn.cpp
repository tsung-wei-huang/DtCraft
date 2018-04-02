#include <app/debs18/debs18.hpp>

// Procedure: debs18_dnn
void debs18_dnn(const std::string& file) {

  // TODO (Guaannan)
  // raining here
  dtc::Graph G;

  
  auto debs18 = G.insert<dtc::cell::Debs18StreamFeeder>(
    file,
    [] (size_t& tid, std::string& record) -> std::tuple<size_t, std::string> {
      // Preprocessing here.
      return std::forward_as_tuple(tid, record);
    }
  );
  
  auto learner = G.insert<dtc::cell::Operator1x1>(
    [] (std::tuple<size_t, std::string>& task) mutable -> std::optional<std::tuple<size_t, std::string>> {
      auto& [taskid, record] = task;
      dtc::LOGD("received task[", taskid, "]: ", record);
      // Inferring data (10 features).
      if(auto num_commas = std::count(record.begin(), record.end(), ','); num_commas == 9) {

        // TODO (Guannan)
        // Infer the predicted arrival time and port.
        return std::make_tuple(taskid, std::string{"14-03-15 15:19"} + ',' + "GIBRALTAR");
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
}
