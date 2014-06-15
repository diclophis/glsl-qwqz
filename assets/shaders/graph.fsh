#ifdef GL_ES
precision mediump float;
#endif

uniform float iGlobalTime;
uniform vec2 iResolution;

uniform sampler2D texture1;
varying vec2 OutTexture;

#define time iGlobalTime

// ---- change scale here ----
float scale = 5.0;

vec3 iMouse = vec3(0.0, 0.0, 0.0);

// ---- change function here -----
float function( vec2 v ) {
  float r = length( v );
  float t = atan( v.y, v.x );
  
  //return sin(r * 10.0) * 0.05;
  //return sin( r + time * 0.7 ) - sin( t * t + time * 0.33 );
  //return sin( r + time ) - cos( t + time * 0.33 );
  //return sin( r + time ) - sin( sin( t * t ) + time * 0.33 );
  return mix(sin(t * time * 0.1), sin(time), sin(time) + 1.0);
}

float value( vec2 p, float size ) {
  float error = size;
  return 1.0 / ( max( abs( function( p ) / error ) - 1.0, 0.0 ) + 1.0 );
}

float grid( vec2 p, float width ) {
  p += width * 0.5;
  
  float grid_width = 1.0;
  float k = 1.0;
  
  k *= step( width * 2.0, abs( p.x ) );
  k *= step( width * 2.0, abs( p.y ) );
  
  grid_width *= 0.25;
  k *= min( step( width, abs( floor( p.x / grid_width + 0.5 ) * grid_width - p.x ) ) + 0.75, 1.0 );
  k *= min( step( width, abs( floor( p.y / grid_width + 0.5 ) * grid_width - p.y ) ) + 0.75, 1.0 );

  return k; 
}

void main(void)
{
  float width = 0.0025;
  vec2 control = iResolution.xy * 0.5;
  
  vec2 uv = ( gl_FragCoord.xy - control ) * width;
  
  float k_grid = grid( uv, width );
  float k_func = value( uv * scale, width * scale );
  
  gl_FragColor = vec4( ( 1.0 - k_func ) * k_grid );
}
