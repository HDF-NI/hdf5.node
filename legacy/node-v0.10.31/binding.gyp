{
    'targets': [
        {
            'target_name': 'hdf5',
            'cflags': ['-fPIC', "-O4", "-std=c++11"],
            'include_dirs': [
                '../../../hdf5/include'
            ],
            'sources': [
                'src/hdf5.cc',
                'src/h5_file.cc',
                'src/h5_group.cc',
            ],
            'link_settings': {
                'libraries': [
                    '-lhdf5',
                    '-lhdf5_hl',
                    '-lhdf5_cpp'
                ],
                'ldflags': [
                    '-L../../../../hdf5/lib'
                ]
            }
        },
        {
            'target_name': 'h5lt',
            'cflags': ['-fPIC', "-O4", "-std=c++11"],
            'include_dirs': [
                '../../../hdf5/include'
            ],
            'sources': [
                'src/Float64Array.hpp',
                'src/h5lt.cc',
                'src/h5_lt.hpp'
            ],
            'link_settings': {
                'libraries': [
                    '-lhdf5',
                    '-lhdf5_hl',
                    '-lhdf5_cpp'
                ],
                'ldflags': [
                    '-L../../../../hdf5/lib'
                ]
            }
        },
        {
            'target_name': 'h5im',
            'cflags': ['-fPIC', "-O4", "-std=c++11"],
            'include_dirs': [
                '../../../hdf5/include'
            ],
            'sources': [
                'src/h5im.cc',
                'src/h5_im.hpp'
            ],
            'link_settings': {
                'libraries': [
                    '-lhdf5',
                    '-lhdf5_hl',
                    '-lhdf5_cpp'
                ],
                'ldflags': [
                    '-L../../../../hdf5/lib'
                ]
            }
        }
    ]
}