precision highp float;
attribute vec3 aVertexPosition;
attribute vec3 aVertexNormal;
attribute vec2 aVertexUV;

uniform mat4 uNormalMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallnessMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAOMap;
uniform sampler2D uDisplacementMap;

uniform float uDisplacementMapActive;

varying vec3 vNormal; //normal
varying vec3 vFragPos;
varying vec2 vFragUV;

void main(void) {
    vFragPos = vec3(uModelMatrix * vec4(aVertexPosition, 1.0));
    vNormal = normalize( (uNormalMatrix * vec4(aVertexNormal, 1.0)).xyz );
    vFragUV = aVertexUV;

    if(uDisplacementMapActive > 0.0){
        gl_Position = uProjectionMatrix * uViewMatrix * vec4(vFragPos + vNormal * texture2D(uDisplacementMap, vFragUV).x * 0.1, 1.0);
    }else{
        gl_Position = uProjectionMatrix * uViewMatrix * vec4(vFragPos, 1.0);
    }
    
}