import * as indexCJS from './index.cjs';

export const hdf5 = indexCJS.hdf5 || indexCJS.default.hdf5;
export const h5lt = indexCJS.h5lt || indexCJS.default.h5lt;
export const h5tb = indexCJS.h5tb || indexCJS.default.h5tb;
export const h5pt = indexCJS.h5pt || indexCJS.default.h5pt;
export const h5im = indexCJS.h5im || indexCJS.default.h5im;
export const h5ds = indexCJS.h5ds || indexCJS.default.h5ds;

export default {
  hdf5,
  h5lt,
  h5tb,
  h5pt,
  h5im,
  h5ds
};
