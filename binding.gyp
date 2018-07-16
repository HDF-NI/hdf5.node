{
    "variables": {
        "hdf5_home_linux%": "/usr/local",
        "hdf5_home_win%": "C:/Program Files/HDF_Group/HDF5/1.10.2",
        "hdf5_home_mac%": "/usr/local",
        "link_type%": "shared",
        "debug_mode%": "",
        "longlong_type%": "LONGLONG64BITS"
    },
    'targets': [
        {
            'target_name': 'hdf5',
            'conditions': [
            ['OS=="linux"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'include_dirs': [
                    '<(hdf5_home_linux)/include',
                    "/usr/include/hdf5/serial"
                ],
                'sources': [
                    'src/int64.cc',
                    'src/uint64.cc',
                    'src/hdf5.cc',
                    'src/attributes.cc',
                    'src/methods.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
				'defines': [
					'H5_BUILT_AS_DYNAMIC_LIB'
				],
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/int64.cc',
                    'src/uint64.cc',
                    'src/hdf5.cc',
                    'src/attributes.cc',
                    'src/methods.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'msbuild_toolset': 'v140',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ]
                }
            }],
            ['OS=="mac"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-<(link_type)", "-O3", "-std=c++14", "-fexceptions", "-stdlib=libc++"],
                    'OTHER_LDFLAGS': [],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '<(hdf5_home_mac)/include'
                ],
                'sources': [
                    'src/int64.cc',
                    'src/uint64.cc',
                    'src/hdf5.cc',
                    'src/attributes.cc',
                    'src/methods.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'link_settings': {
                    'libraries': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ]
                }
            }]
          ]
        },
        {
            'target_name': 'h5lt',
            'conditions': [
            ['OS=="linux"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'include_dirs': [
                    '<(hdf5_home_linux)/include',
                    "/usr/include/hdf5/serial"
                ],
                'sources': [
                    'src/h5lt.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
                'defines': [
					'H5_BUILT_AS_DYNAMIC_LIB'
				],
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/int64.cc',
                    'src/uint64.cc',
                    'src/h5lt.cc'
                ],
                'msbuild_toolset': 'v140',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-<(link_type)", "-O3", "-std=c++14", "-fexceptions", "-stdlib=libc++"],
                    'OTHER_LDFLAGS': [],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '<(hdf5_home_mac)/include'
                ],
                'sources': [
                    'src/h5lt.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
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
                'cflags': ["-D<(longlong_type)", "-fPIC", "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'include_dirs': [
                    '<(hdf5_home_linux)/include',
                    "/usr/include/hdf5/serial"
                ],
                'sources': [
                    'src/h5tb.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
                'defines': [
					'H5_BUILT_AS_DYNAMIC_LIB',
                                        "<(longlong_type)"
				],
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5tb.cc'
                ],
                'msbuild_toolset': 'v140',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-D<(longlong_type)", "-fPIC", "-<(link_type)", "-O3", "-std=c++14", "-fexceptions", "-stdlib=libc++"],
                    'OTHER_LDFLAGS': [],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '<(hdf5_home_mac)/include'
                ],
                'sources': [
                    'src/h5tb.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ]
                }
            }]
          ]
        },
        {
            'target_name': 'h5pt',
            'conditions': [
            ['OS=="linux"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'include_dirs': [
                    '<(hdf5_home_linux)/include',
                    "/usr/include/hdf5/serial"
                ],
                'sources': [
                    'src/int64.cc',
                    'src/uint64.cc',
                    'src/h5pt.cc',
                    'src/h5_pt.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
                'defines': [
					'H5_BUILT_AS_DYNAMIC_LIB'
				],
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/int64.cc',
                    'src/uint64.cc',
                    'src/h5pt.cc',
                    'src/h5_pt.cc'
                ],
                'msbuild_toolset': 'v140',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-<(link_type)", "-O3", "-std=c++14", "-fexceptions", "-stdlib=libc++"],
                    'OTHER_LDFLAGS': [],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '<(hdf5_home_mac)/include'
                ],
                'sources': [
                    'src/h5pt.cc',
                    'src/h5_pt.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
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
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'include_dirs': [
                    '<(hdf5_home_linux)/include',
                    "/usr/include/hdf5/serial"
                ],
                'sources': [
                    'src/h5im.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
                'defines': [
					'H5_BUILT_AS_DYNAMIC_LIB'
				],
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5im.cc'
                ],
                'msbuild_toolset': 'v140',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-<(link_type)", "-O3", "-std=c++14", "-fexceptions", "-stdlib=libc++"],
                    'OTHER_LDFLAGS': [],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '<(hdf5_home_mac)/include'
                ],
                'sources': [
                    'src/h5im.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
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
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'include_dirs': [
                    '<(hdf5_home_linux)/include',
                    "/usr/include/hdf5/serial"
                ],
                'sources': [
                    'src/h5ds.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
                'defines': [
					'H5_BUILT_AS_DYNAMIC_LIB'
				],
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5ds.cc'
                ],
                'msbuild_toolset': 'v140',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-<(link_type)", "-O4", "-std=c++14", "-fexceptions", "-Werror"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-<(link_type)", "-O3", "-std=c++14", "-fexceptions", "-stdlib=libc++"],
                    'OTHER_LDFLAGS': [],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '<(hdf5_home_mac)/include'
                ],
                'sources': [
                    'src/h5ds.cc'
                ],
                'link_settings': {
                    'libraries': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib',
                        '-lhdf5',
                        '-lhdf5_hl'
                    ]
                }
            }]
          ]
        }

    ]
}
