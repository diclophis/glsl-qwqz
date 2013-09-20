#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
  vec2 uPos = (gl_FragCoord.xy / iResolution.xy) * 1.0;
  vec2 aPos = uPos;
  vec2 bPos = uPos;

  aPos.x += iGlobalTime * (0.75);
  aPos.y *= -1.0;

  bPos.x += iGlobalTime * (0.5);
  bPos.y *= -1.0;

  vec4 t1 = texture2D(texture1, aPos);
  vec4 t2 = texture2D(texture2, bPos);

  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
  gl_FragColor = (gl_FragColor*(1.0-t2.a))+(t2*t2.a);
  gl_FragColor = (gl_FragColor*(1.0-t1.a))+(t1*t1.a);
}
