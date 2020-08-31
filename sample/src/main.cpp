#include <cstdio>

#include <baka_logger.h>

#include <baka_application.h>
#include <baka_time.h>

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
        bakalog("sample app destroyed");
    }

    void Start() override
    {
        
    }

    void Update() override
    {
        
    }
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Run();
    return 0;
}