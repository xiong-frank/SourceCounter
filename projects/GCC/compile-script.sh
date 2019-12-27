COMPILE_CMD='g++-9 --std=c++17'
PROJECT_NAME=SourceCounter
PROJECT_DIR=../..
SOURCE_DIR=${PROJECT_DIR}/sources
OUTPUT_DIR=${PROJECT_DIR}/output

if [ ! -d ${OUTPUT_DIR} ]; then
  mkdir ${OUTPUT_DIR}
fi

${COMPILE_CMD} $1 -c ${SOURCE_DIR}/config/LangRules.cpp          -o ${OUTPUT_DIR}/LangRules.o 
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/config/Option.cpp             -o ${OUTPUT_DIR}/Option.o 
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/counter/analyzer/Analyzer.cpp -o ${OUTPUT_DIR}/Analyzer.o 
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/counter/Rapporteur.cpp        -o ${OUTPUT_DIR}/Rapporteur.o 
${COMPILE_CMD} $1 -c ${SOURCE_DIR}/Main.cpp                      -o ${OUTPUT_DIR}/Main.o 

${COMPILE_CMD} ${OUTPUT_DIR}/*.o -o ${OUTPUT_DIR}/${PROJECT_NAME}
