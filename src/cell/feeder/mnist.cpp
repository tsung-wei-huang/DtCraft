#include <dtc/cell/feeder/mnist.hpp>

namespace dtc::cell {

// Constructor
MnistStreamFeeder::Storage::Storage(const std::filesystem::path& m, const std::filesystem::path& l) :
  images {ml::read_mnist_image<Eigen::MatrixXf>(m)},
  labels {ml::read_mnist_label<Eigen::VectorXi>(l)},
  cursor {0} {
  
  assert(images.rows() == labels.rows());
}

// Constructor
MnistStreamFeeder::Storage::Storage(const Storage& rhs) :
  images {std::move(rhs.images)},
  labels {std::move(rhs.labels)},
  cursor {rhs.cursor} {
}

// Constructor
MnistStreamFeeder::MnistStreamFeeder(Graph& g, std::filesystem::path m, std::filesystem::path l) : 
  _graph  {g},
  _vertex {_graph.vertex()},
  _in     {-1},
  _out    {_vertex, {}}
{
  _vertex.on(
    [this, m=std::move(m), l=std::move(l)] (Vertex& v) { 
      v.any.emplace<Storage>(m, l);
    }
  );
}

// Operator    
MnistStreamFeeder::operator key_type () const {
  return _vertex;
}

// Function: out
PlaceHolder& MnistStreamFeeder::out() {
  return _out;
} 

// Function: in
key_type MnistStreamFeeder::in() const {
  return _in;
}

// Procedure: _shuffle
void MnistStreamFeeder::_shuffle(Eigen::MatrixXf& D, Eigen::VectorXi& L) {
  Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> p(D.rows());
  p.setIdentity();
  std::shuffle(p.indices().data(), p.indices().data() + p.indices().size(), this_thread::random_engine());
  D = p * D;
  L = p * L;
}
    
// Function: _next_batch
Event::Signal MnistStreamFeeder::_next_batch(Vertex& v, InputStream& is) {

  int n;
  while(is(n) != -1) {

    if(n == -1) {
      for(const auto& okey : _out.keys()) {
        v.remove_ostream(okey);
      }
      return Event::REMOVE;
    }

    Storage& s = std::any_cast<Storage&>(v.any);
    
    // Adjust the cursor.
    if(s.cursor >= s.images.rows()) {
      _shuffle(s.images, s.labels);
      s.cursor = 0;
    }

    auto N = s.images.rows();
    auto b = s.cursor + n < N ? n : N - s.cursor;
  
    Eigen::MatrixXf m = s.images.middleRows(s.cursor, b);
    Eigen::VectorXi l = s.labels.middleRows(s.cursor, b);

    s.cursor += b;

    v.broadcast_to(_out.keys(), m, l);
  }
  return Event::DEFAULT;
}



};  // end of namespace dtc::cell. ----------------------------------------------------------------












