#version 330 core

#define PI 2 //3.141592

in vec2 vectorUV;

out vec4 FragColor;

uniform sampler2D screenTexture; // klasyczek do post processu
uniform vec2 curvature; // jak mocno jest widoczny efekt zakrzywienia (uzyty w metodzie curveTexture)
uniform vec3 outline_color; // kolor obramowki, a'la telewizor

uniform vec2 screen_resolution; // uzywany do "gestosci" sinusoidy w obie strony (x i y mozna podac rozne)
uniform float vignette_radius; // troche funkcjonuje jak resolution po x (nie ma dla y zeby wyszla elipsa [wiem, pokretna logika ale ma sens, trust])

uniform vec2 lines_sinusoid_factor; // jak duzy kontrast miedzy liniami a imitacją pikseli
uniform float vignette_factor; // jak ostra jest winietka (przejscie jasne-ciemne)

uniform vec3 brightness; // moze tez byc uzyte jako filtr kolorystyczny (mnożniki wiec x > 1 zeby rozjasnilo, < 1 zeby przyciemnilo, 1 nie zmieni nic)

uniform float time;


float random(vec2 st);
vec2 curveTexture(vec2 uv);
vec4 screenLinesWithIntensity(float uv, float resolution, float opacity);
vec4 vignetteWithIntensity(vec2 uv, float resolution, float opacity);

void main()
{

    // Przykladowe wartosci
    // float sx = 240;
    // float sy = 320;
    // float opac = 1.0;
    // vec3 brightness = {2.0, 1.0, 1.0};

    vec2 remappedUV = curveTexture(vec2(vectorUV)); // Uzywamy ladnego efektu zakrzywienia

    vec4 tempColor = texture(screenTexture, remappedUV); // Wyznaczamy kolor fragmentow na zaktualizowane uv

    tempColor *= vignetteWithIntensity(remappedUV, vignette_radius, vignette_factor);

    tempColor *= screenLinesWithIntensity(remappedUV.x, screen_resolution.y, lines_sinusoid_factor.x);
    tempColor *= screenLinesWithIntensity(remappedUV.y, screen_resolution.x, lines_sinusoid_factor.y);

    // Upewniamy sie czy nie wychodzimy poza krawedzie tekstury
    if(remappedUV.x < 0.0 || remappedUV.x > 1.0 || remappedUV.y < 0.0 || remappedUV.y > 1.0) {
         FragColor = vec4(outline_color, 1.0); // Jesli tak to dajemy kolor obramowki
    } else {
        tempColor *= vec4(brightness, 1.0);
        FragColor = tempColor;
    }

    //float offset = 0.005;
    //float glitch = sin(vectorUV.y * 50.0 + time * 5.0) * 0.02;

    //float r = texture(screenTexture, vec2(vectorUV.x + offset + glitch, vectorUV.y)).r;
    //float g = texture(screenTexture, vectorUV).g;
    //float b = texture(screenTexture, vec2(vectorUV.x - offset - glitch, vectorUV.y)).b;
    //vec3 color = vec3(r, g, b);

    //float noise = random(vectorUV * 100.0 + time) * 0.15;
    //float line = sin(vectorUV.y * 600.0 + time * 20.0) * 0.03;

    //color += vec3(noise + line);
    //color = clamp(color, 0.0, 1.0);

    //FragColor = vec4(color, 1.0);
    //FragColor = texture(screenTexture, vectorUV);

}

// Generate curved effect, (apply first)
vec2 curveTexture(vec2 uv) {
    
    uv = uv * 2.0 - 1.0; // Przelicz wspolrzedne wartosci na teksturze na [-1, 1]

    // Wzor na cubic function do rozciagniecia tekstury: f(x) = x(abs(2x-1) / curvature)^2 + x
    // Uzywamy uv.yx bo wyobrazmy sobie, zakrzywienie w pionie zalezy od polozenia w poziomie
    vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);
    uv = uv + uv * offset * offset;

    uv = uv * 0.5 + 0.5; // Przelicz z powrotem na standardowe koordynaty teksturowe [0, 1]
    return uv;
    
}

// Generate lines on screen to add pixel differentiation effect, (apply last) 
// Read resolution as how many lines to generate (kind of resolution dependent to look good)
vec4 screenLinesWithIntensity(float uv, float resolution, float opacity) {
    
    // generuje fale sinusową, ktora sie powtarza na ekranie tyle ile mowi zmienna resolution
    float intensity = sin(uv * resolution * PI * 2.0);
    
    intensity = 0.5 * intensity + 0.5; // Przelicza z zakresu [-1, 1] do zakresu [0, 1]
    intensity = intensity * 0.9 + 0.1; // Przelicza [0, 1] na zakres [0.1, 1] (zeby nie bylo calkiem czarne bo zaciemni)

    return vec4(vec3(pow(intensity, opacity)), 1.0); // Apply power factor for sharper/less sharp effect (kontrast)

}

// Generate darker fragments near the edges to make a vignette, (apply second)
vec4 vignetteWithIntensity(vec2 uv, float resolution, float opacity) {
    
    float intensity = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y); // tworzy parabole z maksimum w srodku uv[0.5, 0.5]
    
    // im wieksze resolution.x tym slabsza winieta (jasniejszy kolor). im wieksze opacity tym ostrzejsza winieta
    // clampujemy zeby nie przekroczyc [0, 1] i alfa na 1
    return vec4(vec3(clamp(pow((resolution / 4.0) * intensity, opacity), 0.0, 1.0)), 1.0);

}

// Szum oparty o pozycję i czas
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
