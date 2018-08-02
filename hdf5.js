'use strict';

const isDebug = !!parseInt(process.env.HDF5_DEBUG);
const configName = isDebug ? 'Debug' : 'Release';

for (let mod of ["hdf5", "h5lt", "h5tb", "h5pt", "h5im", "h5ds"]) {
  module.exports[mod] = require(`./build/${configName}/${mod}.node`);
}
