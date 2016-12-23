#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main() {
  //vec2 aPos = (gl_FragCoord.xy);
  //vec2 aPos = (gl_FragCoord.xy-iResolution.xy)/iResolution.y;
  vec2 aPos = (gl_FragCoord.xy) / iResolution.xy;

  vec4 t1 = texture2D(texture1, aPos);
  vec4 t2 = texture2D(texture1, OutTexture);

  gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
  gl_FragColor = (gl_FragColor * (1.0 - t1.a)) + (t1 * t1.a);
}
