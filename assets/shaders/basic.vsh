//

attribute vec2 Position;

attribute vec2 Texture;
varying vec2 OutTexture;

//uniform mat4 ModelViewProjectionMatrix;

void main() {
  vec2 p = Position;
  p.x = 0.01 * Position.x;
  p.y = (0.01 * Position.y) - 0.5;
  gl_Position = vec4(p, 1.0, 1.0);

  OutTexture = Texture;
}
