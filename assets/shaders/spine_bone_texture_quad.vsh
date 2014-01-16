//

attribute vec2 Position;

attribute vec2 Texture;
varying vec2 OutTexture;

uniform mat4 ModelViewProjectionMatrix;
uniform vec2 iResolution;

void main() {
  float zoom = (iResolution.y / 1024.0);
  vec4 p = ModelViewProjectionMatrix * vec4(Position * zoom, 1.0, 1.0);
  p.y -= 1.0;
  gl_Position = p;
  OutTexture = Texture;
}
