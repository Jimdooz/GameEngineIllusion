precision highp float;
// Fragment-Interpolated data
varying vec3 vNormal;
varying vec3 vFragPos; // (p) dans le cours
varying vec2 vFragUV;

// Camera Position
// //Camera  v = p - cam
// v = vFragPos - uViewPos
uniform vec3 uViewPos;
// Light
uniform vec3 uLightPos; // p_i = vFragPos - uLightPos
uniform vec3 uLightColor; //Couleur

// Material
//uniform vec3 uObjectColor;
//uniform vec3 uShininess;

uniform mat4 uNormalMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

uniform vec3 uAlbedo;
uniform float uRoughness;
uniform float uMetalness;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallnessMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAOMap;
uniform sampler2D uDisplacementMap;
uniform sampler2D uEmissionMap;

uniform float uAlbedoMapActive;
uniform float uNormalMapActive;
uniform float uMetalnessMapActive;
uniform float uRoughnessMapActive;
uniform float uAOMapActive;
uniform float uDisplacementMapActive;
uniform float uEmissionMapActive;

#define PI 3.14159265359

#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)

//PBR
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    vec3 NxH = cross(N, H);
    float NoH = max(dot(N, H), 0.0);
    float a = NoH * roughness;
    float k = roughness / (dot(NxH, NxH) + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float HdotV, vec3 F0){
    return F0 + (vec3(1.0) - F0) * pow(1.0 - HdotV, 5.0);
}

void main() {
    vec2 uv = vFragUV.xy;

    vec3 albedo     = uAlbedoMapActive > 0.0 ? texture2D(uAlbedoMap, vFragUV).rgb : uAlbedo;
    vec3 normal     = uNormalMapActive > 0.0 ? texture2D(uNormalMap, vFragUV).rgb : vNormal;
    float metallic  = uMetalnessMapActive > 0.0 ? texture2D(uMetallnessMap, vFragUV).r : uMetalness;
    float roughness = uRoughnessMapActive > 0.0 ? texture2D(uRoughnessMap, vFragUV).r : uRoughness;
    float ao        = uAOMapActive > 0.0 ? texture2D(uAOMap, vFragUV).r : 1.0;
    vec3 emission   = uEmissionMapActive > 0.0 ? texture2D(uEmissionMap, vFragUV).rgb : vec3(0.0);

    normal = normalize(normal * 2.0 - 1.0);

    //Ne pas oublier de normaliser les vecteurs
    vec3 N = normalize(vNormal);
    //vec3 N = - normalize(vNormal * texture2D(uNormalMap, vFragUV).xyz);
    vec3 V = normalize(uViewPos - vFragPos); //Vue : normalisé pour avoir entre 0 et 1

    vec3 T;
    vec3 B;
    vec3 c1 = cross(N, vec3(0.0, 0.0, 1.0));
    vec3 c2 = cross(N, vec3(0.0, 1.0, 0.0));
    vec3 c3 = cross(N, vec3(1.0, 0.0, 0.0));
    T = length(c1) > length(c2) ? (length(c1) > length(c3) ? c1 : c3) : (length(c2) > length(c3) ? c2 : c3);
    T = c2;
    T = normalize(T);
    B = normalize(cross(N, T));

    mat3 TBN = mat3(B, T, N);

    N = normalize(TBN * normal);

    //Energy : Radiance
    vec3 Lo = vec3(0.0); //Valeur finale utilisé à la fin de programme , il va calculer la lumière

    //Chose à répéter pour toutes les lumières
    //----------------> for(int i = 0; i < 1; i++){

    // HalfWay vector
    vec3 L = normalize(uLightPos - vFragPos); //Light
    vec3 H = normalize(V + L);

    // Frenel 0 = la lumière redistribué : lié à la matière qui varie si il est métallique ou diéléctrique
    // Pour la faire varier on va utiliser le mix
    vec3 F0 = vec3(0.04);
    //Faire varier entre la couleur et la valeur métalique ( soit totalement métallique soit la couleur totale de l'objet de base)
    F0 = mix(F0, albedo, metallic);

    //--> équation de réflectance
    // Radiance de la lumière : atténuation émise par la couleur en 0
    float distance = length(uLightPos - vFragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = uLightColor * attenuation;

    // écriture du BRDF spéculaire
    float NDF = DistributionGGX(N, H, roughness); //Normal Distribution Function
    float G = GeometrySmith(N, V, L, roughness); //Geometry
    // Le dot product peut être négatif or on le veut entre 0 et 1
    // Les valeurs négatives ne sont pas visible donc on peut les ommettres
    vec3 F = fresnelSchlick( max(dot(H, V), 0.0) , F0); //Reflextance

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(0.0, dot(N, V)) * max(0.0, dot(N, L));
    // Pour éviter une division par 0 lorsque le dénominateur = 0;
    vec3 specular = numerator / max(denominator, 0.001);

    //--> Calcul de la part d'énergie de radiance calculé par l'energie spéculaire
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    //La metalness élimine la partie diffuse de la BRDF, on a une information de la metalness pour F0
    //La partie diffuse ne doit pas être prise en compte
    //Pour ne pas utiliser des if -> utiliser un système smooth entre metallique ou non
    kD *= (1.0 - metallic);

    float NdotL = max(0.0, dot(N, L));

    //(diffuse de lambert) + (partie Spéculaire)
    //Lo += (texture2D(uNormalMap, vFragUV).xyz * numerator); //Pas besoin de multiplier par KS car déjà multiplié par
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    //----------------> }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
	
    //Tonnemapping
    color = color / (color + vec3(1.0));
    //Gamma Correction
    color = pow(color, vec3(1.0/2.2));

    //On applique à la surface
    //gl_FragColor = vec4(uAlbedoMapActive, 1);
    gl_FragColor = vec4(Lo + emission, 1.0);
    //gl_FragColor = vec4(Lo, 1.0);
}