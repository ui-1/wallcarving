cmake_minimum_required(VERSION 3.10)

project(Task)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTASK_ROOT_PATH=\\\"\"${CMAKE_CURRENT_LIST_DIR}\\\"\"")

set(SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shader_util.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/floor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/userInput.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wall.cpp
)

set(HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/include/shader_util.h
    ${CMAKE_CURRENT_LIST_DIR}/include/floor.h
    ${CMAKE_CURRENT_LIST_DIR}/include/userInput.h
    ${CMAKE_CURRENT_LIST_DIR}/include/wall.h
)

add_executable(DebugBuild ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(DebugBuild PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(DebugBuild PRIVATE glfw OpenGL::GL GLEW::GLEW glm::glm-header-only)

add_executable(StandaloneRelease ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(StandaloneRelease PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(StandaloneRelease PRIVATE glfw OpenGL::GL GLEW::GLEW glm::glm-header-only)

set_target_properties(StandaloneRelease PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin
    OUTPUT_NAME "wallcarving"
)

target_compile_definitions(StandaloneRelease PRIVATE
    NDEBUG
    _WINDOWS
)

target_compile_options(StandaloneRelease PRIVATE
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Release>:/GL>
)

set_target_properties(StandaloneRelease PROPERTIES
    LINK_FLAGS_RELEASE "/LTCG /OPT:REF /OPT:ICF"
)
