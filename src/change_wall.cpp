// Oletan, et mul on üks struct, mis sisaldab endas mingil moel seda kolmnurkadest koosnevat pinda, mida minu tehtav funktsioon hakkab mõjutama. Veel oletan, et sellel struktuuril on meetod, mis tagastab std::vector<glm::vec3> nimekirja, mis sisaldab iga tipu koordinaate.
#include <vector>
#include "wall.h"
#include "change_wall.h"
#include "glm/glm.hpp"




/* Idea: 
>a line rests horisotally on screen
>line is sudivided into segments
>effectively subdivided plane looked at from the side
>mouse cursor comes down from the top
>reaches line
>clicks
>a magical rainbow point appears above the point where the click happens
>rainbow point makes a circle around itself
>force field
>some vertices of the line are inside the force field
>some are outside
>every inside vertex gets pushed away from the rainbow point by a fixed distance
>because vertices are moved, some edgelengths get changed.
>edge gets too long: a new vertex appears in the middle of it
>edge gets too short: it collapses and its endpoints merge into one.
>
>do that but in 3d
*/







void ChangeWall(WallMatrix* wall, glm::vec3 clickPos)
{
    // Radius of effect. This radius is applied at a position near the 3d point where the mouse is clicked. Verticies inside should be moved.
    float RoE = 50.0f;
    // The distance by which vertices should move.
    float speed = 0.001f;
    // minimum allowable length for an edge. Edge gets collapsed if its length is less.
    float minLen = 0.0f;
    // maximum allowed length for an edge. Edge gets split: new vertex and some new edges get added.
    float maxLen = 2.0f;


    // Looping over every vertex. If vertex is inside the AoE then it gets moved by speed and away from clickPoint.
    for (unsigned int i = 0; i< wall->getVertices().size(); i++)
    {
        glm::vec3 vertPos = wall->getVertices()[i];
        if (glm::distance(vertPos, clickPos) < RoE)
        {
            glm::vec3 direction = glm::normalize(vertPos-clickPos);
            glm::vec3 newVertPos = vertPos+speed*direction;
            wall->setVertex(newVertPos, i);
        }
    }


    // After moving vertices, now edges need to be taken care of. Too short edges must be collapsed and too long edges need to be split. We go over every possible pair of indices and check if corresponding vertices are connected. If there is an edge connecting them then the length of that edge is found.
    for (unsigned int i1 = 0; i1< wall->getVertices().size(); i1++)
    {
        for (unsigned int i2 = 0; i2 < i1; i2++)
        {
            if (!wall->getEdge(i1, i2))
            {
                continue;
            }

            // if code reaches here then the vertices i1 and i2 have a connecting edge. Find the length of it.
            glm::vec3 v1 = wall->getVertices()[i1];
            glm::vec3 v2 = wall->getVertices()[i2];
            float edgeLen = glm::distance(v1, v2);


            // Note that at this point in the code v1 and v2 are the copies of the vertex coords of verts at indices i1 and i2 respectively.
            if (edgeLen < minLen)
            {
                /* If length of that edge is less than minLen, then the edge collapse procedure is as follows.
                1) pick one vertex of that edge.
                2) move the chosen vertex so that it is in the middle of the edge.
                3) look at the vertex that wasn't moved and form a list of all its neighbors.
                4) delete the vertex that was not moved.
                5) vertex that was moved to the center needs to be connected to all the neighbors of the deleted vertex. */
                
                v1 += (v2-v1)*0.5f;
                wall->setVertex(v1, i1);
                // 1), 2) done. vertex at i1 got chosen and moved.
                
                std::vector<int> inicesOfNeighbors = {};
                for (unsigned int i=0; i<wall->getVertices().size(); i++)
                {   
                    if (i == i2)
                    {
                        continue;
                    }
                    if (wall->getEdge(i2, i))
                    {
                        inicesOfNeighbors.push_back(i);
                    }
                }
                // 3) done.

                wall->removeVertex(i2);
                i2 -= 1;
                // 4) done.

                for (unsigned int i : inicesOfNeighbors)
                {
                    wall->setEdge(i, i1, true);
                }
                // 5) done.
                continue;
            }


            
            
            // TODO: split too long edges in the order of their lengths. The longest too long edges first and shorter too long edges last. Doing it in this order might move vertices around so that some of the too long edges get shorter and therefore would not need to be split. This might result in less triangles and more pleasing geometry.
            if (edgeLen > maxLen)
            {
                /* If an edge is too long then its splitting procedure is as follows.
                1) There are either 1 or 2 triangles adjacent to the too long edge. Make a list of their vertices.
                2) Add a new vertex in the middle of the too long edge.
                3) Delete the too long edge.
                4) Connect the vertices in the list to the added vertex. */
                std::vector<unsigned int> vertlist = {i1, i2};

                for (unsigned int i = 0; i<wall->getVertices().size(); i++)
                {
                    if (i==i1 || i==i2)
                    {
                        continue;
                    }
                    if (wall->getEdge(i, i1) && wall->getEdge(i,i2))
                    {
                        vertlist.push_back(i);
                    }
                }
                printf("vertlist size: %d\n", vertlist.size());
                // 1) done.

                glm::vec3 newVertPos = v1+(v2-v1)*0.5f;
                int i3 = wall->addVertex(newVertPos);
                // 2) done.

                wall->setEdge(i1, i2, false);
                // 3)done.

                for (unsigned int i = 0; i<vertlist.size(); i++)
                {
                    wall->setEdge(vertlist[i], i3, true);
                }
                // 4) done.
            }
        }
    }
}