#include "core/rendering/Shader.h"

char DEFAULT_VERTEX_SHADER[] = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fNormal;
out vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fTexCoord = aTexCoord;
    fNormal = aNormal;
}
)";

char DEFAULT_FRAGMENT_SHADER[] = R"(
#version 330 core
out vec4 FragColor;

in vec3 fNormal;
in vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

Shader Shader::defaultShader;