#!/bin/bash -e

PROJECT_DIR=../..
SOURCE_DIR=${PROJECT_DIR}/sources
OUTPUT_DIR=${PROJECT_DIR}/outputs

cd $(dirname "$0")

if [ ! -d ${OUTPUT_DIR} ]; then
  mkdir ${OUTPUT_DIR}
else
  rm -rf ${OUTPUT_DIR}/*
fi

echo "--> compiling sc-counter"
mkdir ${OUTPUT_DIR}/sc-counter
$1 $3 -c ${SOURCE_DIR}/sc-counter/analyzer/Analyzer.cpp -o ${OUTPUT_DIR}/sc-counter/Analyzer.o
$1 $3 -c ${SOURCE_DIR}/sc-counter/RuleManager.cpp       -o ${OUTPUT_DIR}/sc-counter/RuleManager.o
$1 $3 -c ${SOURCE_DIR}/sc-counter/Counter.cpp           -o ${OUTPUT_DIR}/sc-counter/Counter.o
$2 rcs ${OUTPUT_DIR}/sc-counter/libsc-counter.a ${OUTPUT_DIR}/sc-counter/*.o 

echo "--> compiling sc-console"
mkdir ${OUTPUT_DIR}/sc-console
$1 $3 -c ${SOURCE_DIR}/sc-console/Rapporteur.cpp        -o ${OUTPUT_DIR}/sc-console/Rapporteur.o
$1 $3 -c ${SOURCE_DIR}/sc-console/Main.cpp              -o ${OUTPUT_DIR}/sc-console/Main.o
$1 ${OUTPUT_DIR}/sc-console/*.o -o ${OUTPUT_DIR}/sc-console/sc $4 -L${OUTPUT_DIR}/sc-counter/ -lsc-counter

echo "--> compiling sc-test"
mkdir ${OUTPUT_DIR}/sc-test
$1 $3 -c ${SOURCE_DIR}/sc-test/test_rule_mgr.cpp        -o ${OUTPUT_DIR}/sc-test/test_rule_mgr.o
$1 $3 -c ${SOURCE_DIR}/sc-test/test_analyzer.cpp        -o ${OUTPUT_DIR}/sc-test/test_analyzer.o
$1 $3 -c ${SOURCE_DIR}/sc-test/test_counter.cpp         -o ${OUTPUT_DIR}/sc-test/test_counter.o
$1 $3 -c ${SOURCE_DIR}/sc-test/test_main.cpp            -o ${OUTPUT_DIR}/sc-test/test_main.o
$1 ${OUTPUT_DIR}/sc-test/*.o -o ${OUTPUT_DIR}/sc-test/sc-test $4 -L${OUTPUT_DIR}/sc-counter -lsc-counter

cd ${OUTPUT_DIR}
echo "--> build success! output: `pwd`"
