//

attribute vec2 Position;

void main() {
  vec2 p = Position;
  p.x = 1.0 * Position.x;
  p.y = 1.0 * Position.y;
  gl_Position = vec4(p, 1.0, 1.0);
}
