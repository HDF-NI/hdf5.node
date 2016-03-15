{
    "variables": {
        "hdf5_home_linux": "/usr/local",
        "hdf5_home_win": "C:/Software/hdf5",
        "hdf5_home_mac": "/usr/local"
    },
    'targets': [
        {
            'target_name': 'hdf5',
            'conditions': [
            ['OS=="linux"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'include_dirs': [
                    '<(hdf5_home_linux)/include'
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
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
                'include_dirs': [
                    '<(hdf5_home_win)/include',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ]
                }
            }],
            ['OS=="mac"', {
                'cflags!': [ '-fno-exceptions' ],
                'cflags_cc!': [ '-fno-exceptions' ],
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O3", "-std=c++14", "-fexceptions","-v", "-stdlib=libc++", "-Werror"],
                    'OTHER_LDFLAGS': [],
                    'MACOSX_DEPLOYMENT_TARGET': '10.10'
                },
                'include_dirs': [
                    '<(hdf5_home_mac)/include'
                ],
                'sources': [
                    'src/hdf5.cc',
                    'src/h5_file.cc',
                    'src/h5_group.cc',
                ],
                'libraries': [
                    '<(hdf5_home_mac)/lib/libhdf5.dylib',
                    '<(hdf5_home_mac)/lib/libhdf5_hl.dylib'
                ],
                'link_settings': {
                    'libraries': [
                        '<(hdf5_home_mac)/lib/libhdf5.dylib',
                        '<(hdf5_home_mac)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib'
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
                    '<(hdf5_home_linux)/include'
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
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5lt.cc'
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O3", "-std=c++14", "-fexceptions","-v", "-stdlib=libc++", "-Werror"],
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
                        '<(hdf5_home_mac)/lib/libhdf5.dylib',
                        '<(hdf5_home_mac)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib'
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
                    '<(hdf5_home_linux)/include'
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O3", "-std=c++14", "-fexceptions","-v", "-stdlib=libc++", "-Werror"],
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
                        '<(hdf5_home_mac)/lib/libhdf5.dylib',
                        '<(hdf5_home_mac)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib'
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
                    '<(hdf5_home_linux)/include'
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
                        '-L<(hdf5_home_linux)/lib'
                    ]
                }
            }],
            ['OS=="win"', {
                'include_dirs': [
                    '<(hdf5_home_win)/include',
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O3", "-std=c++14", "-fexceptions","-v", "-stdlib=libc++", "-Werror"],
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
                        '<(hdf5_home_mac)/lib/libhdf5.dylib',
                        '<(hdf5_home_mac)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib'
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
                    '<(hdf5_home_linux)/include'
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'cflags': ['-fPIC', "-O4", "-std=c++14", "-fexceptions"],
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O3", "-std=c++14", "-fexceptions", "-v", "-stdlib=libc++", "-Werror"],
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
                        '<(hdf5_home_mac)/lib/libhdf5.dylib',
                        '<(hdf5_home_mac)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib'
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
                    '<(hdf5_home_linux)/include'
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
                'include_dirs': [
                    '<(hdf5_home_win)/include',
                    './win/include',
                    './src',
                    'C:/Software/node-v0.12.7/src',
                    'C:/Program Files/Microsoft SDKs/Windows/v7.1/Include'
                ],
                'sources': [
                    'src/h5ds.cc'
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
                        '<(hdf5_home_win)/lib/hdf5.lib',
                        '<(hdf5_home_win)/lib/hdf5_hl.lib'
                    ],
                }
            }],
            ['OS=="mac"', {
                'xcode_settings': {
                    'OTHER_CPLUSPLUSFLAGS': ["-fPIC", "-O3", "-std=c++14", "-fexceptions", "-v", "-stdlib=libc++", "-Werror"],
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
                        '<(hdf5_home_mac)/lib/libhdf5.dylib',
                        '<(hdf5_home_mac)/lib/libhdf5_hl.dylib'
                    ],
                    'ldflags': [
                        '-L<(hdf5_home_mac)/lib'
                    ]
                }
            }]
          ]
        } ,
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
