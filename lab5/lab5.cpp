#include "lab_m1/lab5/lab5.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;
float fov, ortoD;
float angle = 0.0f;

glm::vec3 dronePosition;
float droneRotation;
int collectedPackets = 0;
vector<glm::vec3> positionsTree;
vector<glm::vec3> positionsCladiri;
bool firstTime = true;
bool firstBloc = true;
float dayNightCycle = 0.0f; 
bool isDaytime = true;


Lab5::Lab5()
{
}

Lab5::~Lab5()
{
}

glm::vec3 generateRandomPosition()
{
    float x = (float)(rand() % 50 - 25); 
    float z = (float)(rand() % 50 - 25); 
    return glm::vec3(x, 0.5f, z);        
}

void Lab5::Init()
{

    packet.position = generateRandomPosition();
    packet.collected = false;
    dronePosition = glm::vec3(0, 2, 0); // start
    droneRotation = 0.0f;

    orthoCamera = new implemented::Camera();
    orthoCamera->Set(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 3, 7), glm::vec3(3, 3, 0), glm::vec3(0, 1, 0));

    ///
    fov = 50;
    ortoD = 10;
    ///
    
    //destinatie
    {
        float radius = 1.0f;
        int numSegments = 100;
        vector<VertexFormat> vertices;
        vector<unsigned int> indices;

        vertices.push_back(VertexFormat(glm::vec3(0, 1, 0), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1)));

        for (int i = 0; i <= numSegments; ++i)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = radius * cos(angle);
            float z = radius * sin(angle);
            vertices.push_back(VertexFormat(glm::vec3(x, 1, z), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1)));
        }

        for (int i = 1; i <= numSegments; ++i)
        {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        meshes["destination_circle"] = new Mesh("destination_circle");
        meshes["destination_circle"]->InitFromData(vertices, indices);
    }

    //sageata
    {
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(0.0, 0, 0.5), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1)),  
            VertexFormat(glm::vec3(-0.1, 0, 0.0), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1)),
            VertexFormat(glm::vec3(0.1, 0, 0.0), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1)),  
            VertexFormat(glm::vec3(0.0, 0, -0.1), glm::vec3(1, 0, 1), glm::vec3(1, 0, 1))  
        };

        vector<unsigned int> indices = {
            0, 1, 2, // Front face
            0, 2, 3, // Right side
            0, 3, 1  // Left side
        };

        meshes["direction_arrow"] = new Mesh("direction_arrow");
        meshes["direction_arrow"]->InitFromData(vertices, indices);
    }

    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab5", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab5", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    //pachet
    {
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 1)),
            VertexFormat(glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 1)),
            VertexFormat(glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 1)),
            VertexFormat(glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 1)),
            VertexFormat(glm::vec3(0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec3(0, 1, 1)),
            VertexFormat(glm::vec3(0.5, -0.5, 0.5), glm::vec3(1, 0, 0), glm::vec3(0, 1, 1)),
            VertexFormat(glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 0, 0), glm::vec3(0, 1, 1)),
            VertexFormat(glm::vec3(0.5, 0.5, -0.5), glm::vec3(1, 0, 0), glm::vec3(0, 1, 1))};

        vector<unsigned int> indices = {
            0, 1, 2, 0, 2, 3,       // Fața din față
            4, 6, 5, 4, 7, 6,       // Fața din spate
            8, 9, 10, 8, 10, 11,    // Fața de sus
            12, 14, 13, 12, 15, 14, // Fața de jos
            16, 17, 18, 16, 18, 19, // Fața din stânga
            20, 22, 21, 20, 23, 22  // Fața din dreapta
        };

        meshes["pachet"] = new Mesh("pachet");
        meshes["pachet"]->InitFromData(vertices, indices);
    }

    //cladire
    {
        vector<VertexFormat> vertices{
            VertexFormat(glm::vec3(-1.0f, 0.0f, 1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)),  // 0
            VertexFormat(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)),   // 1
            VertexFormat(glm::vec3(-1.0f, 4.0f, 1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)),  // 2
            VertexFormat(glm::vec3(1.0f, 4.0f, 1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)),   // 3
            VertexFormat(glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)), // 4
            VertexFormat(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)),  // 5
            VertexFormat(glm::vec3(-1.0f, 4.0f, -1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)), // 6
            VertexFormat(glm::vec3(1.0f, 4.0f, -1.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.2f, 0.8f, 0.6f)),  // 7
        };

        vector<unsigned int> indices =
            {
                0, 1, 2, // Triangle 1
                1, 3, 2, // Triangle 2
                2, 3, 7, // Triangle 3
                2, 7, 6, // Triangle 4
                1, 7, 3, // Triangle 5
                1, 5, 7, // Triangle 6
                6, 7, 4, // Triangle 7
                7, 5, 4, // Triangle 8
                0, 4, 1, // Triangle 9
                1, 4, 5, // Triangle 10
                2, 6, 4, // Triangle 11
                0, 2, 4  // Triangle 12
            };

        meshes["cladire"] = new Mesh("cladire");
        meshes["cladire"]->InitFromData(vertices, indices);
    }

    // copac
    {
        const float trunkRadius = 0.15f;
        const float trunkHeight = 1.6f;
        const float crownRadius1 = 0.6f;
        const float crownHeight1 = 0.8f;
        const float crownRadius2 = 0.6f;
        const float crownHeight2 = 0.8f;
        const int numSegments = 100;

        vector<VertexFormat> vertices;
        vector<unsigned int> indices;

        int baseIndexCyl = vertices.size();
        vertices.push_back(VertexFormat(glm::vec3(0, 0, 0),
                                        glm::vec3(0.3f, 0.2f, 0.1f), glm::vec3(0.2, 0.8, 0.6)));

        for (int i = 0; i <= numSegments; ++i)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = trunkRadius * cos(angle);
            float z = trunkRadius * sin(angle);
            vertices.push_back(VertexFormat(glm::vec3(x, 0, z),
                                            glm::vec3(0.4f, 0.3f, 0.2f), glm::vec3(0.2, 0.8, 0.6)));
        }

        vertices.push_back(VertexFormat(glm::vec3(0, trunkHeight, 0),
                                        glm::vec3(0.3f, 0.2f, 0.1f), glm::vec3(0.2, 0.8, 0.6)));
        for (int i = 0; i <= numSegments; ++i)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = trunkRadius * cos(angle);
            float z = trunkRadius * sin(angle);
            vertices.push_back(VertexFormat(glm::vec3(x, trunkHeight, z),
                                            glm::vec3(0.4f, 0.3f, 0.2f), glm::vec3(0.2, 0.8, 0.6)));
        }

        for (int i = 1; i <= numSegments; ++i)
        {
            indices.push_back(baseIndexCyl);
            indices.push_back(baseIndexCyl + i);
            indices.push_back(baseIndexCyl + i + 1);
        }

        int offsetCylTop = baseIndexCyl + numSegments + 2;
        for (int i = 1; i <= numSegments; ++i)
        {
            indices.push_back(offsetCylTop);
            indices.push_back(offsetCylTop + i);
            indices.push_back(offsetCylTop + i + 1);
        }

        for (int i = 1; i <= numSegments; ++i)
        {
            int next = (i % numSegments) + 1;
            indices.push_back(baseIndexCyl + i);
            indices.push_back(baseIndexCyl + next);
            indices.push_back(offsetCylTop + i);

            indices.push_back(baseIndexCyl + next);
            indices.push_back(offsetCylTop + next);
            indices.push_back(offsetCylTop + i);
        }

        int baseIndexCone1 = vertices.size();
        vertices.push_back(VertexFormat(glm::vec3(0, trunkHeight, 0),
                                        glm::vec3(0.1f, 0.5f, 0.1f), glm::vec3(0.2, 0.8, 0.6))); // Base center

        for (int i = 0; i < numSegments; ++i)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = crownRadius1 * cos(angle);
            float z = crownRadius1 * sin(angle);
            vertices.push_back(VertexFormat(glm::vec3(x, trunkHeight, z),
                                            glm::vec3(0.2f, 0.6f, 0.2f), glm::vec3(0.2, 0.8, 0.6)));
        }

        vertices.push_back(VertexFormat(glm::vec3(0, trunkHeight + crownHeight1, 0),
                                        glm::vec3(0.1f, 0.5f, 0.1f), glm::vec3(0.2, 0.8, 0.6)));

        for (int i = 1; i < numSegments; ++i)
        {
            indices.push_back(baseIndexCone1);
            indices.push_back(baseIndexCone1 + i);
            indices.push_back(baseIndexCone1 + i + 1);
        }
        indices.push_back(baseIndexCone1);
        indices.push_back(baseIndexCone1 + numSegments);
        indices.push_back(baseIndexCone1 + 1);

        int tipIndexCone1 = baseIndexCone1 + numSegments + 1;
        for (int i = 1; i < numSegments; ++i)
        {
            indices.push_back(tipIndexCone1);
            indices.push_back(baseIndexCone1 + i);
            indices.push_back(baseIndexCone1 + i + 1);
        }
        indices.push_back(tipIndexCone1);
        indices.push_back(baseIndexCone1 + numSegments);
        indices.push_back(baseIndexCone1 + 1);

        int baseIndexCone2 = vertices.size();
        float cone2BaseHeight = trunkHeight + 0.4f;
        vertices.push_back(VertexFormat(glm::vec3(0, cone2BaseHeight, 0),
                                        glm::vec3(0.1f, 0.5f, 0.1f), glm::vec3(0.2, 0.8, 0.6)));

        for (int i = 0; i < numSegments; ++i)
        {
            float angle = 2.0f * M_PI * i / numSegments;
            float x = crownRadius2 * cos(angle);
            float z = crownRadius2 * sin(angle);
            vertices.push_back(VertexFormat(glm::vec3(x, cone2BaseHeight, z),
                                            glm::vec3(0.2f, 0.6f, 0.2f), glm::vec3(0.2, 0.8, 0.6)));
        }

        vertices.push_back(VertexFormat(glm::vec3(0, cone2BaseHeight + crownHeight2, 0),
                                        glm::vec3(0.1f, 0.5f, 0.1f), glm::vec3(0.2, 0.8, 0.6)));
        for (int i = 1; i < numSegments; ++i)
        {
            indices.push_back(baseIndexCone2);
            indices.push_back(baseIndexCone2 + i);
            indices.push_back(baseIndexCone2 + i + 1);
        }
        indices.push_back(baseIndexCone2);
        indices.push_back(baseIndexCone2 + numSegments);
        indices.push_back(baseIndexCone2 + 1);

        int tipIndexCone2 = baseIndexCone2 + numSegments + 1;
        for (int i = 1; i < numSegments; ++i)
        {
            indices.push_back(tipIndexCone2);
            indices.push_back(baseIndexCone2 + i);
            indices.push_back(baseIndexCone2 + i + 1);
        }
        indices.push_back(tipIndexCone2);
        indices.push_back(baseIndexCone2 + numSegments);
        indices.push_back(baseIndexCone2 + 1);

        meshes["copac"] = new Mesh("generated_tree");
        meshes["copac"]->InitFromData(vertices, indices);
    }

    //teren
    {
        vector<VertexFormat> vertices;
        vector<unsigned int> indices;

        int gridSize = 100;    // Numărul de vârfuri pe fiecare latură
        float cellSize = 1.0f; // Dimensiunea fiecărei celule din grilă

        for (int z = 0; z < gridSize; z++)
        {
            for (int x = 0; x < gridSize; x++)
            {
                float xPos = (x - gridSize / 2) * cellSize; 
                float zPos = (z - gridSize / 2) * cellSize; 

                vertices.push_back(VertexFormat(
                    glm::vec3(xPos, 0.0f, zPos), 
                    glm::vec3(0.5f),            
                    glm::vec3(0.5f)           
                    ));
            }
        }

        for (int z = 0; z < gridSize - 1; z++)
        {
            for (int x = 0; x < gridSize - 1; x++)
            {
                int topLeft = z * gridSize + x;
                int topRight = topLeft + 1;
                int bottomLeft = (z + 1) * gridSize + x;
                int bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        meshes["terrain"] = new Mesh("terrain");
        meshes["terrain"]->InitFromData(vertices, indices);
    }

    //paralelipiped_1
    {
        vector<VertexFormat> vertices{
            // Primul paralelipiped (paralel cu OX)
            VertexFormat(glm::vec3(-6, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)), // 0
            VertexFormat(glm::vec3(6, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),  // 1
            VertexFormat(glm::vec3(-6, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),  // 2
            VertexFormat(glm::vec3(6, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),   // 3
            VertexFormat(glm::vec3(-6, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),  // 4
            VertexFormat(glm::vec3(6, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),   // 5
            VertexFormat(glm::vec3(-6, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),   // 6
            VertexFormat(glm::vec3(6, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),    // 7

            // Primul cub (stânga-față)
            VertexFormat(glm::vec3(-6, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),   // 8
            VertexFormat(glm::vec3(-4.5, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)), // 9
            VertexFormat(glm::vec3(-6, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),    // 10
            VertexFormat(glm::vec3(-4.5, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),  // 11
            VertexFormat(glm::vec3(-6, 1.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),   // 12
            VertexFormat(glm::vec3(-4.5, 1.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)), // 13
            VertexFormat(glm::vec3(-6, 1.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),    // 14
            VertexFormat(glm::vec3(-4.5, 1.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),  // 15

            // Al doilea cub (dreapta-față)
            VertexFormat(glm::vec3(4.5, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)), // 16
            VertexFormat(glm::vec3(6, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),   // 17
            VertexFormat(glm::vec3(4.5, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),  // 18
            VertexFormat(glm::vec3(6, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),    // 19
            VertexFormat(glm::vec3(4.5, 1.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)), // 20
            VertexFormat(glm::vec3(6, 1.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),   // 21
            VertexFormat(glm::vec3(4.5, 1.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),  // 22
            VertexFormat(glm::vec3(6, 1.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8))     // 23
        };

        vector<unsigned int> indices = {
            0, 1, 2, 1, 3, 2, // Fața frontală
            4, 6, 5, 5, 6, 7, // Fața din spate
            0, 2, 4, 4, 2, 6, // Fața stângă
            1, 5, 3, 3, 5, 7, // Fața dreaptă
            2, 3, 6, 3, 7, 6, // Fața de sus
            0, 4, 1, 1, 4, 5, // Fața de jos

            8, 9, 12, 9, 13, 12,    // Fața frontală
            10, 14, 11, 11, 14, 15, // Fața din spate
            8, 12, 10, 10, 12, 14,  // Fața stângă
            9, 11, 13, 13, 11, 15,  // Fața dreaptă
            12, 13, 14, 13, 15, 14, // Fața de sus

            16, 17, 20, 17, 21, 20, // Fața frontală
            18, 22, 19, 19, 22, 23, // Fața din spate
            16, 20, 18, 18, 20, 22, // Fața stângă
            17, 19, 21, 21, 19, 23, // Fața dreaptă
            20, 21, 22, 21, 23, 22  // Fața de sus

        };

        meshes["paralelipiped_1"] = new Mesh("parallelipiped");
        meshes["paralelipiped_1"]->InitFromData(vertices, indices);
    }

    //paralelipiped_2
    {
        vector<VertexFormat> vertices{
            // Al doilea paralelipiped (paralel cu OZ)
            VertexFormat(glm::vec3(-0.5f, -0.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)), // 0
            VertexFormat(glm::vec3(0.5f, -0.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),  // 1
            VertexFormat(glm::vec3(-0.5f, 0.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),  // 2
            VertexFormat(glm::vec3(0.5f, 0.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),   // 3
            VertexFormat(glm::vec3(-0.5f, -0.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),  // 4
            VertexFormat(glm::vec3(0.5f, -0.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),   // 5
            VertexFormat(glm::vec3(-0.5f, 0.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),   // 6
            VertexFormat(glm::vec3(0.5f, 0.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.8, 0.6)),    // 7

            // Primul cub (stânga-față)
            VertexFormat(glm::vec3(-0.5f, 0.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),    // 8
            VertexFormat(glm::vec3(0.5f, 0.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),     // 9
            VertexFormat(glm::vec3(-0.5f, 0.5f, -4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)), // 10
            VertexFormat(glm::vec3(0.5f, 0.5f, -4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),  // 11
            VertexFormat(glm::vec3(-0.5f, 1.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),    // 12
            VertexFormat(glm::vec3(0.5f, 1.5f, -6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),     // 13
            VertexFormat(glm::vec3(-0.5f, 1.5f, -4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)), // 14
            VertexFormat(glm::vec3(0.5f, 1.5f, -4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8, 0.2, 0.6)),  // 15

            // Al doilea cub (dreapta-față)
            VertexFormat(glm::vec3(-0.5f, 0.5f, 4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)), // 16
            VertexFormat(glm::vec3(0.5f, 0.5f, 4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),  // 17
            VertexFormat(glm::vec3(-0.5f, 0.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),    // 18
            VertexFormat(glm::vec3(0.5f, 0.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),     // 19
            VertexFormat(glm::vec3(-0.5f, 1.5f, 4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)), // 20
            VertexFormat(glm::vec3(0.5f, 1.5f, 4.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),  // 21
            VertexFormat(glm::vec3(-0.5f, 1.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8)),    // 22
            VertexFormat(glm::vec3(0.5f, 1.5f, 6), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.2, 0.2, 0.8))      // 23

        };

        vector<unsigned int> indices = {
            0, 1, 2, 1, 3, 2, // Fața frontală
            4, 6, 5, 5, 6, 7, // Fața din spate
            0, 2, 4, 4, 2, 6, // Fața stângă
            1, 5, 3, 3, 5, 7, // Fața dreaptă
            2, 3, 6, 3, 7, 6, // Fața de sus
            0, 4, 1, 1, 4, 5, // Fața de jos

            // Primul cub (stânga-față)
            8, 9, 12, 9, 13, 12,    // Fața frontală
            10, 14, 11, 11, 14, 15, // Fața din spate
            8, 12, 10, 10, 12, 14,  // Fața stângă
            9, 11, 13, 13, 11, 15,  // Fața dreaptă
            12, 13, 14, 13, 15, 14, // Fața de sus
            8, 10, 9, 9, 10, 11,    // Fața de jos (adăugată)

            // Al doilea cub (dreapta-față)
            16, 17, 20, 17, 21, 20, // Fața frontală
            18, 22, 19, 19, 22, 23, // Fața din spate
            16, 20, 18, 18, 20, 22, // Fața stângă
            17, 19, 21, 21, 19, 23, // Fața dreaptă
            20, 21, 22, 21, 23, 22, // Fața de sus
            16, 18, 17, 17, 18, 19  // Fața de jos

        };

        meshes["paralelipiped_2"] = new Mesh("parallelipiped");
        meshes["paralelipiped_2"]->InitFromData(vertices, indices);
    }

    //elice
    {
        vector<VertexFormat> vertices{
            // Primul paralelipiped (paralel cu OX)
            VertexFormat(glm::vec3(-6, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6)), // 0
            VertexFormat(glm::vec3(6, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6)),  // 1
            VertexFormat(glm::vec3(-6, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6)),  // 2
            VertexFormat(glm::vec3(6, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6)),   // 3
            VertexFormat(glm::vec3(-6, -0.5f, 0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6)),  // 4
            VertexFormat(glm::vec3(6, -0.5f, 0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6)),   // 5
            VertexFormat(glm::vec3(-6, 0.5f, 0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6)),   // 6
            VertexFormat(glm::vec3(6, 0.5f, 0.5f), glm::vec3(0, 0, 0), glm::vec3(0.2, 0.8, 0.6))     // 7

        };

        vector<unsigned int> indices = {
            0, 1, 2, 1, 3, 2, // Fața frontală
            4, 6, 5, 5, 6, 7, // Fața din spate
            0, 2, 4, 4, 2, 6, // Fața stângă
            1, 5, 3, 3, 5, 7, // Fața dreaptă
            2, 3, 6, 3, 7, 6, // Fața de sus
            0, 4, 1, 1, 4, 5, // Fața de jos

        };

        meshes["elice"] = new Mesh("elice");
        meshes["elice"]->InitFromData(vertices, indices);
    }

      // soare
    {
    const int sectors = 30;
    const int stacks = 30;
    vector<VertexFormat> sunVertices;
    vector<unsigned int> sunIndices;

    for (int y = 0; y <= stacks; ++y) {
        float v = (float)y / stacks;
        float phi = v * M_PI;

        for (int x = 0; x <= sectors; ++x) {
            float u = (float)x / sectors;
            float theta = u * 2 * M_PI;

            float sx = sin(phi) * cos(theta);
            float sy = cos(phi);
            float sz = sin(phi) * sin(theta);

            sunVertices.push_back(VertexFormat(
                glm::vec3(sx, sy, sz),
                glm::vec3(1.0f, 1.0f, 0.0f), 
                glm::vec3(1.0f, 1.0f, 0.0f)
            ));
        }
    }

    for (int y = 0; y < stacks; ++y) {
        for (int x = 0; x < sectors; ++x) {
            int first = y * (sectors + 1) + x;
            int second = first + sectors + 1;

            sunIndices.push_back(first);
            sunIndices.push_back(second);
            sunIndices.push_back(first + 1);

            sunIndices.push_back(second);
            sunIndices.push_back(second + 1);
            sunIndices.push_back(first + 1);
        }
    }

    meshes["sun"] = new Mesh("sun_sphere");
    meshes["sun"]->InitFromData(sunVertices, sunIndices);

   //luna
    vector<VertexFormat> moonVertices;
    vector<unsigned int> moonIndices;

    for (int y = 0; y <= stacks; ++y) {
        float v = (float)y / stacks;
        float phi = v * M_PI;

        for (int x = 0; x <= sectors; ++x) {
            float u = (float)x / sectors;
            float theta = u * 2 * M_PI;

            float sx = sin(phi) * cos(theta);
            float sy = cos(phi);
            float sz = sin(phi) * sin(theta);

            moonVertices.push_back(VertexFormat(
                glm::vec3(sx, sy, sz),
                glm::vec3(0.8f, 0.8f, 0.9f), 
                glm::vec3(0.8f, 0.8f, 0.9f)
            ));
        }
    }

    for (int y = 0; y < stacks; ++y) {
        for (int x = 0; x < sectors; ++x) {
            int first = y * (sectors + 1) + x;
            int second = first + sectors + 1;

            moonIndices.push_back(first);
            moonIndices.push_back(second);
            moonIndices.push_back(first + 1);

            moonIndices.push_back(second);
            moonIndices.push_back(second + 1);
            moonIndices.push_back(first + 1);
        }
    }

    meshes["moon"] = new Mesh("moon_sphere");
    meshes["moon"]->InitFromData(moonVertices, moonIndices);
}

    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    projectionMatrixOrto = glm::ortho(-36.0f, 36.0f, -36.0f, 36.0f, 0.01f, 200.0f);
}

void genrateTreePositions(int nr_Tree, vector<glm::vec3> &poz)
{
    for (int i = 0; i < nr_Tree; i++)
    {
        bool validPosition = false;
        glm::vec3 newPosition;

        while (!validPosition)
        {
            float x = (rand() % 50) - 20;
            float z = (rand() % 50) - 20;
            newPosition = glm::vec3(x, 0, z);

            validPosition = true;
            if ((newPosition.x >= -2 && newPosition.x <= 2) && (newPosition.z >= -2 && newPosition.z <= 2))
            {
                validPosition = false;
                continue;
            }
            for (const auto &pos : poz)
            {
                if (glm::distance(newPosition, pos) < 2.0f)
                {
                    validPosition = false;
                    break;
                }
            }
        }

        poz.push_back(newPosition);
    }
    positionsTree = poz;
}

void genretBlocPositions(int nr_Block, vector<glm::vec3> &poz, const vector<glm::vec3> &positionsTree)
{
    for (int i = 0; i < nr_Block; i++)
    {
        bool validPosition = false;
        glm::vec3 newPosition;

        while (!validPosition)
        {
            float x = (rand() % 50) - 20;
            float z = (rand() % 50) - 20;
            newPosition = glm::vec3(x, 0, z);

            validPosition = true;
            if ((newPosition.x >= -2 && newPosition.x <= 2) && (newPosition.z >= -2 && newPosition.z <= 2))
            {
                validPosition = false;
                continue;
            }
            for (const auto &pos : poz)
            {
                if (glm::distance(newPosition, pos) < 3.0f)
                {
                    validPosition = false;
                    break;
                }
            }
            for (const auto &pos : positionsTree)
            {
                if (glm::distance(newPosition, pos) < 3.0f)
                {
                    validPosition = false;
                    break;
                }
            }
        }

        poz.push_back(newPosition);
    }
    positionsCladiri = poz;
}

void Lab5::handleCollisions(float deltaTime, glm::vec3 &newPosition)
{
    glm::vec3 oldPosition = dronePosition;

    // coliziunea cu solul
    if (newPosition.y < 1)
        newPosition.y = 1;

    // coliziunile pe fiecare axă separat
    glm::vec3 testPosition = oldPosition;
    bool collisionX = false;
    bool collisionZ = false;

    testPosition.x = newPosition.x;
    for (const auto &treePos : positionsTree)
    {
        if (checkCollisionWithObject(testPosition, treePos, 1.0f))
        {
            collisionX = true;
            break;
        }
    }
    for (const auto &buildingPos : positionsCladiri)
    {
        if (checkCollisionWithObject(testPosition, buildingPos, 2.0f))
        {
            collisionX = true;
            break;
        }
    }

    testPosition = oldPosition;
    testPosition.z = newPosition.z;
    for (const auto &treePos : positionsTree)
    {
        if (checkCollisionWithObject(testPosition, treePos, 1.0f))
        {
            collisionZ = true;
            break;
        }
    }
    for (const auto &buildingPos : positionsCladiri)
    {
        if (checkCollisionWithObject(testPosition, buildingPos, 2.0f))
        {
            collisionZ = true;
            break;
        }
    }

    if (collisionX)
        newPosition.x = oldPosition.x;
    if (collisionZ)
        newPosition.z = oldPosition.z;
}

bool Lab5::checkCollisionWithObject(const glm::vec3 &dronePos, const glm::vec3 &objPos, float objRadius)
{
    float distance = glm::length(glm::vec2(dronePos.x - objPos.x, dronePos.z - objPos.z));
    float droneRadius = 1.5f;
    return distance < (droneRadius + objRadius);
}

void Lab5::FrameStart()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Lab5::RenderScene()
{
    bool isMinimapView = (projectionMatrix == projectionMatrixOrto);

    //MINIMAP
    if (isMinimapView)
    {   
        //teren
        glm::mat4 modelMatrix = glm::mat4(1);
        RenderMesh2(meshes["terrain"], shaders["LabShader"], modelMatrix);

        // drona
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
        RenderMesh2(meshes["paralelipiped_1"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
        RenderMesh2(meshes["paralelipiped_2"], shaders["VertexColor"], modelMatrix);

        // copaci
        for (const auto &pos : positionsTree)
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, pos);
            RenderMesh2(meshes["copac"], shaders["VertexColor"], modelMatrix);
        }

        // clădiri
        for (const auto &pos : positionsCladiri)
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, pos);
            RenderMesh2(meshes["cladire"], shaders["VertexColor"], modelMatrix);
        }

        // pachet necolectat
        if (!packet.collected)
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(packet.position.x, 0, packet.position.z));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
            RenderMesh2(meshes["pachet"], shaders["VertexColor"], modelMatrix);
        } else { //dupa colectare pachet
            // destinatie
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(destination.position.x, 0, destination.position.z));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
                RenderMesh2(meshes["destination_circle"], shaders["VertexColor"], modelMatrix);
            }

            // sageata
            {
                glm::vec3 directionToDestination = glm::normalize(destination.position - dronePosition);
                float angleToDestination = atan2(directionToDestination.x, directionToDestination.z);
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(3, 0, 0)); // Slightly above drone
                modelMatrix = glm::rotate(modelMatrix, angleToDestination, glm::vec3(0, 1, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(4)); // Make arrow more visible
                RenderMesh2(meshes["direction_arrow"], shaders["VertexColor"], modelMatrix);
            }
        }
    }
    //SCENA principala
    else{
        //soare luna
        {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(100,4, 0));
        
        if (isDaytime) {
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));
            RenderMesh(meshes["sun"], shaders["VertexColor"], modelMatrix);
        } else {
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
            RenderMesh(meshes["moon"], shaders["VertexColor"], modelMatrix);
        }
    }
        //  teren
        {
            glm::mat4 modelMatrix = glm::mat4(1);
            RenderMesh(meshes["terrain"], shaders["LabShader"], modelMatrix);
        }

        // copaci
        {
            if (firstTime)
            {
                genrateTreePositions(5, positionsTree);
                firstTime = false;
            }
            for (const auto &pos : positionsTree)
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, pos);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
                RenderMesh(meshes["copac"], shaders["VertexColor"], modelMatrix);
            }
        }

        // clădiri
        {
            if (firstBloc)
            {
                genretBlocPositions(5, positionsCladiri, positionsTree);
                firstBloc = false;
            }
            for (const auto &pos : positionsCladiri)
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, pos);
                RenderMesh(meshes["cladire"], shaders["VertexColor"], modelMatrix);
            }
        }

        // pachet necolectat
        if (!packet.collected)
        {
            // Pachetul este încă în poziția inițială
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, packet.position + glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
            RenderMesh(meshes["pachet"], shaders["VertexColor"], modelMatrix);

            // drona
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                RenderMesh(meshes["paralelipiped_1"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition);
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                RenderMesh(meshes["paralelipiped_2"], shaders["VertexColor"], modelMatrix);
            }

            // elice
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition);                                // Poziționare dronă
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0)); // Rotire dronă
                modelMatrix = glm::translate(modelMatrix, glm::vec3(1.3f, 0.4f, 0));                     // Poziționare elice relativă la dronă
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));                                 // Scalare elice
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));         // Rotire proprie elice
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition);
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.7f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition);
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.3f, 0.4f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition);
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.4f, 2.2f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }
        }
        //AM COLECTAT PACHET
        else {   
            // sageata
            {
                glm::vec3 directionToDestination = glm::normalize(destination.position - dronePosition);
                float angleToDestination = atan2(directionToDestination.x, directionToDestination.z);
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 2, 0)); // Slightly above drone
                modelMatrix = glm::rotate(modelMatrix, angleToDestination, glm::vec3(0, 1, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f)); // Make arrow more visible
                RenderMesh(meshes["direction_arrow"], shaders["VertexColor"], modelMatrix);
            }
            // destinatie
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(destination.position.x, 0, destination.position.z));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
                RenderMesh(meshes["destination_circle"], shaders["VertexColor"], modelMatrix);
            }

            // drona
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1.3f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                RenderMesh(meshes["paralelipiped_1"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1.3f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                RenderMesh(meshes["paralelipiped_2"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1.3f, 0));        // Poziționare dronă
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0)); // Rotire dronă
                modelMatrix = glm::translate(modelMatrix, glm::vec3(1.3f, 0.4f, 0));                     // Poziționare elice relativă la dronă
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));                                 // Scalare elice
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));         // Rotire proprie elice
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1.3f, 0));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.5f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1.3f, 0));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.3f, 0.4f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 1.3f, 0));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.4f, 2.2f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0, 1, 0));
                RenderMesh(meshes["elice"], shaders["VertexColor"], modelMatrix);
            }

            // Pachetul sub dronă
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(dronePosition.x, dronePosition.y + 0.6f, dronePosition.z));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(droneRotation), glm::vec3(0, 1, 0));
            RenderMesh(meshes["pachet"], shaders["VertexColor"], modelMatrix);
        }
        
    }
}

void Lab5::Update(float deltaTimeSeconds)
{

    dayNightCycle += deltaTimeSeconds * 1.5f; 
    if (dayNightCycle > 24.0f) {
        dayNightCycle -= 24.0f;
    }

    // vad daca e zi
    isDaytime = (dayNightCycle >= 6.0f && dayNightCycle < 18.0f);

    if (isDaytime) 
        glClearColor(0.678f, 0.886f, 1.0f, 1.0f);
    else 
        glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

    angle += deltaTimeSeconds * 500;
    if (angle > 360.0f)
    {
        angle -= 360.0f;
    }

    //perspectiva camera
    glm::vec3 cameraOffset = glm::vec3(0, 3, 10);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(droneRotation), glm::vec3(0, 1, 0));
    glm::vec3 rotatedOffset = glm::vec3(rotationMatrix * glm::vec4(cameraOffset, 1.0f));
    glm::vec3 newCameraPos = dronePosition + rotatedOffset;

    glm::vec3 targetOffset = glm::vec3(5, 3, 0);
    glm::vec3 rotatedTargetOffset = glm::vec3(rotationMatrix * glm::vec4(targetOffset, 1.0f));
    glm::vec3 newTarget = dronePosition + rotatedTargetOffset;

    camera->Set(newCameraPos,
                newTarget,
                glm::vec3(0, 1, 0));

    // scena principala
    glViewport(0, 0, window->GetResolution().x, window->GetResolution().y);
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    RenderScene();

    // minimap
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window->GetResolution().x / 6, window->GetResolution().y / 6);
    projectionMatrix = projectionMatrixOrto;

    // camera ortografica de sus in jos
    orthoCamera->Set(
        dronePosition + glm::vec3(0, 30, 0), 
        dronePosition,                       
        glm::vec3(0, 0, -1)              
    );

    float distanceToPacket = glm::distance(dronePosition, packet.position);
    if (!packet.collected && distanceToPacket < 1.5f)
    {
        packet.collected = true;
        destination.position = generateRandomPosition();
    }

    if (packet.collected)
    {
        packet.position = dronePosition - glm::vec3(0, 1, 0); // Plasăm pachetul sub dronă
    }

    float distanceToDestination = glm::distance(dronePosition, destination.position);
    if (packet.collected && distanceToDestination < 2.0f)
    {
        collectedPackets++;
        packet.collected = false;
        packet.position = generateRandomPosition();
        destination.position = generateRandomPosition();

        cout << "Collected packets: " << collectedPackets << endl;
    }

    RenderScene();

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}
void Lab5::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Lab5::RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));
    mesh->Render();
}

void Lab5::RenderMesh2(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(orthoCamera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrixOrto));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));
    mesh->Render();
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see input_controller.h.
 */

void Lab5::OnInputUpdate(float deltaTime, int mods)
{
    glm::vec3 newPosition = dronePosition; // Poziția potențială nouă
    float droneSpeed = 6.0f;
    float rotationSpeed = 90.0f;

    if (window->KeyHold(GLFW_KEY_W) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        newPosition.z -= droneSpeed * 2 * deltaTime * cos(glm::radians(droneRotation));
        newPosition.x -= droneSpeed * 2 * deltaTime * sin(glm::radians(droneRotation));
    }
    else if (window->KeyHold(GLFW_KEY_W))
    {
        newPosition.z -= droneSpeed * deltaTime * cos(glm::radians(droneRotation));
        newPosition.x -= droneSpeed * deltaTime * sin(glm::radians(droneRotation));
    }

    if (window->KeyHold(GLFW_KEY_S) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        newPosition.z += droneSpeed * 2 * deltaTime * cos(glm::radians(droneRotation));
        newPosition.x += droneSpeed * 2 * deltaTime * sin(glm::radians(droneRotation));
    }
    else if (window->KeyHold(GLFW_KEY_S))
    {
        newPosition.z += droneSpeed * deltaTime * cos(glm::radians(droneRotation));
        newPosition.x += droneSpeed * deltaTime * sin(glm::radians(droneRotation));
    }

    if (window->KeyHold(GLFW_KEY_A) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        newPosition.z += droneSpeed * 2 * deltaTime * sin(glm::radians(droneRotation));
        newPosition.x -= droneSpeed * 2 * deltaTime * cos(glm::radians(droneRotation));
    }
    else if (window->KeyHold(GLFW_KEY_A))
    {
        newPosition.z += droneSpeed * deltaTime * sin(glm::radians(droneRotation));
        newPosition.x -= droneSpeed * deltaTime * cos(glm::radians(droneRotation));
    }

    if (window->KeyHold(GLFW_KEY_D) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        newPosition.z -= droneSpeed * 2 * deltaTime * sin(glm::radians(droneRotation));
        newPosition.x += droneSpeed * 2 * deltaTime * cos(glm::radians(droneRotation));
    }
    else if (window->KeyHold(GLFW_KEY_D))
    {
        newPosition.z -= droneSpeed * deltaTime * sin(glm::radians(droneRotation));
        newPosition.x += droneSpeed * deltaTime * cos(glm::radians(droneRotation));
    }

    if (window->KeyHold(GLFW_KEY_E) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
        newPosition.y += droneSpeed * 2 * deltaTime;
    else if (window->KeyHold(GLFW_KEY_E))
        newPosition.y += droneSpeed * deltaTime;

    if (window->KeyHold(GLFW_KEY_Q) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
        newPosition.y -= droneSpeed * 2 * deltaTime;
    else if (window->KeyHold(GLFW_KEY_Q))
        newPosition.y -= droneSpeed * deltaTime;

    handleCollisions(deltaTime, newPosition);

    dronePosition = newPosition;

    if (window->KeyHold(GLFW_KEY_LEFT) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        droneRotation += rotationSpeed * 2 * deltaTime;
        if (droneRotation > 360.0f)
            droneRotation -= 360.0f;
    }
    else if (window->KeyHold(GLFW_KEY_LEFT))
    {
        droneRotation += rotationSpeed * deltaTime;
        if (droneRotation > 360.0f)
            droneRotation -= 360.0f;
    }

    if (window->KeyHold(GLFW_KEY_RIGHT) && window->KeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        droneRotation -= rotationSpeed * 2 * deltaTime;
        if (droneRotation < 0.0f)
            droneRotation += 360.0f;
    }
    else if (window->KeyHold(GLFW_KEY_RIGHT))
    {
        droneRotation -= rotationSpeed * deltaTime;
        if (droneRotation < 0.0f)
            droneRotation += 360.0f;
    }
}

void Lab5::OnKeyPress(int key, int mods)
{
}

void Lab5::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

void Lab5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Lab5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}

void Lab5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}

void Lab5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Lab5::OnWindowResize(int width, int height)
{
}
