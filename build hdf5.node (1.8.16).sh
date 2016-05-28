#!/bin/bash

function pause(){
   read -n 1 -p "Press any key to continue..."
}

export PYTHONHOME=/home/roger/Python-2.7.8/dist
export PYTHONPATH=/home/roger/Software/ActiveTcl-8.6.1/lib:$PYTHONHOME/lib:$PYTHONHOME/lib/python-2.7:/home/roger/Software/numpy-1.8.1/dist/lib/python2.7/site-packages:/home/roger/Software/h5py-2.3.1/dist/lib/python2.7/site-packages:/home/roger/Software/mercurial-3.1-rc/dist/lib/python2.7/site-packages:/home/roger/Software/ActiveTcl-8.6.1/lib:/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu:/lib64:$PYTHONPATH

export HDF5_HOME=/home/roger/NodeProjects/hdf5_1.8.16
export hdf5_home_linux=$HDF5_HOME
#/home/roger/Software/ros/dist
export NODE_HOME=/home/roger/Software/node-v6.0.0-linux-x64
export V8_HOME=$NODE_HOME/../deps/v8
export PATH=/home/roger/Software/gcc/dist/bin:$PYTHONHOME/bin:$NODE_HOME/bin:$V8_HOME/out/x64.release:$PATH
export LD_LIBRARY_PATH=$PYTHONHOME/lib:$PYTHONHOME/lib/python-2.7:/home/roger/Software/gcc/dist/lib64:/home/roger/NetBeansProjects/zlib/lib:$HDF5_HOME/lib:/home/roger/Software/ActiveTcl-8.6.1/lib:/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu:/lib64:$LD_LIBRARY_PATH

export FC=gfortran
export CC=gcc
export CXX=g++
#export LDFLAGS="-fPIC -O4 -L$HDF5_HOME/lib -I$HDF5_HOME/include/oce "
#export CFLAGS="-fPIC -O4 -I/home/roger/NodeProjects/V8new/node/src -I$V8_HOME/include -I$V8_HOME/src -I$HDF5_HOME/include -I$HDF5_HOME/include/oce "
#export CXXFLAGS="-fPIC -O4 -std=c++11 -I/home/roger/NodeProjects/V8new/node/src -I$V8_HOME/include -I$V8_HOME/src -I$HDF5_HOME/include -I$HDF5_HOME/include/oce "
#export CPPFLAGS="-fPIC -O4 -I/home/roger/NodeProjects/V8new/node/src -I$V8_HOME/include -I$V8_HOME/src -I$HDF5_HOME/include -I$HDF5_HOME/include/oce "

#/home/roger/Software/git-1.8.5.2/dist/bin/git pull

which python
export PYTHON=$PYTHONHOME/bin/python2.7
export HDF5_PLUGIN_PATH=/home/roger/NodeProjects/HDF5Plugin

#$NODE_HOME/bin/npm install  --save-dev aws-sdk
npm owner ls
export hdf5_home_linux=$HDF5_HOME
#$NODE_HOME/bin/npm install  --build-from-source --hdf5_home_linux=$HDF5_HOME

export PATH=$NODE_HOME/bin:/home/roger/NodeProjects/hdf5.node/node_modules/node-pre-gyp/bin:$PATH
export NODE_PATH=.:/home/roger/NodeProjects/hdf5.node/build/Release:$NODE_PATH
export LD_LIBRARY_PATH=$HDF5_HOME/lib:$LD_LIBRARY_PATH

export PATH=/home/roger/NodeProjects/hdf5.node/node_modules/mocha/bin:$PATH
which mocha

#$NODE_HOME/bin/npm start &
#pause
#$NODE_HOME/bin/npm test

#mocha --harmony --require should --require co-mocha #2> /dev/null
#mocha --harmony --require should --require co-mocha -g 'stream makes a dataset' ./test/test_h5lt.js #2> /dev/null
#mocha --harmony --require should --require co-mocha -g 'stream dataset' ./test/test_streams.js #2> /dev/null
mocha --harmony --require should --require co-mocha ./test/test_h5ds.js #2> /dev/null

#which node
#node --version
#node-gyp --version
pwd

cp ./build/Release/*.node ./lib/binding
export node_pre_gyp_accessKeyId="AKIAIRCVFZRDCIFWT5TA"
export node_pre_gyp_secretAccessKey="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"

#node-pre-gyp package unpublish publish

#node --help
#node --v8-options
#node -e "console.log(process.versions.v8)"
#/home/roger/NodeProjects/V8new/node/dist/bin/sweeper 
$NODE_HOME/bin/npm outdated 
#/home/roger/NodeProjects/V8new/node/dist/bin/npm ls
pause
