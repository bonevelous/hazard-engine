cmake_minimum_required(VERSION 4.0)
project(hazard-engine)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

set(SOURCES
	${CMAKE_SOURCE_DIR}/src/main.c
	${CMAKE_SOURCE_DIR}/src/hazard-engine.c
)

set(BUILD_SOURCES ${SOURCES}
	${CMAKE_SOURCE_DIR}/src/hazard-build.c
)

set(GAME_SOURCES ${SOURCES}
	${CMAKE_SOURCE_DIR}/src/hazard-actor.c
	${CMAKE_SOURCE_DIR}/src/harvhaz.c
)

find_package(SDL3 REQUIRED)

add_executable(hazard-build ${BUILD_SOURCES})
add_executable(harvhaz ${GAME_SOURCES})
target_link_libraries(hazard-build SDL3::SDL3 m)
target_link_libraries(harvhaz SDL3::SDL3 m)
