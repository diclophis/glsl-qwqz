#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

#define N 20
#define NF float(N)
#define PI 3.14
#define HPI PI/2.0

void main( void ) {
  float time = iGlobalTime;
  vec2 mouse = vec2(0, 0);
  vec2 resolution = iResolution;
  
  vec2 PixelCoord = (gl_FragCoord.xy/resolution.xy);
  vec3 PixelColor = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < N; i++)
  {
    vec2 ParticlePos = vec2(0.5, 0.5);
    ParticlePos.x += 0.25*sin(0.0 + float(i) / NF);
    ParticlePos.y += 0.25*cos(time + float(i) / NF);
    float ParticleSize = 0.0001 * float(i);
    PixelColor.r += ParticleSize * (0.5/length(ParticlePos-PixelCoord));
    PixelColor.g += ParticleSize * (1.0/length(ParticlePos-PixelCoord) * float(i)/NF);
    PixelColor.b += 2.5 * ParticleSize * (1.0/length(ParticlePos-PixelCoord) * float(i)/NF);
  }
  
  
  gl_FragColor = vec4(PixelColor, 1.0);
}
