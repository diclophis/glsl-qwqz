//
#ifdef GL_ES
precision highp float;
#endif

attribute vec2 Position;

attribute vec2 Texture;
varying vec2 OutTexture;

uniform float iGlobalTime;
uniform vec2 iResolution;
uniform mat4 ModelViewProjectionMatrix;

void main() {
  float zoom = (iResolution.y / 512.0);
  vec4 p = ModelViewProjectionMatrix * vec4(Position * zoom, 1.0 * zoom, 1.0);
  gl_Position = p;
  OutTexture = Texture;
}
