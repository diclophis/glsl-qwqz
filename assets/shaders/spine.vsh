//

attribute vec2 Position;

attribute vec2 Texture;
varying vec2 OutTexture;

uniform mat4 ModelViewProjectionMatrix;

void main() {
  float zoom = 0.5;
  vec4 p = ModelViewProjectionMatrix * vec4(Position * zoom, 1.0, 1.0);
  p.y -= 0.5;
  gl_Position = p;
  OutTexture = Texture;
}
