#version 330 core
out vec4 FragColor;

in vec3 fNormal;
in vec2 fTexCoord;

struct Material {
    vec4 color;
}; 
  
uniform Material material;

void main() {
    FragColor = material.color;
}
