#include <GL/glew.h>             
#include <GLFW/glfw3.h>             
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "userInput.h"             
#include <iostream> 

glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 15.0f); // Initialize camera position
glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f); // Initial front direction

// Define movement variables
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;
float cameraSpeed = 0.1f; 

static double lastX = 320.0;
static double lastY = 240.0;
static bool firstMouse = true;
static float yaw = -90.0f;
static float pitch = -90.0f;

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
    printf("The number is: %f\n", yaw);

    // so that cam cant get flipped upside down
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Calculate new front vector
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front); // Normalize to ensure consistent direction
}

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

