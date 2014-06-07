#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

#define N 6
#define NF float(N)
#define PI 3.14
#define HPI PI/2.0

void main( void ) {
  float time = iGlobalTime;
  vec2 mouse = vec2(0, 0);
  vec2 resolution = iResolution;
  
  //vec2 PixelCoord = (gl_FragCoord.xy/resolution.xy);
  float width = 1.0 / iResolution.x; //1.0 / max(iResolution.x, iResolution.y);
  vec2 control = vec2(0.0, 0.0); //iResolution.xy; //vec2(.0, 0.0); //mix( iResolution.xy * 0.5, iMouse.xy, 1.0 - step( iMouse.z, 0.0 ) );
  
  vec2 PixelCoord = ( gl_FragCoord.xy - control ) * width;
  
  vec3 PixelColor = vec3(0.0, 0.0, 0.0);
  int i=0;
  for (int i = 0; i < N; i++)
  {
    vec2 ParticlePos = vec2(0.5, 0.25);
    ParticlePos.x += 0.25 * (float(i) / NF); //0.25; //*sin(0.0 + float(i) / NF);
    ParticlePos.y += 0.125 * cos((time * 1.0) + ((float(i) / NF) * 1.0));
    float ParticleSize = 0.001 * float(i);
    PixelColor.r += ParticleSize * (0.5/length(ParticlePos-PixelCoord));
    PixelColor.g += ParticleSize * (1.0/length(ParticlePos-PixelCoord) * float(i)/NF);
    PixelColor.b += 2.5 * ParticleSize * (1.0/length(ParticlePos-PixelCoord) * float(i)/NF);
  }
  
  
  gl_FragColor = vec4(PixelColor, length(PixelColor.rgb) * 0.25);
}
