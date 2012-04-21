{
    'targets': [
        {
            'target_name': 'hdf5',
            'sources': ['./src/node_hdf5.cc', './src/node_h5file.cc'],
            'include_dirs': [ '/usr/local/hdf5/include' ]
        }
    ]
}