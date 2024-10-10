#include <stdlib.h>
#include <stack>
#include <thread>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader_util.h"

GLuint createWall(glm::vec3 color, float s);

#define CONCAT_PATHS(A, B) A "/" B
#define ADD_ROOT(B) CONCAT_PATHS(TASK_ROOT_PATH, B)

shader_prog shader(
    ADD_ROOT("shaders/chopper.vert.glsl"),
    ADD_ROOT("shaders/chopper.frag.glsl")
);

GLuint floorVAO;

void initWalls() {
    floorVAO = createWall(glm::vec3(0.22, 0.22, 0.22), 20.0f);
}


GLuint createWall(glm::vec3 color, float s) {
    GLfloat vertices[] = {
                            -s, -s, 0.0,
                             s, -s, 0.0,
                             s,  s, 0.0,
                            -s,  s, 0.0
                        };

    GLubyte indices[] = {
                            0, 1, 2,
                            0, 2, 3
                        };

    GLfloat colors[] = {
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2]
                        };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    shader.attribute3fv("position", vertices, 12);
    shader.attribute3fv("color", colors, 12);

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*12, indices, GL_STATIC_DRAW);
    return vertexArrayHandle;
}


void drawFloor() {
    std::stack<glm::mat4> ms;
    ms.push(glm::mat4(1.0));
        ms.push(ms.top());
            ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
            shader.uniformMatrix4fv("modelMatrix", ms.top());
            glBindVertexArray(floorVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        ms.pop();
        ms.push(ms.top());
    ms.pop();
}


int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(640, 480, "evil gnome hits wall with pickaxe", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

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
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 6.0, 15.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    shader.uniformMatrix4fv("projectionMatrix", projection);
    shader.uniformMatrix4fv("viewMatrix", view);

    initWalls();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(win)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawFloor();
        glfwSwapBuffers(win);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
