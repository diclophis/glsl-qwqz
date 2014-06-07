#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main() {
  gl_FragColor = vec4(0.0);
  vec4 t2 = texture2D(texture1, OutTexture);

  gl_FragColor = (gl_FragColor * (1.0 - t2.a)) + (t2 * t2.a);

  /*
  vec2 uv = OutTexture;
  vec2 divs = vec2(iResolution.x * iGlobalTime * 10.0 / iResolution.y, iGlobalTime * 10.0);
  uv = floor(uv * divs)/ divs;
  gl_FragColor = texture2D(texture1, uv);
  */

}
