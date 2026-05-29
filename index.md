---
layout: default
title: "HDF5 Docs"
---

## Get Started

```bash
npm install hdf5
```
If your native hdf5 libraries aren't at the default 
you can set the path with --hdf5_home_linux switch on this project as well as 
dependent projects.

Note: for many package apps a switch like --hdf5_home_linux will no longer be possible.  Need to find a new way

```bash
npm install hdf5 --hdf5_home_linux=<your native hdf path>
```
For mac and windows the switches are --hdf5_home_mac & --hdf5_home_win

Because this project has native code look at [Install & Setup]({{ site.baseurl }}/doc/install-setup.html) for the details.

## Usage

### ECMAScript Modules (ESM)
If your project uses `"type": "module"`, you can import `hdf5` natively. Due to how the native bindings are structured, use named imports or destructure the package object:

```javascript
import hdf5Package from 'hdf5';
const { hdf5 } = hdf5Package;
const { Access } = hdf5Package; // or from 'hdf5/lib/globals' depending on your export mapping

const file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
```

### CommonJS (CJS)
If you are using legacy CommonJS scripts, the module remains backward-compatible via `index.cjs`:

```javascript
const hdf5 = require('hdf5').hdf5;
const { Access } = require('hdf5/lib/globals');

const file = new hdf5.File(currentH5Path, Access.ACC_RDONLY);
```
