cmake_minimum_required(VERSION 3.13)
project(HTTPfs C)

# LTO is a MUST! so that extern const will be replaced with
# machine instructions at comptime eliminating runtime lookup
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(server
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common.c"
)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  # -O3: Enables aggressive optimizations
  # -march=znver2: native Zen 2 AVX2
  target_compile_options(server PRIVATE -O3 -march=znver2 -fomit-frame-pointer)
  set(IPO_TEST_FLAGS "-03" "-march=znver2")
else()
  target_compile_options(server PRIVATE -O3 -march=native)
  set(IPO_TEST_FLAGS "-O3" "-march=native")
endif()

# check and enable Link time optimizations
include(CheckIPOSupported)
# Compile IPO test with the FLAGS
check_ipo_supported(
  RESULT ipo_supported
  OUTPUT ipo_error
  OPTIONS ${IPO_TEST_FLAGS}
)

if(ipo_supported)
  set_target_properties(server PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "LTO not supported: ${ipo_error}")
endif()
