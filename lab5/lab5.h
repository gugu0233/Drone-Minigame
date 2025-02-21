#pragma once

#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"

#include "components/text_renderer.h"

namespace m1
{
    class Lab5 : public gfxc::SimpleScene
    {
    public:
        Lab5();
        ~Lab5();

        struct Object
        {
            Mesh *mesh;
            glm::mat4 modelMatrix;
        };

        struct Drona
        {
        };
        void Init() override;

    private:
        void CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        bool checkColiziunePachet(glm::vec3 pozPachet);
        void RenderScene();
        void RenderScene(bool Tip);
        bool validMove(glm::vec3 PozDrona, float deltaTime, float cameraSpeed);
        void RenderMesh2(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix);
        bool checkCollisionWithObject(const glm::vec3 &dronePos, const glm::vec3 &objPos, float objRadius);
        void handleCollisions(float deltaTime, glm::vec3 &newPosition);

    protected:
        implemented::Camera *camera;
        implemented::Camera *orthoCamera;

        glm::mat4 projectionMatrix;
        glm::mat4 projectionMatrixOrto;

        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        float fov;
        float ortoD;

        
        struct Packet
        {
            glm::vec3 position;
            bool collected;
        };

        Packet packet;

        struct Destination
        {
            glm::vec3 position;
        };

        Destination destination;

        float left;
        float right;
        float bottom;
        float top;

        gfxc::TextRenderer *text;
    };
} // namespace m1
