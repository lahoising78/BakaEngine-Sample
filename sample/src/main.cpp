#include <cstdio>

#include <baka_logger.h>

#include <baka_application.h>
#include <baka_time.h>

#include <baka_camera.h>
#include <baka_mesh.h>
#include <baka_shader.h>
#include <baka_path_explorer.h>
#include <glm/gtc/type_ptr.hpp>

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
            "D:/Documents/git/baka/sample/build/resources" "/shaders/default.vert",
            "D:/Documents/git/baka/sample/build/resources" "/shaders/default.frag"
        );

        float vertices[] = 
        {
            -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f        
        };
        baka::VertexBuffer *vb = baka::VertexBuffer::Create(vertices, sizeof(vertices));

        baka::VertexBufferLayout layout = baka::VertexBufferLayout({
            { baka::VertexAttributeType::ATTRIBUTE_FLOAT, 3 },
            { baka::VertexAttributeType::ATTRIBUTE_FLOAT, 4 }
        });

        std::uint32_t indices[] = { 0, 1, 2 };
        baka::IndexBuffer *ib = baka::IndexBuffer::Create(indices, sizeof(indices) / sizeof(std::uint32_t));

        mesh = baka::Mesh::Create(vb,layout, ib);

        cam = baka::Camera(baka::CameraType::PERSPECTIVE, {45.0f});
        cam.SetPosition(glm::vec3(0.0f, 0.0f, -1.0f));
    }

    void Update() override
    {
        cam.Update();
    }

    void OnRender() override
    {
        defaultShader->Bind();
        defaultShader->SetUniform(
            baka::Shader::Type::MAT4X4,
            "proj",
            (void*)glm::value_ptr(cam.GetViewProjection())
        );
        mesh->Render();
    }

private:
    baka::Camera cam;
    baka::Shader *defaultShader;
    baka::Mesh *mesh;
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Run();
    return 0;
}