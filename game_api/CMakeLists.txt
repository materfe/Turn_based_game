file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(api STATIC ${SRC_FILES} ${HEADER_FILES})
target_include_directories(api PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_executable(game tmp/game.cc)
target_link_libraries(game PRIVATE api)