#include <cstdio>

#include <baka_logger.h>

#include <baka_application.h>
#include <baka_time.h>
#include <baka_graphics.h>

#include <baka_camera.h>
#include <baka_mesh.h>
#include <baka_shader.h>
#include <baka_path_explorer.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include <baka_lights.h>
#include <baka_material.h>

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
        if(lightModel) delete lightModel;
        if(floor) delete floor;
        if(mesh) delete mesh;
        if(defaultMaterial) delete defaultMaterial;
        if(bulbMaterial) delete bulbMaterial;
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

        dirLight = baka::DirectionalLight(
            glm::vec3(90.0f / 255.0f, 140.0f / 255.0f, 252.0f / 255.0f),
            1.0f,
            glm::quat(glm::vec3(0.0f, 0.0f, 0.0f))
        );

        ambientLight = baka::AmbientLight(
            glm::vec3(235.0f / 255.0f, 126.0f / 255.0f, 80.0f / 255.0f),
            1.0f
        );

        pointLight = baka::PointLight(
            glm::vec3(0.0f, 1.0f, 0.0f),
            10.0f
        );

        floor = baka::Mesh::PrimitiveMesh(baka::Primitive::PLANE);
        lightModel = baka::Mesh::PrimitiveMesh(baka::Primitive::SPHERE);

        baka::LightsManager &lights = baka::LightsManager::Get();
        lights.ActivateLight(&ambientLight);
        lights.ActivateLight(&pointLight);
        lights.ActivateLight(&dirLight);

        this->defaultMaterial = new baka::Material(this->defaultShader);
        this->defaultMaterial->SetUniform("u_tint", glm::vec4(0.8f, 0.2f, 0.3f, 1.0f));
        this->defaultMaterial->SetUniform("u_useLights", 1.0f);

        this->bulbMaterial = new baka::Material(this->defaultShader);
        this->bulbMaterial->SetUniform("u_tint", glm::vec4(pointLight.color, 1.0f));
        this->bulbMaterial->SetUniform("u_useLights", 0.0f);
    }

    void Update() override
    {
        static glm::vec3 camPos = glm::vec3(0.0f, 0.0f, -5.0f);
        static glm::quat camRot = glm::identity<glm::quat>();
        const float speed = 2.0f;
        const float rotSpeed = glm::pi<float>() / 2.0f; //in rads per second
        
        float dt_msec = g_time->GetDeltaTime();
        float dt = dt_msec / 1000.0f;
        int fwd = g_input->IsKeyPressed(BAKA_KEYCODE_W) - g_input->IsKeyPressed(BAKA_KEYCODE_S);
        int side = g_input->IsKeyPressed(BAKA_KEYCODE_D) - g_input->IsKeyPressed(BAKA_KEYCODE_A);
        int vertical = g_input->IsKeyPressed(BAKA_KEYCODE_SPACE) - g_input->IsKeyPressed(BAKA_KEYCODE_LSHIFT);
        glm::vec3 f  = (glm::quat(camRot) * glm::vec3( 0.0f, 0.0f, 1.0f)) * (speed * dt * (float)fwd);
        glm::vec3 r  = (glm::quat(camRot) * glm::vec3( 1.0f, 0.0f, 0.0f)) * (speed * dt * (float)side);
        glm::vec3 up = (glm::quat(camRot) * glm::vec3( 0.0f, 1.0f, 0.0f)) * (speed * dt * (float)vertical);
        camPos += f + r + up;
        cam.SetPosition(camPos);
        
        int ver = g_input->IsKeyPressed(BAKA_KEYCODE_UP) - g_input->IsKeyPressed(BAKA_KEYCODE_DOWN);
        int hor = g_input->IsKeyPressed(BAKA_KEYCODE_RIGHT) - g_input->IsKeyPressed(BAKA_KEYCODE_LEFT);
        camRot *= glm::quat(glm::vec3(
            -ver * rotSpeed * dt,
            hor * rotSpeed * dt,
            0.0f
        ));
        cam.SetRotation(camRot);

        cam.Update();

        if(g_input->IsKeyPressed(BAKA_KEYCODE_E))
            rot *= glm::quat(glm::vec3(0.0f, M_PI * dt, 0.0f));

        // light.rotation *= glm::quat(glm::vec3(0.0f, rotSpeed * dt, 0.0f));
        int modelFwd = g_input->IsKeyPressed(BAKA_KEYCODE_U) - g_input->IsKeyPressed(BAKA_KEYCODE_J);
        int modelRight = g_input->IsKeyPressed(BAKA_KEYCODE_K) - g_input->IsKeyPressed(BAKA_KEYCODE_H);
        modelPos += glm::vec3(
            modelRight * speed * dt,
            0.0f,
            modelFwd * speed * dt
        );

        baka::Graphics &graphics = baka::Graphics::Get();
        char newWindowName[BAKA_WINDOW_NAME_MAX_LENGTH] = {0};
        snprintf(
            newWindowName, 
            BAKA_WINDOW_NAME_MAX_LENGTH, 
            "%s",
            graphics.GetWindowName());
        char *fpsStart = strrchr(newWindowName, '(');
        if(fpsStart) *(fpsStart - 1) = '\0';
        snprintf(
            newWindowName,
            BAKA_WINDOW_NAME_MAX_LENGTH,
            "%s (%.2f ms)",
            newWindowName,
            dt_msec
        );
        graphics.SetWindowName(newWindowName);
    }

    void OnRender() override
    {
        const glm::vec4 tint = glm::vec4(0.5f, 0.7f, 0.5f, 1.0f);
        baka::LightsManager &lightsManager = baka::LightsManager::Get();
        glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), modelPos) * glm::toMat4(rot);

        defaultShader->Bind();
        defaultShader->SetUniform(
            baka::UniformType::UNIFORM_MAT4X4,
            "u_modelViewProj",
            (void*)glm::value_ptr(cam.GetViewProjection() * modelMat)
        );
        defaultMaterial->Bind();
        defaultShader->SetUniform(
            baka::UniformType::UNIFORM_MAT4X4,
            "u_normalMat",
            (void*)glm::value_ptr(  modelMat )
        );
        lightsManager.BindLights(defaultShader);
        mesh->Render();
        defaultShader->Unbind();

        modelMat = glm::translate( glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) * 
                    glm::toMat4(
                        glm::quat(
                            glm::vec3(glm::pi<float>() / 2.0f, 0.0f, 0.0f)
                        )
                    );

        defaultShader->Bind();
        defaultShader->SetUniform(
            baka::UniformType::UNIFORM_MAT4X4,
            "u_modelViewProj",
            (void*)glm::value_ptr(cam.GetViewProjection() * modelMat)
        );
        defaultMaterial->Bind();
        defaultShader->SetUniform(
            baka::UniformType::UNIFORM_MAT4X4,
            "u_normalMat",
            (void*)glm::value_ptr( modelMat )
        );
        lightsManager.BindLights(defaultShader);
        floor->Render();
        defaultShader->Unbind();

        modelMat = glm::translate(glm::mat4(1.0f), pointLight.position) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

        defaultShader->Bind();
        defaultShader->SetUniform(
            baka::UniformType::UNIFORM_MAT4X4,
            "u_modelViewProj",
            (void*)glm::value_ptr(cam.GetViewProjection() * modelMat)
        );
        bulbMaterial->Bind();
        defaultShader->SetUniform(
            baka::UniformType::UNIFORM_MAT4X4,
            "u_normalMat",
            (void*)glm::value_ptr( modelMat )
        );
        lightModel->Render();
        defaultShader->Unbind();
    }

private:
    baka::Camera cam;
    baka::Shader *defaultShader;
    baka::Mesh *mesh;
    baka::Material *defaultMaterial;
    baka::Material *bulbMaterial;
    glm::quat rot;

    baka::Mesh *floor;
    baka::Mesh *lightModel;

    glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
    
    baka::DirectionalLight dirLight;
    baka::AmbientLight ambientLight;
    baka::PointLight pointLight;
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Run();
    return 0;
}