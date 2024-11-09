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

    // Initial test wall setup
    addVertex(glm::vec3(-1.0f,  -1.0f, 0.0f));
    addVertex(glm::vec3( 0.0f,  -1.0f, 0.0f));
    addVertex(glm::vec3( 0.0f,   1.0f, 0.0f));
    addVertex(glm::vec3(-1.0f,   1.0f, 0.0f));

    setEdge(0, 1, true);
    setEdge(1, 2, true);
    setEdge(2, 3, true);
    setEdge(3, 0, true);
    setEdge(0, 2, true);
    
    regenerateVAO();
}

int WallMatrix::addVertex(glm::vec3 vertex) {
    // TODO: vertex limit
    vertices.push_back(vertex);
    stale = true;
    return (int) vertices.size() - 1;
}

void WallMatrix::removeVertex(int i) {
    vertices.erase(vertices.begin() + i);
    for (int j = 0; j < vertices.size(); j++) {
        edges[i][j] = false;
        edges[j][i] = false;
    }
    stale = true;
}

void WallMatrix::setVertex(glm::vec3 vertex, int i) {
    vertices[i] = vertex;
    stale = true;
}

void WallMatrix::setEdge(int i, int j, bool connected) {
    if (i < j) {
        edges[i][j] = connected;
    } else {
        edges[j][i] = connected;
    }
    stale = true;
}

bool WallMatrix::getEdge(int i, int j) {
    if (i < j) {
        return edges[i][j];
    } else {
        return edges[j][i];
    }
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
    std::set<std::tuple<int, int, int>> triSetOrdered;
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

    GLubyte* indicesArray = new GLubyte[vertices.size() * 3];
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
        colors[i*3    ] = c;
        colors[i*3 + 1] = c;
        colors[i*3 + 2] = c;
    }

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    wallShader.attribute3fv("position", verticesArray, (int) vertices.size() * 3);
    wallShader.attribute3fv("color", colors, (int) vertices.size() * 3);

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() * 3, indicesArray, GL_STATIC_DRAW);

    wallVAO = vertexArrayHandle;
    stale = false;
}

void WallMatrix::drawWall() {
    wallShader.uniformMatrix4fv("modelMatrix", glm::mat4(1.0));
    glBindVertexArray(getWallVAO());
    glDrawElements(GL_TRIANGLES, (int) vertices.size() * 3, GL_UNSIGNED_BYTE, 0);
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
