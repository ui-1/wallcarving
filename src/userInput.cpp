#include <GL/glew.h>             
#include <GLFW/glfw3.h>             
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "userInput.h"             
#include "shader_util.h"
#include "wall.h"
#include <iostream>
#include "change_wall.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // Initial camera position
glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f); // Initial front direction
WallMatrix* globalWM = nullptr;

// Movement variables
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;
float cameraSpeed = 0.1f; 

//Variables for looking around
static double lastX = 320.0;
static double lastY = 240.0;
static bool firstMouse = true;
static float yaw = -90.0f;
static float pitch = -90.0f;

/*  How this works:
*   1.Gets info from main.cpp'
*   2.On first pass set the lastX and lastY to the default cursor pos.
*   3.When the mouse moves, calculate the xOffset and yOffset(diff between old and new mouse movement)
*   4.yaw and pitch(mouse rotation on x[yaw] and y[pitch])
*   5.Clamp so you dont go around in circles when looking up
*   6.Change the front vector based on
*/
void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    double xOffset = xpos - lastX;
    double yOffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.2f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;
    //printf("The number is: %f\n", yaw);

    // so that cam cant get flipped upside down
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Calculate new front vector
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front); // Normalize to ensure consistent direction
}

//Movement
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_W:
                moveForward = true;
                break;
            case GLFW_KEY_S:
                moveBackward = true;
                break;
            case GLFW_KEY_A:
                moveLeft = true;
                break;
            case GLFW_KEY_D:
                moveRight = true;
                break;
            default:
                break;
        }
    }
    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_W:
                moveForward = false;
                break;
            case GLFW_KEY_S:
                moveBackward = false;
                break;
            case GLFW_KEY_A:
                moveLeft = false;
                break;
            case GLFW_KEY_D:
                moveRight = false;
                break;
            default:
                break;
        }
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (globalWM != nullptr) {
            glm::vec3 clickedPos = globalWM->getClickedVertex(cameraPos, front);

            ChangeWall(globalWM, clickedPos, 0.2f);
            std::cout << "Click position: (" << clickedPos.x << ", " << clickedPos.y << ", " << clickedPos.z << ")\n";
        }
    }
}


void renderCrosshair(shader_prog crosshairShader, int windowWidth, int windowHeight) {
    // Crosshair vertices (center of screen)
    GLfloat crosshairVertices[] = {
        -0.02f,  0.0f,  1.0f, 0.0f, 0.0f,  // Left horizontal line (Red)
         0.02f,  0.0f,  1.0f, 0.0f, 0.0f,  // Right horizontal line (Red)
         0.0f,   0.02f,  1.0f, 0.0f, 0.0f,  // Top vertical line (Red)
         0.0f,  -0.02f,  1.0f, 0.0f, 0.0f   // Bottom vertical line (Red)
    };
    // Create buffers for crosshair
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    // Position attribute (location 0 in shader)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1 in shader)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Use the crosshair shader program
    crosshairShader.use();

    // Draw 4 vertices (two lines)
    glDrawArrays(GL_LINES, 0, 4);

    // Clean up (delete buffers)
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
