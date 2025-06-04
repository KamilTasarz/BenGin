#pragma once

#ifndef POSTPROCESSDATA_H
#define POSTPROCESSDATA_H

#include "../config.h"

struct PostProcessData {

    bool is_post_process; // Jesli nie to rysuj po prostu teksture zapisan¹ normalnie do framebuffera

    bool is_ssao;

    bool is_bloom;

    bool is_crt_curved; // Shader crt do zakrzywienia ekranu

    // Si³a tego efektu "wypuklosci" ekranu
    glm::vec2 crt_curvature;

    // Kolor "telewizora" (tej obramowki dookola)
    glm::vec3 crt_outline_color;

    // Ilosc linii w poziomie i pionie dla crt
    glm::vec2 crt_screen_resolution;
    
    // Promien elipsy winiety
    float crt_vignette_radius;

    // Jak ostro przechodzi kolor miedzy liniami kolorowymi a czarnymi
    glm::vec2 crt_lines_sinusoid_factor;

    // Jak ostro przechodzi kolor ekran -> winietka
    float crt_vignette_factor;

    // Caloekranowy filtr kolorystyczny
    glm::vec3 crt_brightness;

    //////////////////////////////

    unsigned int ssao_kernel_samples; // How many samples to generate the SSAO kernel (za malo powoduje
                                      //  ziarnisty efekt, za duzo skutkuje spadkami wydajnosci)

    float ssao_radius; // Jak duzy jest promien tego efektu, im wiekszy tym te szpary bardziej przesadzone

    float ssao_bias; // delikatne przesuniecie aby redukowac czarne plamy

    float ssao_intensity; // jak intensywne jest zaciemnienie

    glm::vec2 ssao_noise_scale; // jak czesto powtarza sie wzor zaciemnienia

    //////////////////////////////

    float bloom_treshold;

    float bloom_intensity;

    int bloom_blur_passes;

};

#endif
