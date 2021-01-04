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
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    fTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    fNormal = aNormal;
}
)";

char DEFAULT_FRAGMENT_SHADER[] = R"(
#version 330 core
out vec4 FragColor;

in vec3 fNormal;
in vec2 fTexCoord;

void main() {
    FragColor = vec4(fNormal, 1.0);
}
)";

Shader Shader::defaultShader;