#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

// Funkcja generująca efekt szumu
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
    // Pobierz kolor oryginalnego pikselu
    vec3 color = texture(screenTexture, TexCoord).rgb;

    // Dodaj efekt przesunięcia kanałów RGB (glitche)
    float offset = 0.005; // Przesunięcie
    float glitch = sin(TexCoord.y * 50.0) * 0.02; // Dynamika glitcha
    vec3 shiftedColor = vec3(
        texture(screenTexture, vec2(TexCoord.x + offset + glitch, TexCoord.y)).r,
        texture(screenTexture, TexCoord).g,
        texture(screenTexture, vec2(TexCoord.x - offset - glitch, TexCoord.y)).b
    );

    // Dodaj losowy szum
    float noise = random(TexCoord * 100.0) * 0.2;
    shiftedColor += vec3(noise);

    // Nałóż "linijki" VHS na ekran
    float line = sin(TexCoord.y * 600.0) * 0.03;
    shiftedColor += vec3(line);

    // Wyjściowy kolor
    FragColor = vec4(shiftedColor, 1.0);
}
