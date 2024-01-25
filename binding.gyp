{
  'targets': [
    {
      'target_name': 'aes-mpc-lib-native',
      'variables': {
        'arch': '<!(./arch.sh)',
        'wd': '<!(pwd)'
      },
      'sources': [ 'node/aes_mpc_lib.cc', 'src/mpc.cpp' ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<(wd)/emp-tool",
        "<(wd)/emp-ot",
        "<(wd)/emp-agmpc",
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags': ["-march=<(arch)", "-O3", "-msse2", "-maes", "-lm", "-fstack-clash-protection", "-fstack-protector-all"],
      'cflags!': ["-fno-exceptions"],
      'cflags_cc': ["-march=<(arch)", "-O3", "-msse2", "-maes", "-lm", "-fstack-clash-protection", "-fstack-protector-all", "-std=c++11"],
      'cflags_cc!': ["-fno-exceptions"],
      'defines': ["NDEBUG", "_FORTIFY_SOURCE=2"],
      'libraries': ["-lpthread", "-ldl", "-L<(wd)/emp-tool/build"],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '12.0',
        'OTHER_CFLAGS': ["-march=<(arch)"],
        "OTHER_CPLUSPLUSFLAGS": ["-std=c++11", "-march=<(arch)"]
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      'conditions': [
        ["OS == 'linux'", {
          'libraries': ["-l:libcrypto.a", "-l:libssl.a"],
        }],
        ['node_shared_openssl=="false"', {
          # so when "node_shared_openssl" is "false", then OpenSSL has been
          # bundled into the node executable. So we need to include the same
          # header files that were used when building node.
          'include_dirs': [
            '<(node_root_dir)/deps/openssl/openssl/include'
          ],
          "conditions" : [
            ["target_arch=='ia32'", {
              "include_dirs": [ "<(node_root_dir)/deps/openssl/config/piii" ]
            }],
            ["target_arch=='x64'", {
              "include_dirs": [ "<(node_root_dir)/deps/openssl/config/k8" ]
            }],
            ["target_arch=='arm'", {
              "include_dirs": [ "<(node_root_dir)/deps/openssl/config/arm" ]
            }]
          ]
        }],
      ]
    }
  ]
}
