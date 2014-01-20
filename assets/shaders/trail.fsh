#ifdef GL_ES
precision mediump float;
#endif

// bars - thygate@gmail.com
// rotation and color mix modifications by malc (mlashley@gmail.com)
// modified by @hintz 2013-04-30

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

float time = iGlobalTime;
vec2 mouse = vec2(0.0, 0.0);
vec2 resolution = iResolution;

vec2 position;
float c = cos(time*0.0);
float s = sin(time*0.0);
mat2 R = mat2(c,-s,s,c);
float barsize = 0.15;
float barsangle = 1.0*sin(0.05);
vec4 bar(float pos, float r, float g, float b)
{
  return max(0.0, 1.0 - abs(pos - position.y + 0.1*sin(2.0*time+sin(1.0-pos*2.0)*4.0*position.x)) / barsize) * vec4(r, g, b, 1.0);
}
void main(void) 
{
  position = (gl_FragCoord.xy - 0.5*resolution.xy) / resolution.xx;
  position = 4.0*position * R;    
  float t = time*0.0;
  vec4 color = bar(sin(t), 1.0, 0.0, 0.0);
  color += bar(sin(t+barsangle*2.), 1.0, 0.5, 0.0);
  color += bar(sin(t+barsangle*4.), 1.0, 1.0, 0.0);
  color += bar(sin(t+barsangle*6.), 0.0, 1.0, 0.0);
  color += bar(sin(t+barsangle*8.), 0.0, 1.0, 1.0);
  color += bar(sin(t+barsangle*10.), 0.0, 0.0, 1.0);
  color += bar(sin(t+barsangle*12.), 0.5, 0.0, 1.0);
  color += bar(sin(t+barsangle*14.), 1.0, 0.0, 1.0);  
  gl_FragColor = color;
  gl_FragColor.a = 1.0;
}
