#include <cstdio>

#include <baka_logger.h>

#include <baka_application.h>

baka::Input *input = &baka::Input::Get();
class SampleApp : public baka::BakaApplication
{
public:
    SampleApp()
        : baka::BakaApplication()
    {
        app_config.window_config.width = 1280;
        app_config.window_config.height = 720;
        snprintf(app_config.window_config.name, BAKA_WINDOW_NAME_MAX_LENGTH, "Sample Application");
    }

    void Update() override
    {
        BAKA_ASSERT(input);
        
        if( input->MouseButtonJustPressed(BAKA_MOUSE_BUTTON_LEFT) )
            bakalog("left click ;)");

        if( input->IsKeyPressed(BAKA_KEYCODE_R) )
            bakalog("you are pressing r");
    }
};

int main(int argc, char *argv[])
{
    bakalog("hola, como esta?");
    SampleApp app = SampleApp();
    app.Start();
    return 0;
}