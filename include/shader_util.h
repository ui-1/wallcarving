/**
 * MTAT.03.015 Computer Graphics.
 * Shader configuration utility routines.
 */
#pragma once
#include <string>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

/**
 * Modified version of code from:
 *  http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
 */
class shader_prog {
private:
    GLuint vertex_shader, fragment_shader, prog;
    std::string v_source, f_source;
    std::string v_filename, f_filename;
public:
    shader_prog(const char* vertex_shader_filename, const char* fragment_shader_filename);
    shader_prog(std::string vertex_shader_str, std::string fragment_shader_str);
    void use();
    void free();
    operator GLuint();

    // Shorthands for glUniform specification
    void uniform1i(const char* name, int i);
    void uniform1f(const char* name, float f);
    void uniform3f(const char* name, float x, float y, float z);
    void uniformMatrix4fv(const char* name, const float* matrix);
    void uniformMatrix4fv(const char* name, glm::mat4 matrix);
    void attribute3fv(const char* name, GLfloat* vecArray, int numberOfVertices);
    std::string getVfilename();
    std::string getFfilename();
};
