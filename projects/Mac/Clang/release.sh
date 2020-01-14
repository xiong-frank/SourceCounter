
cd $(dirname "$0")

../../scripts/compile-script.sh "clang++ --std=c++17" "llvm-ar" -O3 -pthread
