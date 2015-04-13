{
    'targets': [
        {
            'target_name': 'hdf5',
            'conditions': [
            ['OS=="linux"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/hdf5.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
            }],
            ['OS=="mac"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-O4", "-std=c++11", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions"]
                },
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/hdf5.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/libhdf5.dylib',
                        '$(HDF5_HOME)/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }]
          ]
        },
        {
            'target_name': 'h5lt',
            'conditions': [
            ['OS=="linux"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5lt.cc',
                    'src/h5_lt.hpp'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions"]
                },
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5lt.cc',
                    'src/h5_lt.hpp'
                ],
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/libhdf5.dylib',
                        '$(HDF5_HOME)/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }]
          ]
        },
        {
            'target_name': 'h5tb',
            'conditions': [
            ['OS=="linux"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5tb.cc',
                    'src/h5_tb.hpp'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions"]
                },
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5tb.cc',
                    'src/h5_tb.hpp'
                ],
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/libhdf5.dylib',
                        '$(HDF5_HOME)/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }]
          ]
        },
        {
            'target_name': 'h5pt',
            'conditions': [
            ['OS=="linux"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5pt.cc',
                    'src/h5_pt.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions"]
                },
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5pt.cc',
                    'src/h5_pt.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/libhdf5.dylib',
                        '$(HDF5_HOME)/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }]
          ]
        },
        {
            'target_name': 'h5im',
            'conditions': [
            ['OS=="linux"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5im.cc',
                    'src/h5_im.hpp'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions"]
                },
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5im.cc',
                    'src/h5_im.hpp'
                ],
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/libhdf5.dylib',
                        '$(HDF5_HOME)/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }]
          ]
        }
    ]
}