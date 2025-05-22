#pragma once

#ifndef POSTPROCESSDATA_H
#define POSTPROCESSDATA_H

#include "../config.h"

struct PostProcessData {

    bool is_post_process; // Jesli nie to rysuj zwyczajnie quada z tekstura

    bool is_crt_curved; // Shader crt do zakrzywienia ekranu

    // Si³a tego efektu "wypuklosci" ekranu
    glm::vec2 crt_curvature;

    // Kolor "telewizora" (tej obramowki dookola)
    glm::vec3 crt_outline_color;

};

#endif
