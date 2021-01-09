#include "core/rendering/Shader.h"

/**
 * Principal Vertex Shader pour l'engine
 */
char DEFAULT_VERTEX_SHADER[] = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fPos;
out vec3 fNormal;
out vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fTexCoord = aTexCoord;
    fPos = vec3(model * vec4(aPos, 1.0));
    fNormal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

/**
 * Principal Fragment Shader pour l'engine
 */
char DEFAULT_FRAGMENT_SHADER[] = R"(
#version 330 core
out vec4 FragColor;

in vec3 fPos;
in vec3 fNormal;
in vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

///// LIGHT SYSTEM
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define MAX_POINT_LIGHTS 32 
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform float nbPointLights;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void main() {
    // properties
    vec3 norm = normalize(fNormal);
    vec3 viewDir = normalize(viewPos - fPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < nbPointLights && i < MAX_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, fPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
}
)";

/**
 * Vertex shader pour pouvoir afficher des quads, utile pour le post process
 */
char DEFAULT_VERTEX_QUAD_SHADER[] = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormalPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
} 
)";

/**
 * Fragment shader permettant d'extraire les features utiles au post processing + merge le Anti aliasing
 */
char DEFAULT_FRAGMENT_FEATURE_SHADER[] = R"(
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
  
in vec2 TexCoords;
uniform sampler2DMS screenTextureMS; 
uniform vec2 screenSize;

void main() {
	ivec2 vpCoords = ivec2(screenSize.x, screenSize.y);
	vpCoords.x = int(vpCoords.x * TexCoords.x); 
	vpCoords.y = int(vpCoords.y * TexCoords.y);

	vec4 sample1 = texelFetch(screenTextureMS, vpCoords, 0);
	vec4 sample2 = texelFetch(screenTextureMS, vpCoords, 1);
	vec4 sample3 = texelFetch(screenTextureMS, vpCoords, 2);
	vec4 sample4 = texelFetch(screenTextureMS, vpCoords, 3);

    FragColor = (sample1 + sample2 + sample3 + sample4) / 4.0f;

    float brightness = FragColor.r;
    if(brightness > 0.6) BrightColor = vec4(FragColor.rgb, 1.0);
    else BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

/**
 * Fragment Shader pour le rendu final
 */
char DEFAULT_FRAGMENT_SCREEN_SHADER[] = R"(
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;

void main() { 
    FragColor = texture2D(screenTexture, TexCoords);
}
)";

illusion::resources::assets::ShaderResource Shader::defaultShaderResource;
Shader Shader::defaultShader;
Shader Shader::featureShader;
Shader Shader::screenShader;