cmake_minimum_required(VERSION 3.4)
project(serelepe)

set(JERRY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/jerryscript/jerry-core/include")
set(JERRY_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/jerryscript/build/lib")
set(JERRY_LIB "${JERRY_LIB_DIR}/libjerry-core.a")
include_directories(${JERRY_INCLUDE_DIR})
link_directories(${JERRY_LIB_DIR})

message("xx" ${JERRY_INCLUDE_DIR})
message("yy" ${JERRY_LIB_DIR})
message("ww" ${JERRY_LIB})

find_library(LIBC c)
message(LIBC ${LIBC})

add_executable(serelepe src/main.c)

set_target_properties(serelepe PROPERTIES LINKER_LANGUAGE "C")

target_link_libraries(serelepe ${LIBC} ${JERRY_LIB})

install(TARGETS serelepe DESTINATION lib)
install(FILES src/serelepe.h DESTINATION include)
