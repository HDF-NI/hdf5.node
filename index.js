'use strict';

for (let mod of ["hdf5", "h5lt", "h5tb", "h5pt", "h5im", "h5ds"]) {
  module.exports[mod] = require(`./build/Release/${mod}`);
}
