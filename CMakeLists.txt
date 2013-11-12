project(PrintHagakiAddress)
cmake_minimum_required(VERSION 2.8)

include(FindPkgConfig)

# cairo
pkg_check_modules(CAIRO cairo)
INCLUDE_DIRECTORIES(${CAIRO_INCLUDE_DIRS})
LINK_DIRECTORIES(${CAIRO_LIBRARY_DIRS})
LINK_LIBRARIES(${CAIRO_LIBRARIES})

# freetype2
pkg_check_modules(FREETYPE2 freetype2)
INCLUDE_DIRECTORIES(${FREETYPE2_INCLUDE_DIRS})
LINK_DIRECTORIES(${FREETYPE2_LIBRARY_DIRS})
LINK_LIBRARIES(${FREETYPE2_LIBRARIES})

# harfbuzz
pkg_check_modules(HARFBUZZ harfbuzz)
INCLUDE_DIRECTORIES(${HARFBUZZ_INCLUDE_DIRS})
LINK_DIRECTORIES(${HARFBUZZ_LIBRARY_DIRS})
LINK_LIBRARIES(${HARFBUZZ_LIBRARIES})

# harfbuzz-icu
pkg_check_modules(HARFBUZZ_ICU harfbuzz-icu)
INCLUDE_DIRECTORIES(${HARFBUZZ_ICU_INCLUDE_DIRS})
LINK_DIRECTORIES(${HARFBUZZ_ICU_LIBRARY_DIRS})
LINK_LIBRARIES(${HARFBUZZ_ICU_LIBRARIES})

set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -DCMAKE_SOURCE_DIR=\\\"${CMAKE_SOURCE_DIR}\\\"")

ADD_CUSTOM_TARGET(debug
                    ${CMAKE_COMMAND} 
                      -DCMAKE_BUILD_TYPE:STRING=Debug ${PROJECT_SOURCE_DIR})
ADD_CUSTOM_TARGET(release
                    ${CMAKE_COMMAND}
                      -DCMAKE_BUILD_TYPE:STRING=Release ${PROJECT_SOURCE_DIR})

set(CMAKE_C_FLAGS_DEBUG "-O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Make sure to link to Harfbuzz's since it may be installed locally atm.
set(CMAKE_INSTALL_RPATH "${HARFBUZZ_LIBRARY_DIRS}")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

# Uninstaller
configure_file ("${PROJECT_SOURCE_DIR}/src/uninstall.sh.in"
                "${PROJECT_BINARY_DIR}/uninstall.sh")

add_executable(printha src/printha_settings.cpp src/printha.cpp)

set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
install(TARGETS printha
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)

