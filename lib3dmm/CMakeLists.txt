cmake_minimum_required(VERSION 3.0)
project(lib3dmm
    VERSION 0.1
    DESCRIPTION "lib3dmm"
    LANGUAGES CXX
    )

set(LIB3DMM_EXPORT 1)

add_library(lib3dmm SHARED src/lib3dmm.cpp inc/lib3dmm.h)

target_compile_definitions(lib3dmm PRIVATE LIB3DMM_EXPORT)

# For example if you need to include or link other libraries
# target_include_directories(lib3dmm PRIVATE ${MY_INCLUDE_DIR})
# target_link_libraries(lib3dmm PRIVATE ${MY_LINK_LIB})

install(TARGETS lib3dmm DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES lib3dmm.h DESTINATION ${CMAKE_INSTALL_PREFIX}/inc)


#target_include_directories(main PRIVATE ${MY_INCLUDE_DIR})

include_directories("${PROJECT_SOURCE_DIR}/inc")

add_executable(main src/main.cpp)

# For example, if you want to link the lib3dmm library to main target
target_link_libraries(main PRIVATE lib3dmm)
