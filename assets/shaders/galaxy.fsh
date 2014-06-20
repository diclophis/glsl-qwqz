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

// https://www.shadertoy.com/view/MdXSzS

void main()
{
	
	//vec2 uv = (gl_FragCoord.xy/vec2(1024.0, 1024.0))-.5;

  float width = 0.00198765;
  vec2 control = iResolution.xy * 0.5;
  vec2 uv = ( gl_FragCoord.xy - control ) * width;

	float time = time * .3 + ((.25+.05*sin(time*.1))/(length(uv.xy)+.07))* 2.2;
	float si = sin(time);
	float co = cos(time);
	mat2 ma = mat2(co, si, -si, co);

	float c = 0.0;
	float v1 = 0.0;
	float v2 = 0.0;
	
	for (int i = 0; i < 32; i++)
	{
		float s = float(i) * .035;
		vec3 p = s * vec3(uv, 0.0);
		p.xy *= ma;
		p += vec3(.22,.3, s-1.5-sin(sin(time * 2.0) * time *.13)*.1);
		for (int i = 0; i < 6; i++)
		{
			p = abs(p) / dot(p,p) - 0.659;
		}
		v1 += dot(p,p)*.0015 * (1.8+sin(length(uv.xy*13.0)+.5-time*.2));
		v2 += dot(p,p)*.0015 * (1.5+sin(length(uv.xy*13.5)+2.2-time*.3));
		c = length(p.xy*.5) * .35;
	}
	
	float len = length(uv);
	v1 *= smoothstep(.7, .0, len);
	v2 *= smoothstep(.6, .0, len);
	
	float re = clamp(c, 0.0, 1.0);
	float gr = clamp((v1+c)*.25, 0.0, 1.0);
	float bl = clamp(v2, 0.0, 1.0);
	vec3 col = vec3(re, gr, bl) + smoothstep(0.15, .0, len) * .9;

	gl_FragColor=vec4(col, 1.0);
}
