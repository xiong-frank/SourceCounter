COMPILE_CMD='clang++ --std=c++17'
ARCHIVE_CMD='llvm-ar'
PROJECT_DIR=../..
SOURCE_DIR=${PROJECT_DIR}/sources
OUTPUT_DIR=${PROJECT_DIR}/outputs

if [ ! -d ${OUTPUT_DIR} ]; then
  mkdir ${OUTPUT_DIR}
fi

 # mkdir ${OUTPUT_DIR}/sc-counter
PROJECT_NAME=SourceCounter

echo "--> compiling sc-counter"
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-counter/analyzer/Analyzer.cpp -o ${OUTPUT_DIR}/sc-counter/Analyzer.o -mmacosx-version-min=10.15
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-counter/RuleManager.cpp       -o ${OUTPUT_DIR}/sc-counter/RuleManager.o -mmacosx-version-min=10.15
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-counter/Counter.cpp           -o ${OUTPUT_DIR}/sc-counter/Counter.o -mmacosx-version-min=10.15
${ARCHIVE_CMD} rcs ${OUTPUT_DIR}/sc-counter/libsc-counter.a ${OUTPUT_DIR}/sc-counter/*.o 

echo "--> compiling sc-console"
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-console/Rapporteur.cpp        -o ${OUTPUT_DIR}/sc-console/Rapporteur.o -mmacosx-version-min=10.15
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-console/Main.cpp              -o ${OUTPUT_DIR}/sc-console/Main.o -mmacosx-version-min=10.15
${COMPILE_CMD} ${OUTPUT_DIR}/sc-console/*.o -o ${OUTPUT_DIR}/sc-console/sc -L${OUTPUT_DIR}/sc-counter -lsc-counter

echo "--> compiling sc-test"
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-test/test_rule_mgr.cpp        -o ${OUTPUT_DIR}/sc-test/test_rule_mgr.o -mmacosx-version-min=10.15
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-test/test_analyzer.cpp        -o ${OUTPUT_DIR}/sc-test/test_analyzer.o -mmacosx-version-min=10.15
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-test/test_counter.cpp         -o ${OUTPUT_DIR}/sc-test/test_counter.o -mmacosx-version-min=10.15
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/sc-test/test_main.cpp            -o ${OUTPUT_DIR}/sc-test/test_main.o -mmacosx-version-min=10.15
${COMPILE_CMD} ${OUTPUT_DIR}/sc-test/*.o -o ${OUTPUT_DIR}/sc-test/sc-test -L${OUTPUT_DIR}/sc-counter -lsc-counter

