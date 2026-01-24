file(GLOB_RECURSE GAME_SOURCES "src/*.cpp")
file(GLOB_RECURSE GAME_HEADERS "include/*.h")

add_executable(${PROJECT_NAME} ${GAME_SOURCES} ${GAME_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Only copy data folder if it exists
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
        COMMENT "Copying game data folder to build directory"
    )
endif()
