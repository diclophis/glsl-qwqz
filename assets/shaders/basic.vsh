//
#ifdef GL_ES
precision highp float;
#endif

attribute vec2 Position;

attribute vec2 Texture;
varying vec2 OutTexture;

uniform mat4 ModelViewProjectionMatrix;

void main() {
  //vec2 p = Position * 0.005;
  float zoom = 1.0; //(iResolution.xy / vec2(1024.0, 1024.0));
  vec4 p = ModelViewProjectionMatrix * vec4(Position * zoom, 1.0 * zoom, 1.0);
  gl_Position = p;
  //gl_Position = ModelViewProjectionMatrix * vec4(Position * 1.0, 1.0, 1.0);
  //gl_Position = vec4(p, 1.0, 1.0);
  OutTexture = Texture;
}
