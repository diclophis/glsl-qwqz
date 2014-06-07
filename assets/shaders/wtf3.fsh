#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;



void main ( void ){
	float time = iGlobalTime;
  vec2 resolution = iResolution;
  
  vec2 uv = gl_FragCoord.xy/resolution;
  
	vec3 color;
	color.r = sin(uv.x*5.+0.+time*2.);
	color.g = sin(uv.x*5.-2.0-time*2.);
	color.b = sin(uv.x*5.-4.+time*2.);
		
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	float scale = 1.0;
	float pwr = scale - 0.2*scale*sin(time);
	float a = pow(sin(pos.x*3.1415),pwr)*pow(sin(pos.y*3.1415),pwr);
  
	gl_FragColor = mix(vec4(vec3(a),1.0), vec4(color,1.), 0.1);
		
}
