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
                'include_dirs': [
                    '$(HDF5_HOME)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/hdf5.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'msbuild_toolset': 'v120',
                "configurations": {
                            "Release": {
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'RuntimeLibrary': 2,        # shared release
                            'ExceptionHandling': 1,     # /EHsc
                            'AdditionalOptions': 
                            [
                                '/EHsc' # Enable unwind semantics for Exception Handling.  This one actually does the trick - and no warning either.
                            ]
                        },
                        'VCLinkerTool':
                        {
                            'AdditionalOptions': 
                            [
                                '/FORCE:MULTIPLE'
                            ]
                        }
                    }
                }
                },
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/hdf5.lib',
                        '$(HDF5_HOME)/lib/hdf5_hl.lib'
                    ]
                }
            }],
            ['OS=="mac"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-O4", "-std=c++11", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions","-v"]
                },
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/hdf5.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'libraries': [
                    '$(HDF5_HOME)/lib/libhdf5.dylib',
                    '$(HDF5_HOME)/lib/libhdf5_hl.dylib'
                ],
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/libhdf5.dylib',
                        '$(HDF5_HOME)/lib/libhdf5_hl.dylib'
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
                'include_dirs': [
                    '$(HDF5_HOME)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5lt.cc',
                    'src/h5_lt.hpp'
                ],
                'msbuild_toolset': 'v120',
                "configurations": {
                            "Release": {
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'RuntimeLibrary': 2,        # shared release
                            'ExceptionHandling': 1,     # /EHsc
                            'AdditionalOptions': 
                            [
                                '/EHsc' # Enable unwind semantics for Exception Handling.  This one actually does the trick - and no warning either.
                            ]
                        },
                        'VCLinkerTool':
                        {
                            'AdditionalOptions': 
                            [
                                '/FORCE:MULTIPLE'
                            ]
                        }
                    }
                }
                },
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/hdf5.lib',
                        '$(HDF5_HOME)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions","-v"]
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
                        '$(HDF5_HOME)/lib/libhdf5.dylib',
                        '$(HDF5_HOME)/lib/libhdf5_hl.dylib'
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
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
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
                'include_dirs': [
                    '$(HDF5_HOME)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5tb.cc',
                    'src/h5_tb.hpp'
                ],
                'msbuild_toolset': 'v120',
                "configurations": {
                            "Release": {
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'RuntimeLibrary': 2,        # shared release
                            'ExceptionHandling': 1,     # /EHsc
                            'AdditionalOptions': 
                            [
                                '/EHsc' # Enable unwind semantics for Exception Handling.  This one actually does the trick - and no warning either.
                            ]
                        },
                        'VCLinkerTool':
                        {
                            'AdditionalOptions': 
                            [
                                '/FORCE:MULTIPLE'
                            ]
                        }
                    }
                }
                },
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/hdf5.lib',
                        '$(HDF5_HOME)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions","-v"]
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
                        '$(HDF5_HOME)/lib/libhdf5.dylib',
                        '$(HDF5_HOME)/lib/libhdf5_hl.dylib'
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
                'include_dirs': [
                    '$(HDF5_HOME)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5pt.cc',
                    'src/h5_pt.cc'
                ],
                'msbuild_toolset': 'v120',
                "configurations": {
                            "Release": {
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'RuntimeLibrary': 2,        # shared release
                            'ExceptionHandling': 1,     # /EHsc
                            'AdditionalOptions': 
                            [
                                '/EHsc' # Enable unwind semantics for Exception Handling.  This one actually does the trick - and no warning either.
                            ]
                        },
                        'VCLinkerTool':
                        {
                            'AdditionalOptions': 
                            [
                                '/FORCE:MULTIPLE'
                            ]
                        }
                    }
                }
                },
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/hdf5.lib',
                        '$(HDF5_HOME)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions","-v"]
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
                        '$(HDF5_HOME)/lib/libhdf5.dylib',
                        '$(HDF5_HOME)/lib/libhdf5_hl.dylib'
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
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
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
                'include_dirs': [
                    '$(HDF5_HOME)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5im.cc',
                    'src/h5_im.hpp'
                ],
                'msbuild_toolset': 'v120',
                "configurations": {
                            "Release": {
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'RuntimeLibrary': 2,        # shared release
                            'ExceptionHandling': 1,     # /EHsc
                            'AdditionalOptions': 
                            [
                                '/EHsc' # Enable unwind semantics for Exception Handling.  This one actually does the trick - and no warning either.
                            ]
                        },
                        'VCLinkerTool':
                        {
                            'AdditionalOptions': 
                            [
                                '/FORCE:MULTIPLE'
                            ]
                        }
                    }
                }
                },
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/hdf5.lib',
                        '$(HDF5_HOME)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions", "-v"]
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
                        '$(HDF5_HOME)/lib/libhdf5.dylib',
                        '$(HDF5_HOME)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }]
          ]
        },
        {
            'target_name': 'h5ds',
            'conditions': [
            ['OS=="linux"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5ds.cc',
                    'src/h5_ds.hpp'
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
                'include_dirs': [
                    '$(HDF5_HOME)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5ds.cc',
                    'src/h5_ds.hpp'
                ],
                'msbuild_toolset': 'v120',
                "configurations": {
                            "Release": {
                    'msvs_settings':
                    {
                        'VCCLCompilerTool':
                        {
                            'RuntimeLibrary': 2,        # shared release
                            'ExceptionHandling': 1,     # /EHsc
                            'AdditionalOptions': 
                            [
                                '/EHsc' # Enable unwind semantics for Exception Handling.  This one actually does the trick - and no warning either.
                            ]
                        },
                        'VCLinkerTool':
                        {
                            'AdditionalOptions': 
                            [
                                '/FORCE:MULTIPLE'
                            ]
                        }
                    }
                }
                },
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/hdf5.lib',
                        '$(HDF5_HOME)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O4", "-std=c++11", "-fexceptions", "-v"],
                    'OTHER_LDFLAGS': [],
                    'MAXOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '$(HDF5_HOME)/include'
                ],
                'sources': [
                    'src/h5ds.cc',
                    'src/h5_ds.hpp'
                ],
                'link_settings': {
                    'libraries': [
                        '$(HDF5_HOME)/lib/libhdf5.dylib',
                        '$(HDF5_HOME)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L$(HDF5_HOME)/lib'
                    ]
                }
            }]
          ]
        },
        {
            "target_name": "action_after_build",
            "type": "none",
            "dependencies": [ "<(module_name)" ],
            "copies": [
            {
              "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
              "destination": "<(module_path)"
            }
            ]
        }        

    ]
}
