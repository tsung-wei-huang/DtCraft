// Program: mnist
// Authors: Tsung-Wei Huang
// Date: 2018/02/15

#include <dtc/dtc.hpp>

constexpr auto mnist_image_file = DTC_HOME "/benchmark/mnist/train-images.idx3-ubyte";
constexpr auto mnist_label_file = DTC_HOME "/benchmark/mnist/train-labels.idx1-ubyte";

// Procedure: offline
void offline() {

  dtc::Graph G;

  G.vertex().on([] (dtc::Vertex& v) {

    Eigen::MatrixXf images = dtc::ml::read_mnist_image<Eigen::MatrixXf>(mnist_image_file) / 255.0;
    Eigen::VectorXi labels = dtc::ml::read_mnist_label<Eigen::VectorXi>(mnist_label_file);

    assert(images.rows() == labels.rows());

    const int N = images.rows();
    const int num_infers = std::max(10000, N/10);
    const int num_trains = N - num_infers;

    auto tp_beg = std::chrono::steady_clock::now();

    dtc::cerr(
      "-------------------- Mnist-offline trainer --------------------\n",
      "Image: ", mnist_image_file, '\n',
      "Label: ", mnist_label_file, '\n',
      "# images: ", N, '\n',
      "# infers: ", num_infers, '\n',
      "# trains: ", num_trains, '\n'
    ).flush();

    Eigen::MatrixXf Dtr = images.middleRows(0, num_trains);
    Eigen::MatrixXf Dte = images.middleRows(num_trains, num_infers);

    Eigen::VectorXi Ltr = labels.middleRows(0, num_trains);
    Eigen::VectorXi Lte = labels.middleRows(num_trains, num_infers);
    
    dtc::ml::DnnClassifier nn;

    nn.fully_connected_layer(784, 30, dtc::ml::Activation::RELU)
      .fully_connected_layer(30, 10, dtc::ml::Activation::NONE)
      .optimizer<dtc::ml::AdamOptimizer>()
      .train(Dtr, Ltr, 30, 64, 0.01f, [&, i=0] (auto& dnnc) mutable {
         Eigen::VectorXi label = dnnc.infer(Dte);
         auto c = ((label - Lte).array() == 0).count();
         auto t = Dte.rows();
         dtc::cout("[Accuracy at epoch ", i++, "]: ", c, "/", t, "=", c/static_cast<float>(t), '\n').flush();
       });
    
    auto tp_end = std::chrono::steady_clock::now();

    dtc::cout(
      "---------------------------------------------------------------\n",
      "Elapsed time: ", std::chrono::duration_cast<std::chrono::seconds>(tp_end - tp_beg).count(), "s\n"
    ); 

  });

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

  auto dnn = G.insert<dtc::cell::Visitor2x1>(
    [] (dtc::ml::DnnClassifier& c) {
      dtc::cout("Creating a dnn classifier [784x30x10]\n").flush();
      c.fully_connected_layer(784, 30, dtc::ml::Activation::RELU)
       .fully_connected_layer(30, 10, dtc::ml::Activation::NONE)
       .optimizer<dtc::ml::AdamOptimizer>();
    },
    [i=0] (dtc::ml::DnnClassifier& c, std::tuple<Eigen::MatrixXf, Eigen::VectorXi>& data) mutable {
      auto& [images, labels] = data;
      dtc::cout(
        "Accuracy at training cycle ", i++, " [", images.rows(), " images]: ",
        ((labels-c.infer(images)).array() == 0).count() / static_cast<float>(images.rows()), '\n'
      ).flush();
      c.train(images, labels, 10, 64, 0.01f, [](){});
    },
    [] (dtc::ml::DnnClassifier& c, int) {

    }
  );
  
  dnn.in1(src.out());

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









