#include <stdlib.h>
#include <stack>
#include <thread>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "floor.h"
#include "shader_util.h"


GLuint createWall(glm::vec3 color, float s, shader_prog shader) {
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

GLuint initWalls(shader_prog shader) {
    return createWall(glm::vec3(0.22, 0.22, 0.22), 20.0f, shader);
}


void drawFloor(GLuint floorVAO, shader_prog shader) {
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