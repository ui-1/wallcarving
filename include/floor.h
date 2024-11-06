#pragma once
#include "shader_util.h"

GLuint initWalls(shader_prog shader);
GLuint createWall(glm::vec3 color, float s, shader_prog shader);
void drawFloor(GLuint floorVAO, shader_prog shader);