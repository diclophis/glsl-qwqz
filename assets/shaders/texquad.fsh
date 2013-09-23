#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

void main() {
  vec2 uPos = (gl_FragCoord.xy / iResolution.xy) * 1.0;
  vec2 aPos = uPos;
  vec2 bPos = uPos;
  vec2 cPos = uPos;

  aPos.x += iGlobalTime * (0.175);
  aPos.y *= -1.0;

  bPos.x += iGlobalTime * (0.15);
  bPos.y *= -1.0;

  cPos.y *= -1.0;

  vec4 t1 = texture2D(texture1, aPos);
  vec4 t2 = texture2D(texture2, bPos);
  vec4 t3 = texture2D(texture3, cPos);

  vec4 total = vec4(0.0, 0.0, 0.0, 0.0);
  vec4 u_borderColor = vec4(1.0, 0.0, 0.0, 1.0);

  //The step function returns 0.0 if x is smaller then edge and otherwise 1.0.
  //The smoothstep function returns 0.0 if x is smaller then edge0 and 1.0 if x is larger than edge1. 

  float s = step(0.01, 1.0 - t3.a);
  total.rgb = mix(t3.rgb, u_borderColor.rgb, s);

  if (t3.a == 1.0) {
    total.a = t3.a;
  } else {
    total.a = smoothstep(0.4, 0.8, ((t3.a * (1.0 + (sin(iGlobalTime * 2.0))))));
  }

  gl_FragColor = vec4(0.0, 1.0, 0.0, 0.5);
  gl_FragColor = (gl_FragColor * (1.0 - t2.a)) + (t2 * t2.a);
  gl_FragColor = (gl_FragColor * (1.0 - t1.a)) + (t1 * t1.a);
  gl_FragColor = (gl_FragColor * (1.0 - total.a)) + (total * total.a);
}
