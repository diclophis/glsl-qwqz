#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main() {
  gl_FragColor = vec4(0.0);
  vec4 t2 = texture2D(texture1, OutTexture);

  vec4 color = t2; //(gl_FragColor * (1.0 - t2.a)) + (t2 * t2.a);

  //color *= (1.0 * mod(gl_FragCoord.y, 2.0));
  //color *= 0.000; //(0.1 * mod(gl_FragCoord.y, 4.0));

  gl_FragColor = color;

  /*
  vec2 uv = OutTexture;
  vec2 divs = vec2(iResolution.x * iGlobalTime * 10.0 / iResolution.y, iGlobalTime * 10.0);
  uv = floor(uv * divs)/ divs;
  gl_FragColor = texture2D(texture1, uv);
  */

}
