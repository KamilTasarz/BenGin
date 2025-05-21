#version 330 core

in vec2 vectorUV;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec2 curvature; // jak mocno jest widoczny efekt zakrzywienia (uzyty w metodzie curveTexture)
uniform float time;

// Szum oparty o pozycję i czas
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 curveTexture(vec2 uv) {
    
    uv = uv * 2.0 - 1.0; // Przelicz wspolrzedne wartosci na teksturze na [-1, 1]

    // Wzor na cubic function do rozciagniecia tekstury: f(x) = x(abs(2x-1) / curvature)^2 + x
    // Uzywamy uv.yx bo wyobrazmy sobie, zakrzywienie w pionie zalezy od polozenia w poziomie
    vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);
    uv = uv + uv * offset * offset;

    uv = uv * 0.5 + 0.5; // Przelicz z powrotem na standardowe koordynaty teksturowe [0, 1]
    return uv;
    
}

void main()
{

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

    vec2 remappedUV = curveTexture(vec2(vectorUV)); // Uzywamy ladnego efektu zakrzywienia
    // remappedUV = clamp(remappedUV, 0.0, 1.0);

    vec4 tempColor = texture(screenTexture, remappedUV); // Wyznaczamy kolor fragmentow na zaktualizowane uv
    // FragColor = tempColor;

    // Upewniamy sie czy nie wychodzimy poza krawedzie tekstury
    if(remappedUV.x < 0.0 || remappedUV.x > 1.0 || remappedUV.y < 0.0 || remappedUV.y > 1.0) {
         FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Jesli tak to dajemy kolor obramowki
    } else {
        FragColor = tempColor;
    }

    //FragColor = vec4(color, 1.0);
    //FragColor = texture(screenTexture, vectorUV);

}
