# ims
continuous simulations


## Running
Here's a description of how to run the kostyli solution:
1. `wget https://www.fit.vutbr.cz/\~peringer/SIMLIB/source/simlib-3.09-20221108.tar.gz`
2. `tar -xzf simlib-3.09-20221108.tar.gz`
3. `cd simlib` && `make` && `make PREFIX=$(pwd)/.. install` - here, the library should be installed successfully. Unless you use mac. For Darwin, fuckarounds needed.
4. `cd ..`
5. At this moment, we can build & run everything using CMake

## TODO
TODO: install & link the library dynamically