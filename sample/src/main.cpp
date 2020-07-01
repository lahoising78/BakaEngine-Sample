#include <cstdio>

#include <baka_logger.h>

#include <baka_application.h>
#include <baka_time.h>

baka::Input *input = &baka::Input::Get();
baka::Time *g_time = nullptr;
class SampleApp : public baka::BakaApplication
{
public:
    SampleApp()
        : baka::BakaApplication()
    {
        g_time = &baka::Time::Get();

        app_config.window_config.width = 1280;
        app_config.window_config.height = 720;
        snprintf(app_config.window_config.name, BAKA_WINDOW_NAME_MAX_LENGTH, "Sample Application");
    }

    void Update() override
    {
        bakalog("time in int64 since start: %f", g_time->GetTime<float>());
    }
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Start();
    return 0;
}