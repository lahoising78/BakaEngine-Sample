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
        if( input->KeyJustPressed(BAKA_KEYCODE_Q) )
            baka_log_filter_output(BAKA_LOG_BIT_INFO, BAKA_LOG_ACTION_SET);

        else if ( input->KeyJustPressed(BAKA_KEYCODE_W) )
            baka_log_filter_output(BAKA_LOG_BIT_WARN, BAKA_LOG_ACTION_SET);

        else if ( input->KeyJustPressed(BAKA_KEYCODE_E) )
            baka_log_filter_output(BAKA_LOG_BIT_ERR, BAKA_LOG_ACTION_SET);
        
        else if( input->KeyJustPressed(BAKA_KEYCODE_A) )
            baka_log_filter_output(BAKA_LOG_BIT_INFO, BAKA_LOG_ACTION_ON);

        else if ( input->KeyJustPressed(BAKA_KEYCODE_S) )
            baka_log_filter_output(BAKA_LOG_BIT_WARN, BAKA_LOG_ACTION_ON);

        else if ( input->KeyJustPressed(BAKA_KEYCODE_D) )
            baka_log_filter_output(BAKA_LOG_BIT_ERR, BAKA_LOG_ACTION_ON);
        
        else if( input->KeyJustPressed(BAKA_KEYCODE_Z) )
            baka_log_filter_output(BAKA_LOG_BIT_INFO, BAKA_LOG_ACTION_OFF);

        else if ( input->KeyJustPressed(BAKA_KEYCODE_X) )
            baka_log_filter_output(BAKA_LOG_BIT_WARN, BAKA_LOG_ACTION_OFF);

        else if ( input->KeyJustPressed(BAKA_KEYCODE_C) )
            baka_log_filter_output(BAKA_LOG_BIT_ERR, BAKA_LOG_ACTION_OFF);

        else if ( input->KeyJustPressed(BAKA_KEYCODE_M) )
        {
            bakalog("this is info message");
            bakawarn("this is warn message");
            bakaerr("this is error message");
        }
    }
};

int main(int argc, char *argv[])
{
    SampleApp app = SampleApp();
    app.Start();
    return 0;
}