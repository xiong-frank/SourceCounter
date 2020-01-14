
cd $(dirname "$0")

../../scripts/compile-script.sh "g++-9 --std=c++17" ar -O3 -pthread
