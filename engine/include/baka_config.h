#ifndef _BAKA_CONFIG_H_
#define _BAKA_CONFIG_H_

#include <string>
#include <cstdint>

namespace baka
{
    class BakaConfig
    {
    public:
        BakaConfig();
        const char *GetAppName() { return app_name.c_str(); }
        int GetWindowWidth() { return window_width; }
        int GetWindowHeight() { return window_height; }
        bool GetValidation() { return validation; }
        float *GetClearValue() { return clear; }

    private:
        void LoadDefault();

    private:
        std::string app_name;
        int window_width, window_height; 
        bool validation;
        float clear[4];
    };

    extern BakaConfig baka_config;
}

#endif