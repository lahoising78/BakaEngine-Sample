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

        app_config.window_config.width = 1280;
        app_config.window_config.height = 720;
        snprintf(app_config.window_config.name, BAKA_WINDOW_NAME_MAX_LENGTH, "Sample Application");
    }

    void Update() override
    {
        // bakalog("time in int64 since start: %li", g_time->GetTime());

        if( g_input->IsMouseButtonPressed(BAKA_MOUSE_BUTTON_RIGHT) )
        {
            bakalog("Mouse button right is actively pressed");
        }

        if( g_input->MouseButtonJustPressed(BAKA_MOUSE_BUTTON_LEFT) )
        {
            bakalog("just pressed");
        }

        if(g_input->MouseButtonJustReleased(BAKA_MOUSE_BUTTON_LEFT))
        {
            bakalog("button released");
        }
    }
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Start();
    return 0;
}