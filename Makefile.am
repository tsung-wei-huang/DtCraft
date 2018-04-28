# Automake options.
AUTOMAKE_OPTIONS = foreign

# Make sure that when we re-make ./configure, we get the macros we need.
ACLOCAL_AMFLAGS = -I m4 --install

# Automake directories.
SUBDIRS  =
SUBDIRS += .

# Package-related substitution variables.
CXX = @CXX@
CPPFLAGS = @CPPFLAGS@
CXXFLAGS = @CXXFLAGS@
LIBS = @LIBS@
TEST_LIBS = @TEST_LIBS@
DEFS = @DEFS@
CPU_COUNT = @CPU_COUNT@

# Initialize variables here so we can use += operator everywhere else.
lib_LTLIBRARIES =
noinst_LTLIBRARIES =
sbin_PROGRAMS =
bin_PROGRAMS =
noinst_PROGRAMS =
pkglibexec_PROGRAMS =
include_HEADERS =
pkginclude_HEADERS =
nobase_include_HEADERS =
nobase_pkginclude_HEADERS =
dist_bin_SCRIPTS =
dist_pkglibexec_SCRIPTS =
nobase_dist_pkgdata_DATA =
nodist_sbin_SCRIPTS =
check_PROGRAMS =
dist_check_SCRIPTS =
check_SCRIPTS =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
PHONY_TARGETS =
LDADD =
TESTS =

# Libraries
LIBS += -lstdc++fs
LIBS += $(PTHREAD_LIBS)

# CXX flags
CXXFLAGS += -std=c++1z
CXXFLAGS += -Wall
CXXFLAGS += $(PTHREAD_CFLAGS)

# DtCraft Package include
CPPFLAGS += -Iinclude
nobase_pkginclude_HEADERS += include/dtc/archive/binary.hpp
nobase_pkginclude_HEADERS += include/dtc/archive/archive.hpp
nobase_pkginclude_HEADERS += include/dtc/config.hpp.in
nobase_pkginclude_HEADERS += include/dtc/device.hpp
nobase_pkginclude_HEADERS += include/dtc/exit.hpp
nobase_pkginclude_HEADERS += include/dtc/statgrab/statgrab.hpp
nobase_pkginclude_HEADERS += include/dtc/error.hpp
nobase_pkginclude_HEADERS += include/dtc/csv/csv.hpp
nobase_pkginclude_HEADERS += include/dtc/traits.hpp
nobase_pkginclude_HEADERS += include/dtc/contrib/debs18/debs18.hpp
nobase_pkginclude_HEADERS += include/dtc/contrib/debs18/query2.hpp
nobase_pkginclude_HEADERS += include/dtc/contrib/debs18/feeder.hpp
nobase_pkginclude_HEADERS += include/dtc/contrib/debs18/query1.hpp
nobase_pkginclude_HEADERS += include/dtc/contrib/debs18/dataframe.hpp
nobase_pkginclude_HEADERS += include/dtc/cell/cell.hpp
nobase_pkginclude_HEADERS += include/dtc/cell/feeder/feeder.hpp
nobase_pkginclude_HEADERS += include/dtc/cell/feeder/mnist.hpp
nobase_pkginclude_HEADERS += include/dtc/cell/feeder/file.hpp
nobase_pkginclude_HEADERS += include/dtc/cell/feeder/csv.hpp
nobase_pkginclude_HEADERS += include/dtc/cell/operator.hpp
nobase_pkginclude_HEADERS += include/dtc/cell/visitor.hpp
nobase_pkginclude_HEADERS += include/dtc/event/reactor.hpp
nobase_pkginclude_HEADERS += include/dtc/event/select.hpp
nobase_pkginclude_HEADERS += include/dtc/event/demux.hpp
nobase_pkginclude_HEADERS += include/dtc/event/epoll.hpp
nobase_pkginclude_HEADERS += include/dtc/event/event.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/notifier.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/streambuf.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/domain.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/block_file.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/pipe.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/fifo.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/ipc.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/shm.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/ios.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/socket.hpp
nobase_pkginclude_HEADERS += include/dtc/headerdef.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/os.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/allocator.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/table.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/scope_guard.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/random.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/mempool.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/algorithm.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/http_parser.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/hash.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/lambda.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/literals.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/lexical_cast.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/singleton.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/json.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/utility.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/ios.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/chrono.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/uuid.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/logger.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/rnn.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/mnist.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/ml.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/activation.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/linear.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/loss.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/dnn.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/optimizer.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/naive_bayes.hpp
nobase_pkginclude_HEADERS += include/dtc/ml/layer.hpp
nobase_pkginclude_HEADERS += include/dtc/stamp-h1
nobase_pkginclude_HEADERS += include/dtc/config.hpp
nobase_pkginclude_HEADERS += include/dtc/config.hpp.in~
nobase_pkginclude_HEADERS += include/dtc/unittest/catch.hpp
nobase_pkginclude_HEADERS += include/dtc/static/threadpool.hpp
nobase_pkginclude_HEADERS += include/dtc/static/logger.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/shared_guard.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/synchronized.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/concurrent.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/deque.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/mutex.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/fifo.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/unique_guard.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/threadpool.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/queue.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/solution.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/common.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/loadinfo.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/killtask.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/taskinfo.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/protobuf.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/topology.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/resource.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/frontier.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/brokenio.hpp
nobase_pkginclude_HEADERS += include/dtc/dtc.hpp
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SVD/BDCSVD.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SVD/UpperBidiagonalization.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SVD/JacobiSVD.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SVD/JacobiSVD_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SVD/SVDBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseQR/SparseQR.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/OrthoMethods.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Homogeneous.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Rotation2D.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/arch/Geometry_SSE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/AngleAxis.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Quaternion.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/AlignedBox.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/EulerAngles.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/RotationBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Translation.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Umeyama.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Hyperplane.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Transform.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/ParametrizedLine.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Geometry/Scaling.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/OrderingMethods/Ordering.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/OrderingMethods/Amd.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/OrderingMethods/Eigen_Colamd.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_relax_snode.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_kernel_bmod.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_panel_dfs.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_Structs.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_panel_bmod.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLUImpl.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_column_dfs.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_Utils.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_gemm_kernel.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_column_bmod.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_copy_to_ucol.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_heap_relax_snode.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_SupernodalMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_pivotL.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_Memory.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseLU/SparseLU_pruneL.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SuperLUSupport/SuperLUSupport.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/plugins/MatrixCwiseUnaryOps.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/plugins/CommonCwiseUnaryOps.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/plugins/BlockMethods.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/plugins/MatrixCwiseBinaryOps.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/plugins/CommonCwiseBinaryOps.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/plugins/ArrayCwiseUnaryOps.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/plugins/ArrayCwiseBinaryOps.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCholesky/SimplicialCholesky_impl.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCholesky/SimplicialCholesky.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/IncompleteLUT.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/ConjugateGradient.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/IncompleteCholesky.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/SolveWithGuess.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/BiCGSTAB.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/BasicPreconditioners.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/LeastSquareConjugateGradient.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/IterativeLinearSolvers/IterativeSolverBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/PardisoSupport/PardisoSupport.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/LU/Determinant.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/LU/PartialPivLU.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/LU/FullPivLU.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/LU/arch/Inverse_SSE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/LU/PartialPivLU_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/LU/InverseImpl.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/CholmodSupport/CholmodSupport.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/StlSupport/StdDeque.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/StlSupport/StdList.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/StlSupport/details.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/StlSupport/StdVector.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Cholesky/LLT.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Cholesky/LDLT.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Cholesky/LLT_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Householder/HouseholderSequence.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Householder/BlockHouseholder.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Householder/Householder.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/UmfPackSupport/UmfPackSupport.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Jacobi/Jacobi.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/Tridiagonalization.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/HessenbergDecomposition.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/EigenSolver.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/SelfAdjointEigenSolver_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/SelfAdjointEigenSolver.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/GeneralizedSelfAdjointEigenSolver.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/ComplexSchur_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/RealQZ.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/MatrixBaseEigenvalues.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/GeneralizedEigenSolver.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/RealSchur.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/ComplexEigenSolver.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/RealSchur_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Eigenvalues/ComplexSchur.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/BooleanRedux.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/GeneralProduct.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/BandMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/ReturnByValue.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Swap.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Inverse.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/GenericPacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/ConditionEstimator.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CwiseTernaryOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/DenseCoeffsBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/MapBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Diagonal.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/SolveTriangular.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Matrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/DiagonalMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Transpose.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Select.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/NoAlias.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/PermutationMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Assign_MKL.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Assign.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CommaInitializer.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/AssignEvaluator.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/NestByValue.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/VectorwiseOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/MatrixBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Dot.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/CUDA/TypeCasting.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/CUDA/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/CUDA/Complex.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/CUDA/PacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/CUDA/Half.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/CUDA/PacketMathHalf.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/ZVector/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/ZVector/Complex.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/ZVector/PacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AVX512/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AVX512/PacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/Default/Settings.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AltiVec/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AltiVec/Complex.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AltiVec/PacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AVX/TypeCasting.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AVX/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AVX/Complex.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/AVX/PacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/SSE/TypeCasting.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/SSE/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/SSE/Complex.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/SSE/PacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/NEON/MathFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/NEON/Complex.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/arch/NEON/PacketMath.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/TriangularMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/EigenBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CoreEvaluators.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/SolverBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/VectorBlock.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/IO.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/GlobalFunctions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Random.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/DiagonalProduct.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Array.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Fuzzy.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/SelfCwiseBinaryOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/SelfadjointMatrixMatrix_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/GeneralBlockPanelKernel.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/GeneralMatrixMatrixTriangular_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/SelfadjointMatrixVector_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/TriangularSolverMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/SelfadjointProduct.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/GeneralMatrixMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/TriangularMatrixVector.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/SelfadjointMatrixMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/TriangularMatrixMatrix_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/GeneralMatrixVector.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/TriangularMatrixVector_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/GeneralMatrixMatrix_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/SelfadjointMatrixVector.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/TriangularSolverVector.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/Parallelizer.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/TriangularMatrixMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/SelfadjointRank2Update.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/TriangularSolverMatrix_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/GeneralMatrixMatrixTriangular.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/products/GeneralMatrixVector_BLAS.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/StableNorm.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Transpositions.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Visitor.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CoreIterators.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/DenseStorage.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/ForceAlignedAccess.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CwiseUnaryView.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/functors/NullaryFunctors.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/functors/TernaryFunctors.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/functors/AssignmentFunctors.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/functors/BinaryFunctors.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/functors/UnaryFunctors.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/functors/StlFunctors.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CwiseNullaryOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/PlainObjectBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Reverse.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CwiseUnaryOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Ref.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/CwiseBinaryOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Map.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Block.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/NumTraits.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/SelfAdjointView.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/BlasUtil.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/Macros.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/StaticAssert.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/XprHelper.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/ForwardDeclarations.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/Constants.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/Memory.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/NonMPL2.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/ReenableStupidWarnings.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/MKL_support.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/Meta.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/util/DisableStupidWarnings.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/ProductEvaluators.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Redux.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/ArrayBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Product.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/ArrayWrapper.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Solve.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/DenseBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/MathFunctionsImpl.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Stride.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/Core/Replicate.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/QR/CompleteOrthogonalDecomposition.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/QR/FullPivHouseholderQR.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/QR/ColPivHouseholderQR.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/QR/HouseholderQR_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/QR/HouseholderQR.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/QR/ColPivHouseholderQR_LAPACKE.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/misc/Kernel.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/misc/lapacke.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/misc/RealSvd2x2.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/misc/blas.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/misc/lapacke_mangling.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/misc/lapack.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/misc/Image.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseSolverBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseAssign.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseRef.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseRedux.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseFuzzy.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/MappedSparseMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseDiagonalProduct.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseTriangularView.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseDenseProduct.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseColEtree.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseMatrix.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseTranspose.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseCwiseUnaryOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseSparseProductWithPruning.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/ConservativeSparseSparseProduct.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseProduct.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/AmbiVector.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseCompressedBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseDot.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseSelfAdjointView.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparsePermutation.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseBlock.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseCwiseBinaryOp.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseMatrixBase.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseMap.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseUtil.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/TriangularSolver.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/CompressedStorage.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseView.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SparseCore/SparseVector.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/PaStiXSupport/PaStiXSupport.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/SPQRSupport/SuiteSparseQRSupport.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/src/MetisSupport/MetisSupport.h
nobase_pkginclude_HEADERS += include/dtc/Eigen/SVD
nobase_pkginclude_HEADERS += include/dtc/Eigen/SparseQR
nobase_pkginclude_HEADERS += include/dtc/Eigen/Geometry
nobase_pkginclude_HEADERS += include/dtc/Eigen/OrderingMethods
nobase_pkginclude_HEADERS += include/dtc/Eigen/SparseLU
nobase_pkginclude_HEADERS += include/dtc/Eigen/SuperLUSupport
nobase_pkginclude_HEADERS += include/dtc/Eigen/SparseCholesky
nobase_pkginclude_HEADERS += include/dtc/Eigen/Dense
nobase_pkginclude_HEADERS += include/dtc/Eigen/StdList
nobase_pkginclude_HEADERS += include/dtc/Eigen/IterativeLinearSolvers
nobase_pkginclude_HEADERS += include/dtc/Eigen/PardisoSupport
nobase_pkginclude_HEADERS += include/dtc/Eigen/LU
nobase_pkginclude_HEADERS += include/dtc/Eigen/CholmodSupport
nobase_pkginclude_HEADERS += include/dtc/Eigen/Cholesky
nobase_pkginclude_HEADERS += include/dtc/Eigen/Sparse
nobase_pkginclude_HEADERS += include/dtc/Eigen/StdDeque
nobase_pkginclude_HEADERS += include/dtc/Eigen/StdVector
nobase_pkginclude_HEADERS += include/dtc/Eigen/Householder
nobase_pkginclude_HEADERS += include/dtc/Eigen/UmfPackSupport
nobase_pkginclude_HEADERS += include/dtc/Eigen/Eigen
nobase_pkginclude_HEADERS += include/dtc/Eigen/Jacobi
nobase_pkginclude_HEADERS += include/dtc/Eigen/CMakeLists.txt
nobase_pkginclude_HEADERS += include/dtc/Eigen/Eigenvalues
nobase_pkginclude_HEADERS += include/dtc/Eigen/Core
nobase_pkginclude_HEADERS += include/dtc/Eigen/QR
nobase_pkginclude_HEADERS += include/dtc/Eigen/SparseCore
nobase_pkginclude_HEADERS += include/dtc/Eigen/PaStiXSupport
nobase_pkginclude_HEADERS += include/dtc/Eigen/SPQRSupport
nobase_pkginclude_HEADERS += include/dtc/Eigen/QtAlignedMalloc
nobase_pkginclude_HEADERS += include/dtc/Eigen/MetisSupport
nobase_pkginclude_HEADERS += include/dtc/json/json.hpp
nobase_pkginclude_HEADERS += include/dtc/macrodef.hpp
nobase_pkginclude_HEADERS += include/dtc/utility.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/executor.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/manager.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/vertex.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/graph.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/scheduler.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/master.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/container.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/agent.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/cgroup.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/stream.hpp
nobase_pkginclude_HEADERS += include/dtc/policy.hpp
nobase_pkginclude_HEADERS += include/dtc/webui/webui.hpp

# Library definition: lib/libDtCraft.la
lib_LTLIBRARIES           += lib/libDtCraft.la
lib_libDtCraft_la_SOURCES  =
lib_libDtCraft_la_SOURCES += src/device.cpp
lib_libDtCraft_la_SOURCES += src/statgrab/statgrab.cpp
lib_libDtCraft_la_SOURCES += src/csv/csv.cpp
lib_libDtCraft_la_SOURCES += src/contrib/debs18/feeder.cpp
lib_libDtCraft_la_SOURCES += src/contrib/debs18/query2.cpp
lib_libDtCraft_la_SOURCES += src/contrib/debs18/dataframe.cpp
lib_libDtCraft_la_SOURCES += src/exit.cpp
lib_libDtCraft_la_SOURCES += src/event/reactor.cpp
lib_libDtCraft_la_SOURCES += src/event/event.cpp
lib_libDtCraft_la_SOURCES += src/event/select.cpp
lib_libDtCraft_la_SOURCES += src/event/epoll.cpp
lib_libDtCraft_la_SOURCES += src/ipc/socket.cpp
lib_libDtCraft_la_SOURCES += src/ipc/block_file.cpp
lib_libDtCraft_la_SOURCES += src/ipc/ipc.cpp
lib_libDtCraft_la_SOURCES += src/ipc/domain.cpp
lib_libDtCraft_la_SOURCES += src/ipc/fifo.cpp
lib_libDtCraft_la_SOURCES += src/ipc/notifier.cpp
lib_libDtCraft_la_SOURCES += src/ipc/pipe.cpp
lib_libDtCraft_la_SOURCES += src/ipc/shm.cpp
lib_libDtCraft_la_SOURCES += src/ipc/streambuf.cpp
lib_libDtCraft_la_SOURCES += src/utility/os.cpp
lib_libDtCraft_la_SOURCES += src/utility/http_parser.cpp
lib_libDtCraft_la_SOURCES += src/utility/table.cpp
lib_libDtCraft_la_SOURCES += src/ml/loss.cpp
lib_libDtCraft_la_SOURCES += src/ml/rnn.cpp
lib_libDtCraft_la_SOURCES += src/ml/naive_bayes.cpp
lib_libDtCraft_la_SOURCES += src/ml/linear.cpp
lib_libDtCraft_la_SOURCES += src/ml/optimizer.cpp
lib_libDtCraft_la_SOURCES += src/ml/dnn.cpp
lib_libDtCraft_la_SOURCES += src/ml/layer.cpp
lib_libDtCraft_la_SOURCES += src/error.cpp
lib_libDtCraft_la_SOURCES += src/policy.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/loadinfo.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/resource.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/frontier.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/taskinfo.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/protobuf.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/solution.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/common.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/topology.cpp
lib_libDtCraft_la_SOURCES += src/kernel/container.cpp
lib_libDtCraft_la_SOURCES += src/kernel/cgroup.cpp
lib_libDtCraft_la_SOURCES += src/kernel/manager.cpp
lib_libDtCraft_la_SOURCES += src/kernel/stream.cpp
lib_libDtCraft_la_SOURCES += src/kernel/scheduler.cpp
lib_libDtCraft_la_SOURCES += src/kernel/vertex.cpp
lib_libDtCraft_la_SOURCES += src/kernel/graph.cpp
lib_libDtCraft_la_SOURCES += src/kernel/master.cpp
lib_libDtCraft_la_SOURCES += src/kernel/agent.cpp
lib_libDtCraft_la_SOURCES += src/kernel/executor.cpp
lib_libDtCraft_la_SOURCES += src/webui/webui.cpp

#### DtCraft binaries ####

# Program: main/master.cpp
noinst_PROGRAMS += bin/master
bin_master_SOURCES  = main/master.cpp
bin_master_LDADD    = lib/libDtCraft.la

# Program: main/agent.cpp
noinst_PROGRAMS += bin/agent
bin_agent_SOURCES  = main/agent.cpp
bin_agent_LDADD    = lib/libDtCraft.la

#### Examples ####

# Program: example/pi.cpp
noinst_PROGRAMS += example/pi
example_pi_SOURCES  = example/pi.cpp
example_pi_LDADD    = lib/libDtCraft.la

# Program: example/hello_world.cpp
noinst_PROGRAMS += example/hello_world
example_hello_world_SOURCES  = example/hello_world.cpp
example_hello_world_LDADD    = lib/libDtCraft.la

# Program: example/feeder.cpp
noinst_PROGRAMS += example/feeder
example_feeder_SOURCES  = example/feeder.cpp
example_feeder_LDADD    = lib/libDtCraft.la

# Program: example/prober.cpp
noinst_PROGRAMS += example/prober
example_prober_SOURCES  = example/prober.cpp
example_prober_LDADD    = lib/libDtCraft.la

# Program: example/kmeans.cpp
noinst_PROGRAMS += example/kmeans
example_kmeans_SOURCES  = example/kmeans.cpp
example_kmeans_LDADD    = lib/libDtCraft.la

# Program: example/prime.cpp
noinst_PROGRAMS += example/prime
example_prime_SOURCES  = example/prime.cpp
example_prime_LDADD    = lib/libDtCraft.la

# Program: example/operator.cpp
noinst_PROGRAMS += example/operator
example_operator_SOURCES  = example/operator.cpp
example_operator_LDADD    = lib/libDtCraft.la

# Program: example/external.cpp
noinst_PROGRAMS += example/external
example_external_SOURCES  = example/external.cpp
example_external_LDADD    = lib/libDtCraft.la

# Program: example/mnist.cpp
noinst_PROGRAMS += example/mnist
example_mnist_SOURCES  = example/mnist.cpp
example_mnist_LDADD    = lib/libDtCraft.la

# Program: example/reduce_sum.cpp
noinst_PROGRAMS += example/reduce_sum
example_reduce_sum_SOURCES  = example/reduce_sum.cpp
example_reduce_sum_LDADD    = lib/libDtCraft.la

#### App ####

# Application app/debs18/debs18
noinst_PROGRAMS += app/debs18/debs18
app_debs18_debs18_LDADD =
app_debs18_debs18_LDADD += lib/libDtCraft.la
app_debs18_debs18_SOURCES =
app_debs18_debs18_SOURCES += app/debs18/debs18.cpp

#### Unittest ####

# Program: unittest/archive
noinst_PROGRAMS += unittest/archive
unittest_archive_LDADD     = lib/libDtCraft.la
unittest_archive_LDADD    += $(TEST_LIBS)
unittest_archive_SOURCES   = unittest/archive.cpp
TESTS += unittest/archive.sh

# Program: unittest/statgrab
noinst_PROGRAMS += unittest/statgrab
unittest_statgrab_LDADD     = lib/libDtCraft.la
unittest_statgrab_LDADD    += $(TEST_LIBS)
unittest_statgrab_SOURCES   = unittest/statgrab.cpp
TESTS += unittest/statgrab.sh

# Program: unittest/webui
noinst_PROGRAMS += unittest/webui
unittest_webui_LDADD     = lib/libDtCraft.la
unittest_webui_LDADD    += $(TEST_LIBS)
unittest_webui_SOURCES   = unittest/webui.cpp
TESTS += unittest/webui.sh

# Program: unittest/ipc
noinst_PROGRAMS += unittest/ipc
unittest_ipc_LDADD     = lib/libDtCraft.la
unittest_ipc_LDADD    += $(TEST_LIBS)
unittest_ipc_SOURCES   = unittest/ipc.cpp
TESTS += unittest/ipc.sh

# Program: unittest/reactor
noinst_PROGRAMS += unittest/reactor
unittest_reactor_LDADD     = lib/libDtCraft.la
unittest_reactor_LDADD    += $(TEST_LIBS)
unittest_reactor_SOURCES   = unittest/reactor.cpp
TESTS += unittest/reactor.sh

# Program: unittest/utility
noinst_PROGRAMS += unittest/utility
unittest_utility_LDADD     = lib/libDtCraft.la
unittest_utility_LDADD    += $(TEST_LIBS)
unittest_utility_SOURCES   = unittest/utility.cpp
TESTS += unittest/utility.sh

# Program: unittest/traits
noinst_PROGRAMS += unittest/traits
unittest_traits_LDADD     = lib/libDtCraft.la
unittest_traits_LDADD    += $(TEST_LIBS)
unittest_traits_SOURCES   = unittest/traits.cpp
TESTS += unittest/traits.sh

# Program: unittest/scheduler
noinst_PROGRAMS += unittest/scheduler
unittest_scheduler_LDADD     = lib/libDtCraft.la
unittest_scheduler_LDADD    += $(TEST_LIBS)
unittest_scheduler_SOURCES   = unittest/scheduler.cpp
TESTS += unittest/scheduler.sh

# Program: unittest/ml
noinst_PROGRAMS += unittest/ml
unittest_ml_LDADD     = lib/libDtCraft.la
unittest_ml_LDADD    += $(TEST_LIBS)
unittest_ml_SOURCES   = unittest/ml.cpp
TESTS += unittest/ml.sh

# Program: unittest/concurrent
noinst_PROGRAMS += unittest/concurrent
unittest_concurrent_LDADD     = lib/libDtCraft.la
unittest_concurrent_LDADD    += $(TEST_LIBS)
unittest_concurrent_SOURCES   = unittest/concurrent.cpp
TESTS += unittest/concurrent.sh

# Add files to the distribution list
EXTRA_DIST += Makefile.sh
EXTRA_DIST += conf
EXTRA_DIST += sbin
EXTRA_DIST += webui
EXTRA_DIST += benchmark
EXTRA_DIST += README.md
EXTRA_DIST += LICENSE
EXTRA_DIST += logo.jpg

# Regression 
regression: all-am
	@for t in $(TESTS); do $$t -d yes; done

# Clean hook
clean-local:
	@find . -name ._\* -delete

# Project-specific targets
echo_CXX:
	@echo $(CXX)

echo_CPPFLAGS:
	@echo $(CPPFLAGS)

echo_CXXFLAGS:
	@echo $(CXXFLAGS)

echo_LIBS:
	@echo $(LIBS)

echo_TEST_LIBS:
	@echo $(TEST_LIBS)

echo_DEFS:
	@echo $(DEFS)

echo_bin_PROGRAMS:
	@echo $(bin_PROGRAMS)

echo_sbin_PROGRAMS:
	@echo $(sbin_PROGRAMS)

echo_check_PROGRAMS:
	@echo $(check_PROGRAMS)

echo_check_SCRIPTS:
	@echo $(check_SCRIPTS)

echo_lib_LTLIBRARIES:
	@echo $(lib_LTLIBRARIES)

echo_prefix:
	@echo $(prefix)

echo_builddir:
	@echo $(builddir)

echo_abs_builddir:
	@echo $(abs_builddir)

echo_top_builddir:
	@echo $(top_builddir)

echo_abs_top_builddir:
	@echo $(abs_top_builddir)

echo_top_build_prefix:
	@echo $(top_build_prefix)

echo_srcdir:
	@echo $(srcdir)

echo_abs_srcdir:
	@echo $(abs_srcdir)

echo_top_srcdir:
	@echo $(top_srcdir)

echo_abs_top_srcdir:
	@echo $(abs_top_srcdir)

echo_bindir:
	@echo $(bindir)

echo_sbindir:
	@echo $(sbindir)

echo_docdir:
	@echo $(docdir)

echo_exec_prefix:
	@echo $(exec_prefix)

echo_libdir:
	@echo $(libdir)

echo_pkglibdir:
	@echo $(pkglibdir)

echo_includedir:
	@echo $(includedir)

echo_pkgincludedir:
	@echo $(pkgincludedir)

echo_libexecdir:
	@echo $(libexecdir)

echo_datadir:
	@echo $(datadir)

echo_OMP_CFLAGS:
	@echo $(OPENMP_CFLAGS)

echo_OMP_CXXFLAGS:
	@echo $(OPENMP_CXXFLAGS)

echo_BOOST_CPPFLAGS:
	@echo $(BOOST_CPPFLAGS)

echo_PYTHON_CPPFLAGS:
	@echo $(PYTHON_CPPFLAGS)

echo_PYTHON_LDFLAGS:
	@echo $(PYTHON_LDFLAGS)

echo_CPU_COUNT:
	@echo $(CPU_COUNT)

