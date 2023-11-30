#!/bin/bash

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
    mkdir -p build && cd build || return
    cmake .. && make
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
        *)
            echo "Usage: $0 [build|archive <archiveName>]"
            ;;
    esac
}

main "$@"
