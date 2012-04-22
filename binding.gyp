{
    'targets': [
        {
            'target_name': 'hdf5',
            'include_dirs': [
                '/usr/local/hdf5/include'
            ],
            'sources': [
                'src/node_hdf5.cc',
                'src/node_h5file.cc'
            ]
        }
    ]
}