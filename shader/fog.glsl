#version 330 core

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D fogTexture; // The fog texture
uniform vec2 playerPos;      // Player's position
uniform vec2 screenSize;     // Size of the screen or viewport
uniform float clearRadius;   // Radius around the player where fog is cleared

void main()
{
    vec4 fogColor = texture(fogTexture, fragTexCoord);

    vec2 texPos = vec2(fragTexCoord.x * screenSize.x, fragTexCoord.y * screenSize.y);
    float dist = distance(texPos, playerPos);

    float edge0 = clearRadius / 2;
    float edge1 = clearRadius;
    float x = clamp((dist - edge0) / (edge1 - edge0), 0.0, 1.0);
    float fogDensity = x * x * (3 - 2 * x);

    fogColor.a *= fogDensity;
    finalColor = fogColor;
}

