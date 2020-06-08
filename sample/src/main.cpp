#include <cstdio>

#include <baka_logger.h>

#include <baka_application.h>

class SampleApp : public baka::BakaApplication
{
public:
    SampleApp()
        : baka::BakaApplication()
    {
        app_config.initialWindowWidth = 1280;
        app_config.initialWindowHeight = 720;
        snprintf(app_config.initialWindowName, BAKA_WINDOW_NAME_MAX_LENGTH, "Sample Application");
    }

    void Update() override
    {}
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Start();
    return 0;
}