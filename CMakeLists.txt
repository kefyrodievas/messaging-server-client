cmake_minimum_required(VERSION 3.10)

project(Msg)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-O2 -Wall -Wextra")
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# find_package(SDL2 REQUIRED)

# file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
# add_executable(${PROJECT_NAME} ${SOURCES})

file(GLOB_RECURSE SERVER ${PROJECT_SOURCE_DIR}/src/Server/*.c)
add_executable(Server ${SERVER})

file(GLOB_RECURSE CLIENT ${PROJECT_SOURCE_DIR}/src/Client/*.c)
add_executable(Client ${CLIENT})

# add_custom_target( server )
# add_dependencies(server Server)
# add_custom_target( client )
# add_dependencies(client Client)

# target_link_libraries(${PROJECT_NAME} pthread)
# target_compile_features(${PROJECT_NAME} PUBLIC)
# set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
