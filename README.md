# MolSim GroupA

This project provides a powerful interface to run particle simulations. The current version of the project is fairly limited in features, however optimizations and support for more complex particle simulations are to come.

To contribute or build the project locally, first run the `build.sh` file. This will setup CMake, create the Makefile and run the help message of the project. You can find the code documentation of the master branch at [our GitHub Pages](https://anatoly03.github.io/MolSim-WS25-GroupA/index.html) or you can opt to build it locally using the `doc_doxygen` target in the Makefile.

This project integrates [Tracy Profiler](https://github.com/wolfpld/tracy) for performance profiling.

Before commiting it's recommended to run make with `format` target as we care about the project remaining clean! Additionally with the `check` target we expose a CTest command that runs with preferred arguments: These tests should pass if you wish to get your Pull Request merged!

To get rid of output files we provide the `clean-vtk` target with make. Happy simulations!

### Advanced: Profiling

1. Download or build the Tracy profiler GUI (= server) from: https://github.com/wolfpld/tracy/releases. On MacOS you can install tracy from homebrew.
2. Run the Tracy GUI application
3. Run your MolSim executable (= client)
4. Tracy will automatically connect and start profiling
