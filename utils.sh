#!/bin/bash

initialMetArray=(0.1 0.2)
initialAdoMetArray=(0.3 0.4)
initialAdoHcyArray=(0.5 0.6)
initialHcyArray=(0.7 0.8)
metinArray=(0.9 1.0)

buildDir="$(pwd)/build"
datDir="$(pwd)/graphs/dat"
pngDir="$(pwd)/graphs/png"


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
  rm -rf bin/* # Clearing the bin directory
  make && make PREFIX="$(pwd)/bin" install

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
  "${buildDir}/./ims" --initialMet "$initialMet" --initialAdoMet "$initialAdoMet" --initialAdoHcy "$initialAdoHcy" --initialHcy "$initialHcy" --metin "$metin" --imagePath "$datDir/$datFileName"
}

createDataset() {
  if [ ! -d "$datDir" ]; then
    mkdir -p "$datDir"
  fi
  if [ ! -d "$pngDir" ]; then
    mkdir -p "$pngDir"
  fi

  runImsWithParams "50" "10" "5" "2" "50"

  echo "CREATING PNG FILES..."
  for datFile in "$datDir"/*.dat; do
    local pngFile="${datFile%.dat}.png"
    gnuplot -e "set terminal png; set output '$pngFile'; set title 'Lorenz Equation Output'; set xlabel 'Time'; set ylabel 'Values'; set datafile separator whitespace; plot '$datFile' using 1:2 with lines title 'Met', '$datFile' using 1:3 with lines title 'AdoMet', '$datFile' using 1:4 with lines title 'AdoHcy', '$datFile' using 1:5 with lines title 'Hcy'"
  done
  mv "$datDir"/*.png "$pngDir"
}

createArchive() {
  local archiveName=$1

  if [ -z "$archiveName" ]; then
    echo "Error: Tarball name not provided."
    return 1
  fi

  echo "Creating archive $archiveName.tar.gz..."
  tar -czvf "$archiveName.tar.gz" CMakeLists.txt utils.sh README.md src
}

main() {
  case $1 in
    build)
      buildSimlib
      buildProject
      ;;
    archive)
      createArchive "$2"
      ;;
    create_dataset)
      buildProject
      createDataset
      ;;
    *)
      echo "Usage: $0 [build|archive <archiveName>|create_dataset]"
      ;;
  esac
}


main "$@"
