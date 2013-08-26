  // Created by inigo quilez - iq/2013
  // License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
  #ifdef GL_ES
  precision highp float;
  #endif
  uniform float iGlobalTime;
  uniform vec2 iResolution;
  //uniform vec2 iChannel0;
  //uniform sampler2D iChannel0;
  #define FULL_PROCEDURAL
  #ifdef FULL_PROCEDURAL
    // hash based 3d value noise
    float hash( float n )
    {
        return fract(sin(n)*43758.5453);
    }
    float noise( in vec3 x )
    {
        vec3 p = floor(x);
        vec3 f = fract(x);

        f = f*f*(3.0-2.0*f);
        float n = p.x + p.y*57.0 + 113.0*p.z;
        return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                       mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                   mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                       mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    }
  #else
    // LUT based 3d value noise
    float noise( in vec3 x )
    {
      vec3 p = floor(x);
      vec3 f = fract(x);
      f = f*f*(3.0-2.0*f);
      vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
      vec2 rg = texture2D(iChannel0, (uv+ 0.5)/256.0, -100.0 ).yx;
      return mix( rg.x, rg.y, f.z );
    }
  #endif

  vec4 map( in vec3 p )
  {
    float d = 0.2 - p.y;
    vec3 q = p - vec3(1.0,0.1,0.0)*iGlobalTime * 0.1;
    float f;
    f  = 0.5000*noise( q ); q = q*2.02;
    f += 0.2500*noise( q ); q = q*2.03;
    f += 0.1250*noise( q ); q = q*2.01;
    f += 0.0625*noise( q );
    d += 3.0 * f;
    d = clamp( d, 0.0, 1.0 );
    vec4 res = vec4( d );
    res.xyz = mix( 1.15*vec3(1.0,0.95,0.8), vec3(0.7,0.7,0.7), res.x );
    return res;
  }

  vec3 sundir = vec3(-1.0,0.0,0.0);

  vec4 raymarch( in vec3 ro, in vec3 rd )
  {
    vec4 sum = vec4(0, 0, 0, 0);
    float t = 0.0;
    for(int i=0; i<44; i++)
    {
      vec3 pos = ro + t*rd;
      vec4 col = map( pos );
      float dif =  clamp((col.w - map(pos+0.3*sundir).w)/0.6, 0.0, 1.0 );
      vec3 brdf = vec3(0.65,0.68,0.7)*1.35 + 0.45*vec3(0.7, 0.5, 0.3)*dif;
      col.xyz *= brdf;
      col.a *= 0.35;
      col.rgb *= col.a;
      sum = sum + col*(1.0 - sum.a);  
      t += max(0.1,0.05*t);
    }
    sum.xyz /= (0.001+sum.w);
    return clamp( sum, 0.0, 1.0 );
  }

  void main(void)
  {
    //vec2 iMouse = vec2(10.0 + sin(iGlobalTime * 0.3) * 10.0, 20.0 + (15.0 * sin(iGlobalTime * 0.33)));
    vec2 iMouse = vec2(0, 0); //10.0 + sin(iGlobalTime * 0.3) * 10.0, 20.0 + (15.0 * sin(iGlobalTime * 0.33)));
    vec2 q = gl_FragCoord.xy / iResolution.xy;
    vec2 p = -1.0 + 2.0*q;
    p.x *= iResolution.x/ iResolution.y;
    vec2 mo = -1.0 + 2.0*iMouse.xy / iResolution.xy;
    
    // camera
    vec3 ro = 4.0*normalize(vec3(cos(2.75-3.0*mo.x), 0.7+(mo.y+1.0), sin(2.75-3.0*mo.x)));
    vec3 ta = vec3(0.0, 2.0, 0.0);
    vec3 ww = normalize( ta - ro);
    vec3 uu = normalize(cross( vec3(0.0,1.0,0.0), ww ));
    vec3 vv = normalize(cross(ww,uu));
    vec3 rd = normalize( p.x*uu + p.y*vv + 1.5*ww );

    vec4 res = raymarch( ro, rd );

    float sun = clamp( dot(sundir,rd), 0.0, 1.0 );
    vec3 col = vec3(0.6,0.71,0.75) - rd.y*0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;
    col += 0.2*vec3(1.0,.6,0.1)*pow( sun, 8.0 );
    col *= 0.95;
    col = mix( col, res.xyz, res.w );
    col += 0.1*vec3(1.0,0.4,0.2)*pow( sun, 3.0 );
      
    gl_FragColor = vec4( col, 1.0 );
  }
