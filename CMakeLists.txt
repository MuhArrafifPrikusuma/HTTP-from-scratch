cmake_minimum_required(VERSION 3.13)
project(HTTPfs C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(server
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/server.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/server_error.c"
)
target_include_directories(server PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/include"
"${CMAKE_CURRENT_SOURCE_DIR}/server"
)

# native CPU arch for max performance
# NOTE: may cause issues
option(ENABLE_NATIVE_ARCH "Optimize for the build machine's CPU" OFF)

include(CheckCCompilerFlag)
function(add_flag_if_supported target flag)
  string(MAKE_C_IDENTIFIER "${flag}" flag_var)
  check_c_compiler_flag("${flag}" HAS_${flag_var})
  if(HAS_${flag_var})
    target_compile_options(${target} PRIVATE ${flag})
  endif()
endfunction()

add_flag_if_supported(server -O3)
# add_flag_if_supported(server -Wall)
# add_flag_if_supported(server -Wextra)

if(ENABLE_NATIVE_ARCH)
  add_flag_if_supported(server -march=native)
endif()

# Linker
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
  set_target_properties(server PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "LTO not supported: ${ipo_error}")
endif()
