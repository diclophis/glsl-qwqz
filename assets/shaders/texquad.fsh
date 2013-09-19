#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

//varying vec2 v_texCoord;
uniform sampler2D texture1;

void main() {
  vec2 uPos = (gl_FragCoord.xy / iResolution.xy) * 1.0;
  uPos.x += iGlobalTime;
  uPos.y *= -1.0;
  gl_FragColor = texture2D(texture1, uPos);
}
