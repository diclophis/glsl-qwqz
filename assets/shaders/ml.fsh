// forked from https://www.shadertoy.com/view/MsjSzz

#ifdef GL_ES
precision mediump float;
#endif


// uniforms are provided by CPU land on each draw cycle
uniform float iGlobalTime;
uniform vec2 iResolution;
uniform sampler2D texture1;


// rasterize functions, turns two points into pixels between them
float line(vec2 p, vec2 p0, vec2 p1, float w) {
    vec2 d = p1 - p0;
    float t = clamp(dot(d,p-p0) / dot(d,d), 0.0,1.0);
    vec2 proj = p0 + d * t;
    float dist = length(p - proj);
    dist = 1.0/dist*w;
    return min(dist*dist,1.0);
}


// color math
vec3 hsv(float h, float s, float v) {
    vec3 c = vec3(h, s, v);
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);

}


vec3 rotate_vector( vec4 quat, vec3 vec ) {
  return vec + 2.0 * cross( cross( vec, quat.xyz ) + quat.w * vec, quat.xyz );
}


// takes a screen space point and returns percentage chance its within a hexagon of :s size
vec2 nearestHex(float s, vec2 st) {
    float PI = 3.14159265359;
    float TAU = 2.0*PI;
    float deg30 = TAU/12.0;
    float h = sin(deg30)*s;
    float r = cos(deg30)*s;
    float b = s + 2.0*h;
    float a = 2.0*r;
    float m = h/r;

    vec2 sect = st/vec2(2.0*r, h+s);
    vec2 sectPxl = mod(st, vec2(2.0*r, h+s));
    
    float aSection = mod(floor(sect.y), 2.0);
    
    vec2 coord = floor(sect);
    if (aSection > 0.0) {
        if (sectPxl.y < (h-sectPxl.x*m)) {
            coord -= 1.0;
        } else if(sectPxl.y < (-h + sectPxl.x*m)) {
            coord.y -= 1.0;
        }
    } else {
        if (sectPxl.x > r) {
            if (sectPxl.y < (2.0*h - sectPxl.x * m)) {
                coord.y -= 1.0;
            }
        } else {
            if (sectPxl.y < (sectPxl.x*m)) {
                coord.y -= 1.0;
            } else {
                coord.x -= 1.0;
            }
        }
    }
    
    float xoff = mod(coord.y, 2.0)*r;
    return vec2(coord.x*2.0*r-xoff, coord.y*(h+s))+vec2(r*2.0, s);
}


// more math
float expstep(float a, float b, float x) {
  return exp(-b*pow(a,x));
}


void main(void) {
  // color
  float hue = radians(210.0);
  float sat = 61.5; //70.9;
  float val = 86.7; //91.8;

  // fx init
  float speedOfHexIn = iGlobalTime * 0.0625;
  float speedOfBlurIn = iGlobalTime * 0.0625;
  float speedOfMove = iGlobalTime * 0.0625;
  float speedOfSwitch = (1.0 / 3.0) * speedOfMove;
  float f = 0.0;
  vec3 c = vec3(0.0);
  float hexSize = 0.0;
  vec2 bm = vec2(0.0, 0.0); // center of logo
  float wi = 0.45; // logo width

  // switch timing
  float xt = mod(speedOfSwitch, 3.0);

  // move timing
  float mt = mod(speedOfMove, 3.0);

  // hex timing
  float ht = mod(speedOfHexIn, 3.0);

  // scale timing
  float ct = mod(speedOfHexIn, 3.0);

  // blur timing
  float st = mod(speedOfBlurIn, 3.0);

  // screen position
  vec2 position = gl_FragCoord.xy;

  // scale resolution over time
  vec2 resolution = iResolution.xy;
  resolution.y = resolution.y * 1.0 * (mt / 3.0);
 
  if (xt < 1.0) {
    hexSize = 100.0 * expstep((ht / 3.0), 5.0, 10.0);
  } else if (xt < 2.0) {
    hexSize = 1.0 + (107.0 - (107.0 * (ht / 3.0)));
  } else {
    hexSize = (150.0 * (1.0 - ((floor((ht / 3.0) * (9.0)) / 9.0)))) - ((1.0 / 9.0) * 150.0);
  }

  // tesselate screen space into hexagonal grid
  vec2 uv = nearestHex(hexSize, position.xy) / (resolution);

  // center 0,0 in screen
  uv = (uv * 2.0) - 1.0;
  uv.x *= resolution.x / resolution.y;

  // width of line segment
  float w = 0.005 + ((3.0 - st) * 0.03);


  vec2 tr = vec2((0.3 * wi), 0.25);
  vec2 br = vec2((wi * 0.5), -0.05);
  vec2 tm = vec2(0.0, 0.15);
  vec2 tl = vec2(-(0.3 * wi), 0.25);
  vec2 bl = vec2(-(wi * 0.5), -0.05);

  float sc = 1.0 + (10.0 * ((3.0 - st) / 3.0));

  tr *= sc;
  br *= sc;
  tl *= sc;
  bl *= sc;
  tm *= sc;

  f = line(uv, bm, tr, w);
  c += hsv(hue, sat, f);
  f = line(uv, tr, br, w);
  c += hsv(hue, sat, f);
  f = line(uv, br, tm, w);
  c += hsv(hue, sat, f);
  f = line(uv, tm, bl, w);
  c += hsv(hue, sat, f);
  f = line(uv, bl, tl, w);
  c += hsv(hue, sat, f);
  f = line(uv, tl, tm, w);
  c += hsv(hue, sat, f);
  f = line(uv, tr, tm, w);
  c += hsv(hue, sat, f);
  f = line(uv, tl, bm, w);
  c += hsv(hue, sat, f);
  f = line(uv, bl, bm, w);
  c += hsv(hue, sat, f);
  f = line(uv, br, bm, w);
  c += hsv(hue, sat, f);

  gl_FragColor = vec4(c,1.0);
}
