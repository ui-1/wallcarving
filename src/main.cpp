#include <stdlib.h>
#include <stack>
#include <thread>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "userInput.h"
#include "floor.h"
#include "shader_util.h"

#define CONCAT_PATHS(A, B) A "/" B
#define ADD_ROOT(B) CONCAT_PATHS(TASK_ROOT_PATH, B)

shader_prog shader(
    ADD_ROOT("shaders/chopper.vert.glsl"),
    ADD_ROOT("shaders/chopper.frag.glsl")
);

int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(1920, 1080, "evil gnome hits wall with pickaxe", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetCursorPosCallback(win, mouse_position_callback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //hide cursor

    //for movement
    glfwSetKeyCallback(win, key_callback);

    glfwMakeContextCurrent(win);
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(status));
    }

    const GLubyte* renderer = glGetString (GL_RENDERER); 
    const GLubyte* version = glGetString (GL_VERSION);
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    shader.use();

    glm::mat4 projection = glm::perspective(glm::radians(80.0f), 4.0f / 3.0f, 0.1f, 100.f);

    shader.uniformMatrix4fv("projectionMatrix", projection);


    GLuint floorVAO = initWalls(shader);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

while (!glfwWindowShouldClose(win)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Continuous camera movement based on key states
    if (moveForward) {
        cameraPos += cameraSpeed * front; // Move forward
    }
    if (moveBackward) {
        cameraPos -= cameraSpeed * front; // Move backward
    }
    if (moveLeft) {
        cameraPos -= glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))) * cameraSpeed; // Move left
    }
    if (moveRight) {
        cameraPos += glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))) * cameraSpeed; // Move right
    }

    // Update the view matrix each frame
    glm::vec3 target = cameraPos + front; // Calculate target position based on current camera position and front vector
    glm::mat4 view = glm::lookAt(cameraPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.uniformMatrix4fv("viewMatrix", view);

    drawFloor(floorVAO, shader);

    glfwSwapBuffers(win);
    glfwPollEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}


    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
