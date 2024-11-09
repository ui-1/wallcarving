#pragma once

#include <vector>
#include <GL/glew.h>
#include "glm/glm.hpp"

#include "shader_util.h"

#define EM_SIZE 100

class WallMatrix {
    public:
        WallMatrix(shader_prog& shader);

        // Adds a new vertex (without edges). Returns the index of the created vertex
        int addVertex(glm::vec3 vertex);

        // Sets the vertex at index i
        void setVertex(glm::vec3 vertex, int i);

        // Remove vertex at index i and any edges connected to it
        void removeVertex(int i);

        // Connects vertices i and j if connected == true, disconnects them otherwise
        void setEdge(int i, int j, bool connected);

        // Returns true iff vertices i and j are connected
        bool getEdge(int i, int j);
        
        // Note: this is const to avoid accidentally adding vertices without setting the stale flag;
        //       for adding vertices, use addVertex
        const std::vector<glm::vec3>& getVertices();

        // To be called in the main draw loop
        void drawWall();

        // Print debug information about the wall
        void debugPrint();

    private:
        shader_prog& wallShader;
        
        std::vector<glm::vec3> vertices;

        // Vertices i and j (i < j) are connected iff edges[i][j] == true
        bool edges[EM_SIZE][EM_SIZE];

        // Has anything changed? i.e. should a new VAO be generated
        bool stale;

        GLuint wallVAO;

        GLuint& getWallVAO();
        
        void regenerateVAO();
};
