#ifndef USERINPUT_H
#define USERINPUT_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

extern bool moveForward;
extern bool moveBackward;
extern bool moveLeft;
extern bool moveRight;
extern float cameraSpeed;

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

extern glm::vec3 cameraPos;
extern glm::vec3 front;

#endif
