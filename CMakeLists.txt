cmake_minimum_required(VERSION 3.13)

project(TAT LANGUAGES CXX)

string(REGEX REPLACE "-O[23]" "-Ofast" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "-O[23]" "-Ofast" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REGEX REPLACE "-O[23]" "-Ofast" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
string(REGEX REPLACE "-O[23]" "-Ofast" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}")
find_package(TAT)

# 尝试产生本库的python接口, 先寻找系统中的pybind11, 否则尝试使用本目录下自行放置的pybind11
get_directory_property(TAT_HAS_PARENT PARENT_DIRECTORY)
if(NOT TAT_HAS_PARENT)
   if(TARGET PyTAT)
      set_target_properties(PyTAT PROPERTIES EXCLUDE_FROM_ALL OFF)
   endif()

   # examples中的各个程序
   link_libraries(TAT)

   file(GLOB CPP_SRC ${PROJECT_SOURCE_DIR}/examples/*.cpp ${PROJECT_SOURCE_DIR}/test/*.cpp)
   foreach(FILE ${CPP_SRC})
      get_filename_component(NAME ${FILE} NAME_WE)
      add_executable(${NAME} ${FILE})
   endforeach(FILE)

   # 以及他们的测试
   enable_testing()
   add_test(simple_test ${PROJECT_BINARY_DIR}/simple_test ${PROJECT_SOURCE_DIR}/test/simple_test.out)
   add_test(diag ${PROJECT_BINARY_DIR}/diag ${PROJECT_SOURCE_DIR}/examples/diag.out)
   add_test(mps ${PROJECT_BINARY_DIR}/mps 4 4 100 0.1 10 ${PROJECT_SOURCE_DIR}/examples/mps.out)
   add_test(fermi-diag ${PROJECT_BINARY_DIR}/fermi-diag ${PROJECT_SOURCE_DIR}/examples/fermi-diag.out)
   add_test(OBC ${PROJECT_BINARY_DIR}/OBC ${PROJECT_SOURCE_DIR}/examples/OBC.out)
endif()
