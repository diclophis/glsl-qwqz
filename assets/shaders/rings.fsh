#ifdef GL_ES
precision mediump float;
#endif

//uniform float time;
//uniform vec2 mouse;
//uniform vec2 resolution;

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

#define time iGlobalTime
#define resolution iResolution

vec3 hsv(float h,float s,float v) {
	return mix(vec3(1.),clamp((abs(fract(h+vec3(3.,2.,1.)/3.)*6.-3.)-1.),0.,1.),s)*v;
}

float circle(vec2 p, float r) {
	return smoothstep(0.1, 0.0, abs(length(p)-r)); // try changing the 0.1 to 0.3
}

float r3 = sqrt(3.0);

void main(void) {
	vec2 uv = -1.0 + 2.0*gl_FragCoord.xy / iResolution.xy;
	uv.x *= iResolution.x/iResolution.y;
	uv *= (1.0 * (tan(sin(iGlobalTime * 0.25)) * 8.0) + 8.0);
	float r = smoothstep(-0.7, 0.7, sin(iGlobalTime*1.57-length(uv)*0.1))+1.0;
	vec2 rep = vec2(4.0,r3*4.0);
	vec2 p1 = mod(uv, rep)-rep*0.5;
	vec2 p2 = mod(uv+vec2(2.0,0.0), rep)-rep*0.5;
	vec2 p3 = mod(uv+vec2(1.0,r3), rep)-rep*0.5;
	vec2 p4 = mod(uv+vec2(3.0,r3), rep)-rep*0.5;
	vec2 p5 = mod(uv+vec2(0.0,r3*2.0), rep)-rep*0.5;
	vec2 p6 = mod(uv+vec2(2.0,r3*2.0), rep)-rep*0.5;
	vec2 p7 = mod(uv+vec2(1.0,r3*3.0), rep)-rep*0.5;
	vec2 p8 = mod(uv+vec2(3.0,r3*3.0), rep)-rep*0.5;
	
	float c = 0.0;
	c += circle(p1, r);
	c += circle(p2, r);
	c += circle(p3, r);
	c += circle(p4, r);
	c += circle(p5, r);
	c += circle(p6, r);
	c += circle(p7, r);
	c += circle(p8 , r);
	gl_FragColor = vec4(hsv(r, 1.0, c), 1.0);
}
