


cmake_minimum_required(VERSION 4.0.0 FATAL_ERROR)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_FLAGS "-stdlib=libc++")

project(evm-interpreter
  VERSION 0.1.0
  LANGUAGES CXX
)


set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(magic_enum STATIC)
target_sources(magic_enum PUBLIC
  FILE_SET modules
    TYPE CXX_MODULES
    FILES lib/magic_enum/magic_enum.cppm
)
target_compile_features(magic_enum PRIVATE cxx_std_26)

add_library(evmi SHARED)
target_sources(evmi PUBLIC
  FILE_SET modules
    TYPE CXX_MODULES
    FILES src/interpreter.cppm
)
target_compile_features(evmi
  PRIVATE cxx_std_26
  INTERFACE cxx_std_20
)
target_link_libraries(evmi
  PRIVATE magic_enum
)

add_executable(example)
target_sources(example
  PRIVATE example/main.cpp
)
target_compile_options(example
  PRIVATE "-fsanitize=address,leak"
)
target_compile_features(example
  PRIVATE cxx_std_26
)
target_link_libraries(example
  PRIVATE evmi
)
target_link_options(example
  PRIVATE "-fsanitize=address,leak"
)
