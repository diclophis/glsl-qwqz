#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

uniform vec2 iTextureOffset;

void main() {
  gl_FragColor = vec4(0.0);
  vec4 t2 = texture2D(texture1, OutTexture + iTextureOffset);
  vec4 color = t2;
  
  gl_FragColor = color;
}
