#include "core/rendering/Shader.h"

/**
 * Principal Vertex Shader pour l'engine
 */
char DEFAULT_VERTEX_SHADER[] = R"(
#version 330 core
#define NUM_BONES_PER_VERTEX 4
#define NUM_BONES_PER_MESH 128

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uvec4 aBoneIds;
layout (location = 4) in vec4 aBoneWeights;

out vec3 fPos;
out vec3 fNormal;
out vec2 fTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float SkeletonActive;
uniform mat4 Bones[NUM_BONES_PER_MESH];


void main() {
    mat4 modelComputed = model;
    mat4 vp = projection * view;

    if(SkeletonActive > 0.0){
        mat4 bonesTransform = Bones[aBoneIds[0]] * aBoneWeights[0];
        for(int i=1; i<NUM_BONES_PER_VERTEX; i++){
            bonesTransform += Bones[aBoneIds[i]] * aBoneWeights[i];
        }
        modelComputed = modelComputed * bonesTransform;
    }

    fTexCoord = aTexCoord;
    fPos = vec3(modelComputed * vec4(aPos, 1.0));
    fNormal = mat3(transpose(inverse(modelComputed))) * aNormal;
    
    gl_Position = vp * modelComputed * vec4(aPos, 1.0);
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

uniform float bloomThreshold;

void main() {
	ivec2 vpCoords = ivec2(screenSize.x, screenSize.y);
	vpCoords.x = int(vpCoords.x * TexCoords.x); 
	vpCoords.y = int(vpCoords.y * TexCoords.y);

	vec4 sample1 = texelFetch(screenTextureMS, vpCoords, 0);
	vec4 sample2 = texelFetch(screenTextureMS, vpCoords, 1);
	vec4 sample3 = texelFetch(screenTextureMS, vpCoords, 2);
	vec4 sample4 = texelFetch(screenTextureMS, vpCoords, 3);

    FragColor = (sample1 + sample2 + sample3 + sample4) / 4.0f;

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > bloomThreshold) BrightColor = vec4(FragColor.rgb, 1.0);
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
uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform float gamma;
uniform float exposure;

uniform float bloomIntensity;

void main() { 
    vec3 hdrColor = texture2D(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomIntensity * bloomColor; // additive blending
    // Tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // Gamma correction
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}
)";

/**
 * Fragment Shader Bloom
 */
char DEFAULT_FRAGMENT_BLUR_SHADER[] = R"(
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D image;
  
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform float expansion;

void main() {             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
    tex_offset *= expansion;
    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
)";

illusion::resources::assets::ShaderResource Shader::defaultShaderResource;
Shader Shader::defaultShader;
Shader Shader::featureShader;
Shader Shader::screenShader;

Shader Shader::blurShader;