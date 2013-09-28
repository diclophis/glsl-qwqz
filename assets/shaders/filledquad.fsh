#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main() {
  gl_FragColor = vec4(0.0);
  vec4 t2 = texture2D(texture1, OutTexture);
  gl_FragColor = (gl_FragColor * (1.0 - t2.a)) + (t2 * t2.a);
}
