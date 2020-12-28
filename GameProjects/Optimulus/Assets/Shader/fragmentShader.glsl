#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform bool collision;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = collision ? vec4(1,0,0,1) : vec4(0.5,0.8,0.2,1);
}
