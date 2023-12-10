#!/bin/bash

graphExt=pdf

buildDir="$(pwd)/build"
datDir="$(pwd)/graphs/dat"
pngDir="$(pwd)/graphs/${graphExt}"


buildSimlib() {
  local rootDir="$(pwd)"
  local simlibTar="simlib-3.09-20221108.tar.gz"
  local simlibUrl="https://www.fit.vutbr.cz/~peringer/SIMLIB/source/${simlibTar}"
  local simlibDir="simlib"

  echo "Creating third_party directory..."
  mkdir -p third_party && cd third_party || return

  if [ ! -d "$simlibDir" ]; then
    echo "Downloading simlib from $simlibUrl..."
    wget "$simlibUrl" && tar -xvf "$simlibTar"
  fi

  echo "Building simlib..."
  cd "$simlibDir" || return
  mkdir -p bin && make clean
  rm -rf bin/* # Clearing the bin directory just in case
  
  cd src && make clean && cd - && make PREFIX="$(pwd)/bin" install

  cd "$rootDir" || return
}

buildProject() {
  echo "Building the project..."
  rm -rf build && mkdir -p build && cd build || return
  cmake .. && make
}

runImsWithParams() {
  local initialMet="$1"
  local initialAdoMet="$2"
  local initialAdoHcy="$3"
  local initialHcy="$4"
  local metin="$5"

  local datFileName="inMet-${initialMet}_inAdoMet-${initialAdoMet}_inAdoHcy-${initialAdoHcy}_inHcy-${initialHcy}_Met-${metin}.dat"
  ${buildDir}/./ims --initialMet "$initialMet" --initialAdoMet "$initialAdoMet" --initialAdoHcy "$initialAdoHcy" --initialHcy "$initialHcy" --metinMax "$metin" --imagePath "$datDir/$datFileName"

  echo RUNNING COMMAND: "${buildDir}/./ims" --initialMet "$initialMet" --initialAdoMet "$initialAdoMet" --initialAdoHcy "$initialAdoHcy" --initialHcy "$initialHcy" --metinMax "$metin" --imagePath "$datDir/$datFileName"
}

createDataset() {
  if [ ! -d "$datDir" ]; then
    mkdir -p "$datDir"
  fi
  if [ ! -d "$pngDir" ]; then
    mkdir -p "$pngDir"
  fi

  #                 MET     AdoMet  AdoHcy   Hcy   MetinRate 5mthf
  runImsWithParams "0.05"   "1000"  "0.1"   "0.1"  "50"
  runImsWithParams "50"      "10"    "5"     "2"  "20"

  echo -e "\n"
  echo "CREATING PNG FILES from" "$datDir"/*.dat
  for datFile in "$datDir"/*.dat ; do
    local graphFile="${datFile%.dat}.${graphExt}"
    local graphFile2="${datFile%.dat}LLLL.${graphExt}"
    echo "CREATING GRAPH $graphFile..."
    gnuplot -e "set terminal pdf size 29.7cm, 84.1cm; \
                set output '${graphFile}'; \
                set multiplot layout 5,2 title 'Biology Data Overview'; \
                set datafile separator whitespace; \
                set lmargin 10; \
                set rmargin 10; \
                set tmargin 2; \
                set bmargin 2; \
                set title 'Met'; \
                set ylabel 'µM'; \
                plot '$datFile' using 1:2 with lines title 'Met'; \
                set title 'AdoMet'; \
                set ylabel 'µM'; \
                plot '$datFile' using 1:3 with lines title 'AdoMet'; \
                set title 'AdoHcy'; \
                set ylabel 'µM'; \
                plot '$datFile' using 1:4 with lines title 'AdoHcy'; \
                set title 'Hcy'; \
                set ylabel 'µM'; \
                plot '$datFile' using 1:5 with lines title 'Hcy'; \
                set title 'Metin'; \
                set ylabel 'µM'; \
                plot '$datFile' using 1:6 with lines title 'Metin'; \
                unset multiplot; \
                set output;"

    gnuplot -e "set terminal pdf size 40.7cm, 20.1cm; \
                set output '${graphFile2}'; \
                set multiplot layout 1,2 title 'Biology Data Overview'; \
                set datafile separator whitespace; \
                set lmargin 10; \
                set rmargin 10; \
                set tmargin 2; \
                set bmargin 2; \
                set title 'Met'; \
                set ylabel 'µM'; \
                plot '$datFile' using 1:2 with lines title 'Met'; \
                set title 'AdoMet'; \
                set ylabel 'µM'; \
                plot '$datFile' using 1:3 with lines title 'AdoMet'; \
                set title 'AdoHcy'; \
                set ylabel 'µM'; \
                set output;"
  done
  mv "$datDir"/*.$graphExt "$pngDir"
}

createArchive() {
  local archiveName=$1

  if [ -z "$archiveName" ]; then
    echo "Error: Tarball name not provided."
    return 1
  fi

  echo "Creating archive $archiveName.tar.gz..."
  tar -czvf "$archiveName.tar.gz" CMakeLists.txt utils.sh README.md src doc Makefile
}


main() {
  case $1 in
    run)
      createDataset
      ;;
    build)
      buildSimlib
      buildProject
      ;;
    archive)
      createArchive "$2"
      ;;
    rerun)
      buildProject
      createDataset
      ;;
    *)
      echo "Usage: $0 build|archive <archiveName>|run|rerun]"
      ;;
  esac
}


main "$@"
