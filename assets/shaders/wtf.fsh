#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main( void ) {

vec2 position = gl_FragCoord.xy / iResolution.y * 3.0 - vec2(iResolution.x / iResolution.y, 1.0);

vec3 col = vec3(0, 0, 0);

  float time = iGlobalTime * 0.01;

for (int i = 0; i < 1; ++i) {
float mt = mod(time, 999.0) * 0.3 * (float(i) * 0.3);
float t = mod(time * 0.4, 1111.0) * (float(i) + 2.0);
float x2 = cos(t) * 0.7 + sin(mt) * 0.02;
float y2 = sin(t) * 0.7 + cos(mt) * 0.02;
float x = cos(t/0.3) * x2 + sin(t/0.3) * y2;
//float y = sin(t/0.3) * x2 + cos(t/0.3) * y2;
x = x2;
float y = y2;
vec2 d3 = position - vec2(x, y);

float tt = t * (float(i) + 5.0) / 20.0 + d3.x * tan(t*1.5) + d3.y * tan(t*2.0);
vec2 d = vec2(d3.x * cos(tt) + d3.y * sin(tt), d3.x * sin(tt) - d3.y * cos(tt));

float rx = 0.7 - (abs(sin(t * 0.5)) / 2.0);
float ry = 0.7 - (abs(tan(t * 100.5)) / 2.0);
d.x *= rx;
d.y *= ry;

float d2 = dot(d, d);
float a = abs(d2 - 0.03);

float u = mod(time*2.0, 1111.0) * (float(i) * 0.1 + 2.0);
col.r += 0.005 * sin(u) * sin(u) / (a + 0.00001);
if (d2 < 0.03)
col.r += 0.3 * sin(u) * sin(u);

u = mod(time*1.9, 1111.0) * (float(i) * 0.1 + 2.0);
col.g += 0.005 * cos(u) * cos(u) / (a + 0.00001);
if (d2 < 0.03)
col.g += 0.3 * sin(u) * sin(u);

u = mod(time*1.8, 1111.0) * (float(i) * 0.1 + 2.0);
col.b += 0.005 * sin(u+0.787) * sin(u+0.787) / (a + 0.00001);
if (d2 < 0.03)
col.b += 0.3 * sin(u) * sin(u);
}
gl_FragColor = vec4(col, 1.0);
}
