#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main() {
  vec2 uPos = (gl_FragCoord.xy / iResolution.xy) * 0.5;
  vec2 aPos = uPos;

  vec4 t1 = texture2D(texture1, aPos);

  gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  gl_FragColor = (gl_FragColor * (1.0 - t1.a)) + (t1 * t1.a);

}
