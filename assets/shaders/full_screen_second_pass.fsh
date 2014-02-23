#ifdef GL_ES
precision highp float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main() {
  vec2 uv = gl_FragCoord.xy / iResolution.xy;
  gl_FragColor = texture2D( texture1, uv );

}
