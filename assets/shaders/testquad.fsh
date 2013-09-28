#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main() {
  vec2 uPos = (gl_FragCoord.xy / iResolution.xy) * 1.0;
  vec2 aPos = uPos;
  aPos.x += iGlobalTime;

  vec4 t1 = texture2D(texture1, aPos);

  gl_FragColor = vec4(0.0, 1.0, 0.0, 0.5);
  gl_FragColor = (gl_FragColor * (1.0 - t1.a)) + (t1 * t1.a);

}
