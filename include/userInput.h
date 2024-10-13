#ifndef USERINPUT_H
#define USERINPUT_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);

extern glm::vec3 cameraPos;
extern glm::vec3 front;

#endif
