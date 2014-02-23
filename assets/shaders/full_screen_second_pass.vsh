attribute vec2 Position;

attribute vec2 Texture;
varying vec2 OutTexture;

uniform float iGlobalTime;
uniform vec2 iResolution;
uniform mat4 ModelViewProjectionMatrix;

uniform sampler2D texture1;

void main() {

  gl_Position = vec4( Position, 1.0, 1.0 );
  OutTexture = Texture;

}
