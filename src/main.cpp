#include <iostream>
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
#include "wall.h"
#include "change_wall.h"

#define CONCAT_PATHS(A, B) A "/" B
#define ADD_ROOT(B) CONCAT_PATHS(TASK_ROOT_PATH, B)

shader_prog shader(
    ADD_ROOT("shaders/chopper.vert.glsl"),
    ADD_ROOT("shaders/chopper.frag.glsl")
);

shader_prog crosshairShader(
    ADD_ROOT("shaders/crosshair.vert.glsl"),
    ADD_ROOT("shaders/crosshair.frag.glsl")
);

int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(1920/2, 1080/2, "evil gnome hits wall with pickaxe", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetCursorPosCallback(win, mouse_position_callback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //hide cursor

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

    glm::mat4 projection = glm::perspective(glm::radians(80.0f), 16.0f / 9.0f, 0.1f, 100.f);

    shader.uniformMatrix4fv("projectionMatrix", projection);

    GLuint floorVAO = initWalls(shader);
    WallMatrix* wm = new WallMatrix(shader);
    globalWM = wm;
    glfwSetMouseButtonCallback(win, mouse_button_callback);

    /*
    // --------------------------------------------------------------
    //             (see wall.h for available methods)
    // example of adding a new vertex
    int i = wm->addVertex(glm::vec3(1.0f, 4.0f, 3.0f));


    // iterate over existing vertices
    std::vector<glm::vec3> verts = wm->getVertices();
    for (glm::vec3 v : verts) {
        // stuff
    }

    // add two edges connected to the new vertex
    wm->setEdge(1, i, true);
    wm->setEdge(2, i, true);

    // move the vertex
    wm->setVertex(glm::vec3(0.8f, 0.3f, -0.2f), i);

    //remove the vertex
    wm->removeVertex(i);

    // print debug information
    wm->debugPrint();

    // --------------------------------------------------------------
    */

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 pos = glm::vec3(2.5f, 2.5f, 1.0f);
    unsigned int  counter = 0;
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

        glEnable(GL_DEPTH_TEST);
        drawFloor(floorVAO, shader);


        
        pos.z -= 0.01f;
        //printf("(%f, %f, %f)", pos.x, pos.y, pos.z);
        if (counter < 100)
        {
            ChangeWall(wm, pos, 0.03);
            counter++;
        }
        wm->drawWall();


        // Render the crosshair in the center of the screen in a different shader
        renderCrosshair(crosshairShader, 1920, 1080);

        //Switch back to the normal shader 
        shader.use();

        //Switch back the projectionMatrix 
        shader.uniformMatrix4fv("projectionMatrix", projection);

        glfwSwapBuffers(win);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
