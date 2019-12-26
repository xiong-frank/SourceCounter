#!/bin/bash

source ~/.bash_profile

PROJECT_NAME=SourceCounter
PROJECT_DIR=../..
SOURCE_DIR=${PROJECT_DIR}/sources
OUTPUT_DIR=${PROJECT_DIR}/outputs

if [ ! -d ${OUTPUT_DIR}  ]; then
  mkdir ${OUTPUT_DIR}
fi

# g++ -c ${SOURCE_DIR}/config/LangRules.cpp -o ${OUTPUT_DIR}/LangRules.o --std=c++17
# g++ -c ${SOURCE_DIR}/config/Option.cpp --std=c++17
# g++ -c ${SOURCE_DIR}/counter/analyzer/Analyzer.cpp --std=c++17
# g++ -c ${SOURCE_DIR}/counter/Rapporteur.cpp --std=c++17
`g++` -c ../../sources/Main.cpp --std=c++17
