A node module for reading the HDF5 file format.

# Dependencies

+ [HDF5 C++ Library](http://www.hdfgroup.org/downloads/index.html) v5-1.8.8

# Compiling

When compiling the HDF5 C++ library, be sure to use the `--enable-cxx` flag. I have installed HDF5 into `/usr/local/hdf5`, so modify `binding.gyp` if yours is different. Also, I can only get HDF5 to properly compile if I manually specify `LD_LIBRARY_PATH`. So, be sure to `export LD_LIBRARY_PATH=/usr/local/hdf5/lib` prior to compiling. If you receive a `file does not exist` error from `libhdf5.so`, then you need to double check your `LD_LIBRARY_PATH` environment variable.