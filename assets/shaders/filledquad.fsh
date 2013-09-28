#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

void main() {
  vec2 uPos = (gl_FragCoord.xy / iResolution.xy) * 1.0;

  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
