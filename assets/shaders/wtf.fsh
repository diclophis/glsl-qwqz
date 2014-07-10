#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

void main( void ) {

	vec2 p = ( gl_FragCoord.xy / iResolution.xy ) - 1.0;
	p.y *= iResolution.y/iResolution.x; 
		
	vec3 col = vec3(0);
	
	p.x = floor(p.x*20.0)/20.0;
	p.y += iGlobalTime*1.0*(p.x+1.03);
	p.y = floor(p.y*20.0)/20.0;
	float n1 = fract(123123.25423*sin(31231.23123*p.x*p.y)); 
	float n2 = fract(123432.42543*sin(21231.23123*p.x*p.y)); 
	float n3 = fract(123544.43242*sin(11231.23123*p.x*p.y)); 
	col = vec3(n1,n2,n3);
	gl_FragColor = vec4(col, 1.0); 
}
