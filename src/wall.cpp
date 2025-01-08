#include <iostream>
#include <set>
#include <tuple>
#include <stack>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader_util.h"
#include "wall.h"

WallMatrix::WallMatrix(shader_prog& shader): wallShader(shader) {
    for (int i = 0; i < EM_SIZE; i++) {
        for (int j = 0; j < EM_SIZE; j++) {
            edges[i][j] = false;
        }
    }

    vertices = std::vector<glm::vec3>();
    stale = false;
    initializeWall(5, 5);
}

int WallMatrix::addVertex(glm::vec3 vertex) {
    // TODO: handle vertex limit
    vertices.push_back(vertex);
    stale = true;
    return (int) vertices.size() - 1;
}

void WallMatrix::removeVertex(int rem_i) {
    vertices.erase(vertices.begin() + rem_i);
    
    for (int i = rem_i + 1; i < EM_SIZE; i++) {
        for (int j = 1; j < i; j++) {
            edges[i-1][j-1] = edges[i][j];
            edges[j][i] = edges[i][j];
        }
    }
    for (int j = 0; j < EM_SIZE; j++) {
        edges[EM_SIZE-1][j] = false;
        edges[j][EM_SIZE-1] = false;
    }

    stale = true;
}

void WallMatrix::setVertex(glm::vec3 vertex, int i) {
    vertices[i] = vertex;
    stale = true;
}

void WallMatrix::setEdge(int i, int j, bool connected) {
    edges[i][j] = connected;
    edges[j][i] = connected;
    stale = true;
}

bool WallMatrix::getEdge(int i, int j) {
    return edges[i][j];
}

const std::vector<glm::vec3>& WallMatrix::getVertices() {
    return vertices;
}

GLuint& WallMatrix::getWallVAO() {
    if (stale) {
        regenerateVAO();
    }
    return wallVAO;
}

void WallMatrix::regenerateVAO() {

    GLfloat* verticesArray = new GLfloat[vertices.size() * 3];
    for (int i = 0; i < vertices.size(); i++) {
        verticesArray[i*3    ] = vertices[i].x;
        verticesArray[i*3 + 1] = vertices[i].y;
        verticesArray[i*3 + 2] = vertices[i].z;
    }

    // TODO: inefficient!! bad!!!1
    std::set<std::tuple<int, int, int>> triSet;
    for (int i = 0; i < vertices.size(); i++) {
        for (int j = i + 1; j < vertices.size(); j++) {
            for (int k = j + 1; k < vertices.size(); k++) {
                if (getEdge(i, j) && getEdge(j, k) && getEdge(k, i)) {
                    triSet.insert(std::make_tuple(i, j, k));
                }
            }
        }
    }

    // TODO: triangle directions based on player location
    triSetOrdered.clear();
    for (std::tuple<int, int, int> e : triSet) {
        int i1 = std::get<0>(e);
        int i2 = std::get<1>(e);
        int i3 = std::get<2>(e);

        glm::vec3 p1 = vertices[i1];
        glm::vec3 p2 = vertices[i2];
        glm::vec3 p3 = vertices[i3];
        
        glm::vec3 p1p2 = p2 - p1;
        glm::vec3 p1p3 = p3 - p1;

        if (glm::cross(p1p2, p1p3).z >= 0) {
            triSetOrdered.insert(std::make_tuple(i1, i2, i3));
        } else {
            triSetOrdered.insert(std::make_tuple(i1, i3, i2));
        }
    }


    GLubyte* indicesArray = new GLubyte[triSetOrdered.size() * 3];
    int index = 0;
    for (std::tuple<int, int, int> e : triSetOrdered) {
        indicesArray[index++] = std::get<0>(e);
        indicesArray[index++] = std::get<1>(e);
        indicesArray[index++] = std::get<2>(e);
    }

    // TODO: assign colors based on depth or sth?
    //       current values are just placeholders

    GLfloat* colors = new GLfloat[vertices.size() * 3];
    for (int i = 0; i < vertices.size(); i++) {
        float c = (float) (i+1)/vertices.size();
        colors[i*3    ] = c*c;
        colors[i*3 + 1] = c*c;
        colors[i*3 + 2] = c*c;
    }

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);


    wallShader.attribute3fv("position", verticesArray, (int) vertices.size() * 3);
    wallShader.attribute3fv("color", colors, (int) vertices.size() * 3);

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * triSetOrdered.size() * 3, indicesArray, GL_STATIC_DRAW);

    wallVAO = vertexArrayHandle;
    stale = false;

    delete[] verticesArray;
    delete[] indicesArray;
    delete[] colors;
}

void WallMatrix::drawWall() {
    wallShader.uniformMatrix4fv("modelMatrix", glm::mat4(1.0));
    glBindVertexArray(getWallVAO());
    glDrawElements(GL_TRIANGLES, (int) triSetOrdered.size() * 3, GL_UNSIGNED_BYTE, 0);
}

void WallMatrix::debugPrint() {
    std::cout << "\n-------------------- WallMatrix debug --------------------\n\n";
    std::cout << "Wall vertex shader - " << wallShader.getVfilename() << '\n';
    std::cout << "Wall fragment shader - " << wallShader.getFfilename() << '\n';
    std::cout << "\nStale flag: " << stale << '\n';

    std::cout << "\nCurrent vertices:\n";
    for (int i = 0; i < vertices.size(); i++) {
        std::cout << i << ":  (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ")\n";
    }

    std::cout << "\nCurrent edges:\n";
    for (int i = 0; i < vertices.size(); i++) {
        for (int j = i+1; j < vertices.size(); j++) {
            if (getEdge(i, j)) {
                std::cout << "    " << i << " <--> " << j << '\n';
            }
        }
    }

    std::cout << "\n----------------------------------------------------------\n\n";
}

// Forms a grid of vertices in rectangular shape with a edges horizontally and b edges vertically.
void WallMatrix::initializeWall(int a, int b) {
    for (int i_a = 0; i_a <= a; i_a++) {
        for (int i_b = 0; i_b <= b; i_b++) {
            addVertex(glm::vec3(i_a, i_b, 0.0f));
        }
    }

    for (int i_a = 0; i_a <= a; i_a++) {
        for (int i_b = 0; i_b < b; i_b++) {
            setEdge(i_a*(b+1) + i_b, i_a*(b+1) + i_b + 1, true);
        }
    }

    for (int i_b = 0; i_b <= b; i_b++) {
        for (int i_a = 0; i_a < a; i_a++) {
            setEdge(i_a*(b+1) + i_b, (i_a+1)*(b+1) + i_b, true);
        }
    }

    for (int i_a = 1; i_a <= a; i_a++) {
        for (int i_b = 1; i_b <= b; i_b++) {
            setEdge(i_a*(b+1) + i_b - 1, (i_a-1)*(b+1) + i_b, true);
        }
    }
}

glm::vec3 WallMatrix::getClickedVertex(glm::vec3 rayOrigin, glm::vec3 rayDirection) {
    printf("Ray origin: %f, %f, %f\nRay dir: %f,%f, %f,", rayOrigin.x, rayOrigin.y, rayOrigin.z, rayDirection.x, rayDirection.y, rayDirection.z);
    // TODO: goes over all triangles until finding a hit; very bad!!
    std::set<std::tuple<int, int, int>> triSet;
    for (int i = 0; i < vertices.size(); i++) {
        for (int j = i + 1; j < vertices.size(); j++) {
            for (int k = j + 1; k < vertices.size(); k++) {
                if (getEdge(i, j) && getEdge(j, k) && getEdge(k, i)) {
                    triSet.insert(std::make_tuple(i, j, k));
                }
            }
        }
    }

    float closestT = FLT_MAX;
    for (std::tuple<int, int, int> t : triSet) {
        //std::cout << "Current triangle: " << std::get<0>(t) << " " << std::get<1>(t) << " " << std::get<2>(t) << '\n';
    

        // "Translated" from ray chopper task
        // Möller-Trumbore algrorithm
        float epsilon = 0.0001f;
        
        glm::vec3 f = vertices[std::get<1>(t)] - vertices[std::get<0>(t)];
        glm::vec3 e = vertices[std::get<2>(t)] - vertices[std::get<0>(t)];
        
        glm::vec3 q = glm::cross(rayDirection,e);
        float D = glm::dot(f, q);


        if (D < epsilon && D > -epsilon) {      //Ray is almost parallel to  triangle
            std::cout << "Missed" << '\n';
            continue;
        }

        float x = 1.0f/D;
        glm::vec3 B = rayOrigin - vertices[std::get<0>(t)];
        float v = x * glm::dot(B, q);


        if (v < 0.0f || v > 1.0f) {             //outside the triangle
            std::cout << "Missed" << '\n';
            continue;
        }

        glm::vec3 p = glm::cross(B, f);
        float u = x * glm::dot(rayDirection, p);


        if (u < 0.0f || u+v > 1.0f) {          //outside the triangle
            std::cout << "Missed" << '\n';
            continue;
        }

        float t = x*glm::dot(e, p);
        if (t < epsilon) {                      //behind the ray origin
            std::cout << "Missed" << '\n';
            continue;
        }

        if (t < FLT_MAX)        //check that t is not extremely large
        {
            closestT = t;
            return rayOrigin + rayDirection*t;
        }
        

    }

    return glm::vec3(999.0f, 999.0f, 999.0f);
}
