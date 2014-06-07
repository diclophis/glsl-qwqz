#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;


void main()
{
  float time = iGlobalTime;
  vec2 resolution = iResolution;
  
  // real fragment position
  vec2 pos = (gl_FragCoord.xy/resolution.xy);
  pos.x *= resolution.y/resolution.x;

  vec2 p = ((gl_FragCoord.xy/resolution.xy) - 0.5) * 4.0;
  p.x *= resolution.y/resolution.x;

  //mouse ranges from 0 to 1 (origin bottom left)
  //resolution is 600
  //fragCoord ranges from 0 to 600 (origin bottom left)
  //mx ranges from 0 to 600 (origin left)
  //my .......................... right
  vec4 grey = vec4(0.2,0.2,0.2,1);
  vec4 blue = vec4(0.0,0.0,1.0,1);
  vec4 gold = vec4(1.0,0.7,0.0,1);
  vec4 fogcolor = mix(blue, gold, .5);
  
  float outcolor = 0.0;
  float horizon = abs(0.5 - pos.y);
  float tileWidth = (0.5 - pos.y);
  bool xFlip = mod(p.x+tileWidth, tileWidth * 2.0) > horizon;
  bool yFlip = mod(pos.y + tileWidth*time, tileWidth*2.0) < horizon;
    
  if (xFlip != yFlip) outcolor = 0.75;


  gl_FragColor = mix(mix(blue, gold, outcolor) * length(p.y) , fogcolor, 0.55);
}
