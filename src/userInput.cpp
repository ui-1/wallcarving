// userInput.cpp
#include "userInput.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 15.0f); // Initialize camera position
glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f); // Initial front direction

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

    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    // so that cam cant get flipped upside down
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Calculate new front vector
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front); // Normalize to ensure consistent direction
}
