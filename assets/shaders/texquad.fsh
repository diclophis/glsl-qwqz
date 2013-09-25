#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;


vec3 rainbow(float h) {
  h = mod(mod(h, 1.0) + 1.0, 1.0);
  float h6 = h * 6.0;
  float r = clamp(h6 - 4.0, 0.0, 1.0) +
    clamp(2.0 - h6, 0.0, 1.0);
  float g = h6 < 2.0
    ? clamp(h6, 0.0, 1.0)
    : clamp(4.0 - h6, 0.0, 1.0);
  float b = h6 < 4.0
    ? clamp(h6 - 2.0, 0.0, 1.0)
    : clamp(6.0 - h6, 0.0, 1.0);
  return vec3(r, g, b);
}


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

  float s = step(0.8, t3.a);
  vec4 rainbow = mix(vec4(rainbow(iGlobalTime * 2.0), t3.a), t3, t3.a);

  total.rgb = rainbow.rgb * (1.0 + (2.0 * (1.0 - t3.a)));

  total.a = min(rainbow.a * 2.0, 1.0);

  //vec4 mixer = mix(vec4(0.0);

  //total = (t3 * (1.0 - t3.a)) + (rainbow * rainbow.a);

  //vec4 u_borderColor = vec4(0.75, 0.83, 0.83, 1.0);
  //vec4 u_borderColor = mix(vec4(0.75, 0.83, 0.83, 1.0), vec4(rainbow(iGlobalTime * 0.1), 1.0), 0.5);

  //75.0  83.0  83.0  
  //76.0  82.0  82.0  

  //The step function returns 0.0 if x is smaller then edge and otherwise 1.0.
  //The smoothstep function returns 0.0 if x is smaller then edge0 and 1.0 if x is larger than edge1. 
  /*
  float s = step(0.01, 1.0 - t3.a);
  total.rgb = mix(t3.rgb, u_borderColor.rgb, s);

  if (t3.a == 1.0) {
    total.a = t3.a;
  } else {
    total.a = smoothstep(0.01, 0.99, ((t3.a * (1.0 + (sin(iGlobalTime * 1.0))))));
  }

  total.a = t3.a;
  */

  gl_FragColor = vec4(0.0, 1.0, 0.0, 0.5);
  gl_FragColor = (gl_FragColor * (1.0 - t2.a)) + (t2 * t2.a);
  gl_FragColor = (gl_FragColor * (1.0 - t1.a)) + (t1 * t1.a);
  gl_FragColor = (gl_FragColor * (1.0 - total.a)) + (total * total.a);
}
