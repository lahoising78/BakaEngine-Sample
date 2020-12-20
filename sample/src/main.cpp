#include <cstdio>

#include <baka_logger.h>

#include <baka_application.h>
#include <baka_time.h>

#include <baka_camera.h>
#include <baka_mesh.h>
#include <baka_shader.h>
#include <baka_path_explorer.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

baka::Input *g_input = &baka::Input::Get();
baka::Time *g_time = nullptr;
class SampleApp : public baka::BakaApplication
{
public:
    SampleApp()
        : baka::BakaApplication()
    {
        g_time = &baka::Time::Get();

        app_config.window_config.width = 800;
        app_config.window_config.height = 600;
        snprintf(app_config.window_config.name, BAKA_WINDOW_NAME_MAX_LENGTH, "Sample Application");
    }

    ~SampleApp()
    {
        if(mesh) delete mesh;
        if(defaultShader) delete defaultShader;
        bakalog("sample app destroyed");
    }

    void Start() override
    {
        defaultShader = baka::Shader::Load(
            "/shaders/default.vert",
            "/shaders/default.frag"
        );

        mesh = baka::Mesh::PrimitiveMesh(baka::Primitive::CONE);
        cam = baka::Camera(baka::CameraType::PERSPECTIVE, {45.0f}, 0.1f, 1000.0f);
        rot = glm::identity<glm::quat>();
    }

    void Update() override
    {
        static glm::vec3 camPos = glm::vec3(0.0f, 0.0f, -5.0f);
        static glm::quat camRot = glm::identity<glm::quat>();
        const float speed = 2.0f;
        const float rotSpeed = M_PI / 2.0f; //in rads per second
        
        float dt = g_time->GetDeltaTime() / 1000.0f;
        int fwd = g_input->IsKeyPressed(BAKA_KEYCODE_W) - g_input->IsKeyPressed(BAKA_KEYCODE_S);
        int side = g_input->IsKeyPressed(BAKA_KEYCODE_D) - g_input->IsKeyPressed(BAKA_KEYCODE_A);
        int vertical = g_input->IsKeyPressed(BAKA_KEYCODE_SPACE) - g_input->IsKeyPressed(BAKA_KEYCODE_LSHIFT);
        glm::vec3 f  = (glm::quat(camRot) * glm::vec3( 0.0f, 0.0f, 1.0f)) * (speed * dt * (float)fwd);
        glm::vec3 r  = (glm::quat(camRot) * glm::vec3(-1.0f, 0.0f, 0.0f)) * (speed * dt * (float)side);
        glm::vec3 up = (glm::quat(camRot) * glm::vec3( 0.0f, 1.0f, 0.0f)) * (speed * dt * (float)vertical);
        camPos += f + r + up;
        cam.SetPosition(camPos);
        
        int ver = g_input->IsKeyPressed(BAKA_KEYCODE_UP) - g_input->IsKeyPressed(BAKA_KEYCODE_DOWN);
        int hor = g_input->IsKeyPressed(BAKA_KEYCODE_RIGHT) - g_input->IsKeyPressed(BAKA_KEYCODE_LEFT);
        camRot *= glm::quat(glm::vec3(
            -ver * rotSpeed * dt,
            -hor * rotSpeed * dt,
            0.0f
        ));
        cam.SetRotation(camRot);

        cam.Update();

        if(g_input->IsKeyPressed(BAKA_KEYCODE_E))
            rot *= glm::quat(glm::vec3(0.0f, M_PI * dt, 0.0f));
    }

    void OnRender() override
    {
        const glm::vec4 tint = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        const glm::mat4 rotMat = glm::toMat4(rot);

        defaultShader->Bind();
        defaultShader->SetUniform(
            baka::Shader::Type::MAT4X4,
            "u_modelViewProj",
            (void*)glm::value_ptr(cam.GetViewProjection() * rotMat)
        );
        defaultShader->SetUniform(
            baka::Shader::Type::FLOAT4,
            "u_tint",
            (void*)(glm::value_ptr(tint))
        );
        defaultShader->SetUniform(
            baka::Shader::Type::MAT4X4,
            "u_normalMat",
            (void*)glm::value_ptr( rotMat )
        );
        mesh->Render();
    }

private:
    baka::Camera cam;
    baka::Shader *defaultShader;
    baka::Mesh *mesh;
    glm::quat rot;
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Run();
    return 0;
}