cmake_minimum_required(VERSION 3.27.0)

project(Task)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTASK_ROOT_PATH=\\\"\"${CMAKE_CURRENT_LIST_DIR}\\\"\"")

set(CMAKE_CXX_FLAGS_RELEASE "/Od /MT")

set(SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shader_util.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/floor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/userInput.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/wall.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/change_wall.cpp
)

set(HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/include/shader_util.h
    ${CMAKE_CURRENT_LIST_DIR}/include/floor.h
    ${CMAKE_CURRENT_LIST_DIR}/include/userInput.h
    ${CMAKE_CURRENT_LIST_DIR}/include/wall.h
    ${CMAKE_CURRENT_LIST_DIR}/include/change_wall.h
)

add_executable(Task ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(Task PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(Task PRIVATE glfw OpenGL::GL GLEW::GLEW glm::glm-header-only)

# Additional configurations and properties
set_target_properties(Task PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin
    OUTPUT_NAME "wallcarving"
)
set(CMAKE_BUILD_TYPE Release)

target_compile_definitions(Task PRIVATE
    NDEBUG
    _WINDOWS
)

set_target_properties(Task PROPERTIES
    LINK_FLAGS_RELEASE "/LTCG /OPT:REF /OPT:ICF"
)