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
  local thf_5m="$6"

  local datFileName="inMet-${initialMet}_inAdoMet-${initialAdoMet}_inAdoHcy-${initialAdoHcy}_inHcy-${initialHcy}_Met-${metin}_Folat=${thf_5m}.dat"
  ${buildDir}/./ims --initialMet "$initialMet" --initialAdoMet "$initialAdoMet" --initialAdoHcy "$initialAdoHcy" --initialHcy "$initialHcy" --metinMax "$metin" --imagePath "$datDir/$datFileName" --thf_5m "$thf_5m"

  echo COMMAND: "${buildDir}/./ims" --initialMet "$initialMet" --initialAdoMet "$initialAdoMet" --initialAdoHcy "$initialAdoHcy" --initialHcy "$initialHcy" --metinMax "$metin" --imagePath "$datDir/$datFileName" --thf_5m "$thf_5m"
}

createDataset() {
  if [ ! -d "$datDir" ]; then
    mkdir -p "$datDir"
  fi
  if [ ! -d "$pngDir" ]; then
    mkdir -p "$pngDir"
  fi

  #                 MET     AdoMet  AdoHcy   Hcy   MetinRate 5mthf
#  runImsWithParams "0.5"   "0.1"    "1"     "1"     "10000" "23.3"
#  runImsWithParams "0.5"   "0.1"    "0.1"   "0.1"   "10000" "23.3"
#  runImsWithParams "00.5"   "00.1"  "00.1"  "00.1"  "10000" "23.3"

#  runImsWithParams "0.05"   "1000"  "1"    "1"     "5000"  "23.3" # don't touch this. this is correct
#  runImsWithParams "0.05"   "1000"  "0.1"  "0.1"   "5000" "2.3" # don't touch this. this is correct
#  runImsWithParams "0.05"   "1000"  "0.1"  "0.1"  "5000" "3.2" # don't touch this. this is correct

   runImsWithParams "0.05"   "1000"  "0.1"  "0.1"  "5000" "520000000" # don't touch this. this is correct
   runImsWithParams "0.05"   "1000"  "0.1"  "0.1"  "5000" "52" # don't touch this. this is correct

#   runImsWithParams "50"   "10"  "5"  "2"  "50" "5.2" # don't touch this. this is correct

#  runImsWithParams "50" "10" "5" "2" "50"

  echo "CREATING PNG FILES from" "$datDir"/*.dat
  for datFile in "$datDir"/*.dat ; do
    local graphFile="${datFile%.dat}.${graphExt}"
    echo "Creating $graphFile..."
gnuplot -e "set terminal pdf size 29.7cm, 84.1cm; \
            set output '${graphFile}'; \
            set multiplot layout 5,1 title 'Biology Data Overview'; \
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
  tar -czvf "$archiveName.tar.gz" CMakeLists.txt utils.sh README.md src
}

main() {
  case $1 in
    build)
      buildProject
      ;;
    build_all)
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
      echo "Usage: $0 [build_all|build|archive <archiveName>|create_dataset]"
      ;;
  esac
}


main "$@"
