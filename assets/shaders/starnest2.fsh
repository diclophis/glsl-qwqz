#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

// Star Nest by Pablo Román Andrioli
// Modified a lot.

// This content is under the MIT License.

#define iterations 16
#define formuparam 0.43

#define volsteps 16
#define stepsize 0.21

#define zoom   1.0
#define tile   01.50
#define speed  5.0

#define brightness 0.00550
#define darkmatter 1.00
#define distfading 0.360
#define saturation 0.00


void main(void)
{

float time = iGlobalTime * speed;
vec2 mouse = vec2(0.0);
vec2 resolution = iResolution;

  //get coords and direction
  vec2 uv=gl_FragCoord.xy/resolution.xy-.5;
  uv.y*=resolution.y/resolution.x;
  vec3 dir=vec3(uv*zoom,1.);
  
  float a2=time;
  float a1=.0;
  mat2 rot1=mat2(cos(a1),sin(a1),-sin(a1),cos(a1));
  mat2 rot2=rot1;//mat2(cos(a2),sin(a2),-sin(a2),cos(a2));
  dir.xz*=rot1;
  dir.xy*=rot2;
  
  //from.x-=time;
  //mouse movement
  vec3 from=vec3(0.,0.,0.);
  from+=vec3(.05*time,.0*time,-2.);
  
  //from.x-=mouse.x;
  //from.y-=mouse.y;
  
  from.y -= 200.;
  
  from.xz*=rot1;
  from.xy*=rot2;
  
  //volumetric rendering
  float s=.4,fade=.2;
  vec3 v=vec3(0.4);
  for (int r=0; r<volsteps; r++) {
    vec3 p=from+s*dir*.7;
    p = abs(vec3(tile)-mod(p,vec3(tile*2.))); // tiling fold
    float pa,a=pa=0.;
    
    for (int i=0; i < iterations; i++) { 
      p=abs(p)/dot(p,p)-1.1*formuparam; // the magic formula
      a+=abs(length(p)-pa); // absolute sum of average change
      pa=length(p);
    }
    
    float dm=max(0.,darkmatter-a*a*.001); //dark matter
    a*=a*a*1.; // add contrast
    if (r>0) fade*=1.-dm; // dark matter, don't render near
    //v+=vec3(dm,dm*.5,0.);
    v+=fade;
    v+=vec3(s,s*s,s*s*s*s)*a*brightness*fade; // coloring based on distance
    fade*=distfading; // distance fading
    s+= stepsize;
  }
  v=mix(vec3(length(v)),v,saturation); //color adjust
  gl_FragColor = vec4(v*.01, 1.); 
}
