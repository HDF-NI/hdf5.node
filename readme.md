A node module for reading the HDF5 file format.

# Dependencies

+ [HDF5 C++ Library](http://www.hdfgroup.org/downloads/index.html) v5-1.8.8

# Compiling

When compiling the HDF5 C++ library, be sure to use the `--enable-cxx` flag. I have installed HDF5 into `/usr/local/hdf5`, so modify `binding.gyp` if yours is different.