#ifndef DTC_APP_DEBS18_FEEDER_HPP_
#define DTC_APP_DEBS18_FEEDER_HPP_

#include <dtc/dtc.hpp>

namespace dtc::cell {

// Class: Debs18StreamFeeder
//
// Infer send: a taskid and a string of ten fields
// Train send: a taskid and a string of two fields (correct)
// Verify get: a taskid and a string of two fields (predicted)
// 
// Each field is separated by a comma.
//
template <typename F>
class Debs18StreamFeeder {

  using R = add_optionality_t<std::decay_t<typename closure_traits<F>::result_type>>;

  struct Storage {
    
    mutable std::unordered_map<std::string, std::queue<std::string>> infers;
    mutable std::unordered_map<size_t, std::string> golden;
    
    size_t cursor    {0};
    size_t total     {0};
    size_t evaluated {0};
    double query1    {0};
    double query2    {0};
    
    Storage() = default;
    Storage(const Storage&);
  };

  private:
    
    Graph* const _graph {nullptr};

    VertexBuilder _vertex;
    
    key_type _in {-1};

    PlaceHolder _out;

    F _op;

    bool _verbose  {true};
    bool _feedback {false};

    void _send_infer(Vertex&, const std::string&) const;
    void _send_train(Vertex&, size_t) const;
    void _evaluate(Vertex&, size_t, const std::string&) const;

    std::string _make_infer(const std::string&) const;
    std::string _make_train(const std::string&) const;


  public:

    Debs18StreamFeeder(Graph*, auto&&, F&&);
    
    Debs18StreamFeeder(const Debs18StreamFeeder&) = delete;
    Debs18StreamFeeder(Debs18StreamFeeder&&) = delete;
    Debs18StreamFeeder& operator = (const Debs18StreamFeeder&) = delete;
    Debs18StreamFeeder& operator = (Debs18StreamFeeder&&) = delete;
    
    operator key_type() const;

    key_type in() const;

    PlaceHolder& out();
    
    Debs18StreamFeeder& in(auto&&);
    Debs18StreamFeeder& verbose(bool);
};

// Constructor
template <typename F>
Debs18StreamFeeder<F>::Storage::Storage(const Storage& rhs) :
  infers    {std::move(rhs.infers)}, 
  golden    {std::move(rhs.golden)},
  cursor    {rhs.cursor},
  total     {rhs.total},
  evaluated {rhs.evaluated},
  query1    {rhs.query1},
  query2    {rhs.query2} {
}

// Constructor
template <typename F>
Debs18StreamFeeder<F>::Debs18StreamFeeder(Graph* g, auto&& p, F&& op) : 
  _graph  {g},
  _vertex {_graph->vertex()},
  _out    {_vertex, {}},
  _op     {std::forward<F>(op)}
{
  _vertex.on(
    [this, p=std::forward<decltype(p)>(p)] (Vertex& v) { 
      
      auto& s = v.any.emplace<Storage>();

      std::ifstream ifs(p);

      if(!ifs.good()) {
        DTC_THROW("Failed to open ", p);
      }
      
      if(_verbose) {
        cout("Debs18 stream feeder from ", p, " ...\n");
      }

      std::string line;

      while(read_line(ifs, line)) {
        assert(std::count(line.begin(), line.end(), ',') == 11);
        s.infers[line.substr(0, line.find_first_of(','))].push(line);
        s.total++;
      }

      if(_verbose) {
        cout(
          "-------------------- statistics --------------------\n",
          "# bytes: ", std::filesystem::file_size(p), "\n",
          "# tests: ", s.total, "\n",
          "# ships: ", s.infers.size(), "\n",
          "----------------------------------------------------\n"
        );
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
template <typename F>
void Debs18StreamFeeder<F>::_evaluate(Vertex& v, size_t taskid, const std::string& record) const {
      
  auto& s = std::any_cast<Storage&>(v.any);

  if(s.golden.find(taskid) == s.golden.end()) return;
  
  // Increment the counter.
  s.evaluated++;

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
  auto p_tp = std::chrono::system_clock::from_time_t(std::mktime(&p_tm));
  auto g_tp = std::chrono::system_clock::from_time_t(std::mktime(&g_tm));
  auto dmin = std::abs(std::chrono::duration_cast<std::chrono::minutes>(p_tp - g_tp).count());
  s.query2 += dmin;
  
  // Output evaluation
  if(_verbose) {
    cout(
      "[evaluation_", taskid, "]: ",
      "PORT_ACCURACY(", p_port, " vs ", g_port, ")=", s.query1 / s.evaluated, " | "
      "AT_MAE(", p_time, " vs ", g_time, ")=", s.query2 / s.evaluated, "\n"
    );
  }
}

// Function: verbose
template <typename F>
Debs18StreamFeeder<F>& Debs18StreamFeeder<F>::verbose(bool flag) {
  _verbose = flag;
  return *this;
}

// Operator
template <typename F>
Debs18StreamFeeder<F>::operator key_type() const {
  return _vertex;
}

// Function: out
template <typename F>
PlaceHolder& Debs18StreamFeeder<F>::out() {
  return _out;
} 

// Function: in
template <typename F>
key_type Debs18StreamFeeder<F>::in() const {
  return _in;
}

// Function: _make_infer
template <typename F>
std::string Debs18StreamFeeder<F>::_make_infer(const std::string& record) const {
  auto comma1 = record.find_last_of(',');
  auto comma2 = record.find_last_of(',', comma1 - 1);
  return record.substr(0, comma2);
}

// Function: _make_train
template <typename F>
std::string Debs18StreamFeeder<F>::_make_train(const std::string& record) const {
  auto comma1 = record.find_last_of(',');
  auto comma2 = record.find_last_of(',', comma1 - 1);
  return record.substr(comma2 + 1);
}

// Function: _send_infer   
template <typename F>
void Debs18StreamFeeder<F>::_send_infer(Vertex& v, const std::string& shipid) const {
    
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

    auto taskid = s.cursor++;
    auto record = queue.front();
    
    if(queue.pop(); queue.empty()) {
      s.infers.erase(shipid);
    }

    s.golden[taskid] = record;

    auto infer_record = _make_infer(record);
    
    if(_verbose) {
      cout("[infer_task_", taskid, "]: ", infer_record, "\n");
    }

    if constexpr(std::is_same_v<void, R>) {
      _op(taskid, infer_record);
    }
    else {
      if(R dout = _op(taskid, infer_record); dout) {
        v.broadcast_to(_out.keys(), *dout);
      }
    }
  }
}
    
// Function: _send_train
template <typename F>
void Debs18StreamFeeder<F>::_send_train(Vertex& v, size_t taskid) const {
  
  auto& s = std::any_cast<Storage&>(v.any);
  
  // Send the data with correct label.
  if(auto node = s.golden.extract(taskid); node) {
    
    auto train_record = _make_train(node.mapped());
    
    if(_feedback) {

      if(_verbose) {
        cout("[train_task_", taskid, "]: ", train_record, "\n");
      }

      if constexpr(std::is_same_v<void, R>) {
        _op(taskid, train_record);
      }
      else {
        if(R dout = _op(taskid, train_record); dout) {
          v.broadcast_to(_out.keys(), *dout);
        }
      }
    }
    
    // Send the next infer data.
    _send_infer(v, node.mapped().substr(0, node.mapped().find_first_of(',')));
  }
}

// Function: in
template <typename F>
Debs18StreamFeeder<F>& Debs18StreamFeeder<F>::in(auto&& tail) {

  if(_in != -1) {
    DTC_THROW("Debs18StreamFeeder:in already connected");
  }
  
  _in = _graph->stream(tail, _vertex).on([this] (Vertex& v, InputStream& is) mutable { 

    auto& s = std::any_cast<Storage&>(v.any);

    std::tuple<size_t, std::string> result;

    while(is(result) != -1) {

      auto& [taskid, record] = result;
      
      // Calculate the accuracy
      _evaluate(v, taskid, record);
      
      // Proceed to the next data.
      _send_train(v, taskid);
    }
    
    // EOF
    if(s.golden.size() == 0) {

      assert(s.infers.size() == 0 && s.evaluated == s.total);

      if(_verbose) {
        cout(
          "-------------------- summary --------------------\n",
          "# tasks: ", s.total, "\n",
          "Query 1: ", s.query1 / s.total, "\n",
          "Query 2: ", s.query2 / s.total, "\n",
          "-------------------------------------------------\n"
        );
      }

      for(const auto& k : _out.keys()) {
        v.remove_ostream(k);
      }
      return Event::REMOVE;
    }

    return Event::DEFAULT;
  });
  
  return *this;
}

// Deduction guide
template <typename F>
Debs18StreamFeeder(Graph*, auto&&, F&&) -> Debs18StreamFeeder<F>;


};  // end of namespace dtc::cell -----------------------------------------------------------------


#endif
