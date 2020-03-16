#include <iostream>
#include <fstream>
#include <string.h>
#include "baka_config.h"

namespace baka
{
    BakaConfig baka_config;

    bool is_empty(std::ifstream& pFile)
    {
        return pFile.peek() == std::ifstream::traits_type::eof();
    }

    BakaConfig::BakaConfig()
    {
        std::ifstream inf;
        inf.open("../baka.config");
        if(!inf.is_open())
        {
            LoadDefault();
            return;
        }

        std::string data;
        
        while( !is_empty(inf) )
        {
            inf >> data;

            if( data == "app_name" )
            {
                getline(inf, app_name);
            }
            else if ( data == "window_width" )
            {
                inf >> window_width;
            }
            else if ( data == "window_height" )
            {
                inf >> window_height;
            }
            else if ( data == "validation" )
            {
                inf >> validation;
            }
            else if ( data == "clear" )
            {
                inf >> clear[0];
                inf >> clear[1];
                inf >> clear[2];
                inf >> clear[3];
            }
        }

        inf.close();
    }

    void BakaConfig::LoadDefault()
    {
        app_name = "Baka Engine";
        window_width = 1280;
        window_height = 720;
        validation = true;
        memset(clear, 0, sizeof(float) * 4);
    }
}