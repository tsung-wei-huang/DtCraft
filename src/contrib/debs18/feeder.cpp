#include <dtc/contrib/debs18/feeder.hpp>

namespace dtc::debs18 {

// Constructor
StreamFeeder::Storage::Storage(const Storage& rhs) :
  infers    {std::move(rhs.infers)}, 
  golden    {std::move(rhs.golden)},
  cursor    {rhs.cursor},
  total     {rhs.total},
  evaluated {rhs.evaluated},
  query1    {rhs.query1},
  query2    {rhs.query2} {
}

// Constructor
StreamFeeder::StreamFeeder(Graph* g, std::filesystem::path p) : 
  _graph  {g},
  _vertex {_graph->vertex()},
  _out    {_vertex, {}}
{
  _vertex.on(
    [this, p=std::move(p)] (Vertex& v) { 
      
      auto& s = v.any.emplace<Storage>();

      std::ifstream ifs(p);

      if(!ifs.good()) {
        DTC_THROW("Failed to open ", p);
      }
      
      if(_verbose) {
        std::cout << " stream feeder from " << p << " ...\n";
      }

      std::string line;

      while(read_line(ifs, line)) {
        assert(std::count(line.begin(), line.end(), ',') == 11);
        s.infers[line.substr(0, line.find_first_of(','))].push(line);
        s.total++;
      }

      if(_verbose) {
        std::cout << "-------------------- statistics --------------------\n"
                  << "# bytes: " << std::filesystem::file_size(p) << '\n' 
                  << "# tests: " << s.total << '\n'
                  << "# ships: " << s.infers.size() << "\n"
                  << "----------------------------------------------------\n";
      }

      // Send the data to the infer port.
      std::vector<std::string> shipids;
      for(const auto& kvp : s.infers) {
        shipids.push_back(kvp.first);
      }
      
      for(const auto& shipid : shipids) {
        _send_infer(v, shipid);
      }
    }
  );
}

// Procedure: evaluate
void StreamFeeder::_evaluate(Vertex& v, const std::string& taskid, const std::string& record) const {
      
  auto& s = std::any_cast<Storage&>(v.any);

  if(s.golden.find(taskid) == s.golden.end()) {
    LOGW("Wrong task id: ", taskid, "(ignored)");
    return;
  }

  std::string p_time, p_port, g_time, g_port;

  // Extract the prediction
  std::istringstream p_iss(record);
  std::getline(p_iss, p_time, ',');
  std::getline(p_iss, p_port);
  
  // Extract the golden
  auto comma1 = s.golden[taskid].find_last_of(',');
  auto comma2 = s.golden[taskid].find_last_of(',', comma1 - 1);
  std::istringstream g_iss(s.golden[taskid].substr(comma2 + 1));
  std::getline(g_iss, g_time, ',');
  std::getline(g_iss, g_port);

  // Calculate the Query 1 accuracy 
  if(g_port == p_port) {
    s.query1 += 1;
  }

  // Calculate the Query 2 accuracy
  std::tm p_tm = {}; 
  std::tm g_tm = {}; 
  ::strptime(p_time.c_str(), "%d-%m-%y %H:%M", &p_tm);
  ::strptime(g_time.c_str(), "%d-%m-%y %H:%M", &g_tm);
  auto p_tp = std::chrono::system_clock::from_time_t(::timegm(&p_tm));
  auto g_tp = std::chrono::system_clock::from_time_t(::timegm(&g_tm));
  auto dmin = std::abs(std::chrono::duration_cast<std::chrono::minutes>(p_tp - g_tp).count());
  s.query2 += dmin;
  
  // Increment the counter.
  s.evaluated++;
  
  // Output evaluation
  if(_verbose) {
    std::cout << "[evaluation_" << taskid << "]: "
              << "PORT_ACCURACY(" << p_port << " vs " << g_port << ")=" << s.query1 / s.evaluated << " | "
              << "AT_MAE(" << p_time << " vs " << g_time << ")=" << s.query2 / s.evaluated << "\n";
  }
  
  // Send the next infer
  _send_infer(v, s.golden[taskid].substr(0, s.golden[taskid].find_first_of(',')));
}

// Function: verbose
StreamFeeder& StreamFeeder::verbose(bool flag) {
  _verbose = flag;
  return *this;
}

// Operator
StreamFeeder::operator key_type() const {
  return _vertex;
}

// Function: out
PlaceHolder& StreamFeeder::out() {
  return _out;
} 

// Function: in
key_type StreamFeeder::in() const {
  return _in;
}

// Function: _make_infer
std::string StreamFeeder::_make_infer(const std::string& record) const {
  auto comma1 = record.find_last_of(',');
  auto comma2 = record.find_last_of(',', comma1 - 1);
  return record.substr(0, comma2);
}

// Function: _send_infer   
void StreamFeeder::_send_infer(Vertex& v, const std::string& shipid) const {
    
  auto& s = std::any_cast<Storage&>(v.any);

  // ShipID doesn't exist in the infer dataset.
  if(s.infers.find(shipid) == s.infers.end()) {
    return;
  }
  
  // The corresponding queue to this ship is empty.
  if(auto& queue = s.infers[shipid]; queue.empty()) {
    s.infers.erase(shipid);
  }
  // Pop out and send a new ship.
  else {

    auto taskid = std::to_string(s.cursor);
    auto record = queue.front();
    s.cursor++;
    
    if(queue.pop(); queue.empty()) {
      s.infers.erase(shipid);
    }

    s.golden[taskid] = record;

    auto infer_record = _make_infer(record);
    
    if(_verbose) {
      std::cout << "[infer_task_" << taskid << "]: " << infer_record << "\n";
    }

    v.broadcast_to(_out.keys(), taskid, infer_record);
  }
}
    




};  // end of namespace dtc::cell -------------------------------------------------------------------

