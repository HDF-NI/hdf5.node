---
layout: page
title: "Install & Setup"
category: doc
date: 2015-05-02 12:02:34
---

## Status

Currently testing with node v0.12.0 and V8 3.28.73.  Builds on Ubuntu 14_04, CentOS 6 and MacOSX 10.7.  Working on getting access to a 10.10 version to solve later build issues. And setting up a windows work environment for VS2013 [any knowledge about node-gyp on these platforms would be much appreciated] 
Resources leaks are being found when the h5 file is closed.  When found they are being eliminated.  Error handling component is being investigated; how to best leverage V8 and node from the native side.

## Dependencies

+ [HDF5 C Library](http://www.hdfgroup.org/downloads/index.html) v5-1.8.14
        (Prior v5-1.8.x's untested yet should work)

The `binding.gyp` expects the HDF5_HOME environment variable set to your install.


## Compiling

The code requires a gcc compiler supporting C++11 for linux. MacOSX build target has been added.  Windows build target is coming. The binding.gyp defines the cflags with -std=c++11.  There isn't any cxxflags that I know of but cflags in node-gyp does 
effect g++.

### In a working copy of git

```bash
export HDF5_HOME=/home/roger/NodeProjects/hdf5
export NODE_PATH=/home/roger/NodeProjects/hdf5.node/build/Release/obj.target:$NODE_PATH
node-gyp configure build
```

NODE_PATH is still used for the mocha tests.

### Including as a node module

The HDF5_HOME needs to be set. NODE_PATH should not. Then an 'npm install hdf5' will pull the version and build it for you
in node_modules/hdf5. There is no need for the node-gyp step.


## Environment Variables

The path to the HDF5 shared objects must be added to the runtime library search path. 

for linux
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./hdf5.node/../hdf5/lib
```

for Mac OSX
```bash
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:./hdf5.node/../hdf5/lib
```

If you want one of the third party filters available put its install path on HDF5_PLUGIN_PATH
```bash
export HDF5_PLUGIN_PATH=/home/roger/NodeProjects/HDF5Plugin
```

## Running Test

The tests are based on co-mocha

```bash
mocha --require should  --require co-mocha
```


