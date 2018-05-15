// Program: mnist
// Authors: Tsung-Wei Huang
// Date: 2018/02/15

#include <dtc/dtc.hpp>

constexpr auto mnist_image_file = DTC_HOME "/benchmark/mnist/train-images.idx3-ubyte";
constexpr auto mnist_label_file = DTC_HOME "/benchmark/mnist/train-labels.idx1-ubyte";

// Procedure: offline
void offline() {
  
  using namespace std::literals;
  using namespace dtc::literals;

  dtc::Graph G;

  auto v = G.vertex().on([] (dtc::Vertex& v) {

    Eigen::MatrixXf images = dtc::ml::read_mnist_image<Eigen::MatrixXf>(mnist_image_file) / 255.0;
    Eigen::VectorXi labels = dtc::ml::read_mnist_label<Eigen::VectorXi>(mnist_label_file);

    assert(images.rows() == labels.rows());

    const int N = images.rows();
    const int num_infers = std::max(10000, N/10);
    const int num_trains = N - num_infers;

    Eigen::MatrixXf Dtr = images.middleRows(0, num_trains);
    Eigen::MatrixXf Dte = images.middleRows(num_trains, num_infers);

    Eigen::VectorXi Ltr = labels.middleRows(0, num_trains);
    Eigen::VectorXi Lte = labels.middleRows(num_trains, num_infers);
    
    dtc::ml::DnnClassifier nn;

    nn.layer<dtc::ml::FullyConnectedLayer>(784, 30, dtc::ml::Activation::RELU);
    nn.layer<dtc::ml::FullyConnectedLayer>(30, 10, dtc::ml::Activation::NONE);

    nn.train(Dtr, Ltr, 30, 64, 0.01f, [&, i=0] (auto& dnnc) mutable {
      auto c = ((dnnc.infer(Dte) - Lte).array() == 0).count();
      auto t = Dte.rows();
      printf("Accuracy at epoch %d: %f\n", i++, c/static_cast<float>(t));
    });
  });

  G.container().add(v).memory(256_MB);

  dtc::Executor(G).run();
}

// ------------------------------------------------------------------------------------------------

// Procedure: online
void online() {

  using namespace std::literals;
    
  dtc::Graph G;

  auto src = G.insert<dtc::cell::MnistStreamFeeder>(
    mnist_image_file,
    mnist_label_file,
    [] (Eigen::MatrixXf& images, Eigen::VectorXi& labels) {
      images /= 255.0f;
      return std::make_tuple(images, labels);
    }
  );

  src.duration(1ms).frequency(1000);

  auto dnn = G.vertex().on([] (dtc::Vertex& v) {
    auto& c = v.any.emplace<dtc::ml::DnnClassifier>(); 
    printf("CreatingDNN classifier [784x30x10] ...\n");
    c.layer<dtc::ml::FullyConnectedLayer>(784, 30, dtc::ml::Activation::RELU);
    c.layer<dtc::ml::FullyConnectedLayer>(30, 10);
  });

  G.stream(src.out(), dnn).on([n=0] (dtc::Vertex& v, dtc::InputStream& is) mutable {
    auto& c = std::any_cast<dtc::ml::DnnClassifier&>(v.any);
    Eigen::MatrixXf images;
    Eigen::VectorXi labels; 
    while(is(images, labels) != -1) {
      float accuracy = ((labels - c.infer(images)).array() == 0).count() / static_cast<float>(images.rows());
      printf("Accuracy at cycle %d: %f\n", n++, accuracy);
      c.train(images, labels, 1, 64, 0.01f, [] () {});
    }
    return dtc::Event::DEFAULT;
  });

  G.container().add(dnn);
  G.container().add(src);

  dtc::Executor(G).run();
}

// ------------------------------------------------------------------------------------------------

// Function: main
int main(int argc, char*argv[]) {

  if(argc != 2) {
    std::cout << "usage: ./mnist --[offline|online]\n";
    std::exit(EXIT_FAILURE);
  }

  if(std::string_view m = argv[1]; m == "--offline") {
    offline();
  }
  else if(m == "--online") {
    online();
  }
  else {
    std::cout << "invalid method: " << m << std::endl; 
    std::exit(EXIT_FAILURE);
  }

  return 0;
}









