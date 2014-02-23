#ifdef GL_ES
precision highp float;
#endif

attribute vec2 Position;

attribute vec2 Texture;
varying vec2 OutTexture;

uniform float iGlobalTime;
uniform vec2 iResolution;
uniform mat4 ModelViewProjectionMatrix;

uniform sampler2D texture1;

attribute vec2 surfacePosAttrib;
varying vec2 surfacePosition;

void main() {
//  surfacePosition = vec2(0, 0); //surfacePosAttrib;
//  gl_Position = vec4( Position, 1.0, 1.0 );
//  OutTexture = Texture;
  float zoom = (iResolution.y / 512.0);
  vec4 p = ModelViewProjectionMatrix * vec4(Position * zoom, 1.0 * zoom, 1.0);
  gl_Position = p; //vec4(Position, 1.0, 1.0);
  OutTexture = Texture;
}
