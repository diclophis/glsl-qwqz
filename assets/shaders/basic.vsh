//

attribute vec2 Position;

void main() {
  vec2 p = Position;
  p.x = 0.001 * Position.x;
  p.y = (0.001 * Position.y) - 0.5;
  gl_Position = vec4(p, 1.0, 1.0);

  //gl_Position = vec4(Position, 1.0, 1.0);
}
