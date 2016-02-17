{
  "target_defaults": {
      "default_configuration": "Release",
      "xcode_settings": {
        "CLANG_CXX_LIBRARY": "libc++",
        "CLANG_CXX_LANGUAGE_STANDARD":"c++11",
        "GCC_VERSION": "com.apple.compilers.llvm.clang.1_0",
      },
      "cflags_cc": ["-std=c++11"],
      "configurations": {
          "Debug": {
              "defines": [
                  "DEBUG"
              ],
              "xcode_settings": {
                  "GCC_OPTIMIZATION_LEVEL": "0",
                  "GCC_GENERATE_DEBUGGING_SYMBOLS": "YES",
                  "OTHER_CPLUSPLUSFLAGS": [ "-Wall", "-Wextra", "-pedantic", "-g", "-O0" ]
              }
          },
          "Release": {
              "defines": [
                  "NDEBUG"
              ],
              "xcode_settings": {
                  "GCC_OPTIMIZATION_LEVEL": "3",
                  "GCC_GENERATE_DEBUGGING_SYMBOLS": "NO",
                  "DEAD_CODE_STRIPPING": "YES",
                  "GCC_INLINES_ARE_PRIVATE_EXTERN": "YES",
                  "OTHER_CPLUSPLUSFLAGS": [ "-Wall", "-Wno-unknown-pragmas", "-O3" ]
              }
          }
      }
  }
}

