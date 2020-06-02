
#pragma include "easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime;
uniform float     iChannelTime[4];
uniform vec4      iMouse;
uniform sampler2D iChannel0;

vec4 noise(vec2 _st, float scale, float offset){
    return vec4(vec3(_noise(vec3(_st*scale, offset*iGlobalTime))), 1.0);
}

vec4 voronoi(vec2 _st, float scale, float speed, float blending){
    vec3 color = vec3(0.0);
 // Scale
    _st *= scale;
 // Tile the space
    vec2 i_st = floor(_st);
    vec2 f_st = fract(_st);
    float m_dist = 10.0;  // minimun distance
    vec2 m_point;        // minimum point
    for (int j=-1; j<=1; j++ ) {
        for (int i=-1; i<=1; i++ ) {
            vec2 neighbor = vec2(float(i),float(j));
            vec2 p = i_st + neighbor;
            vec2 point = fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
            point = 0.5 + 0.5*sin(iGlobalTime*speed + 6.2831*point);
            vec2 diff = neighbor + point - f_st;
            float dist = length(diff);
            if( dist < m_dist ) {
                m_dist = dist;
                m_point = point;
            }
        }
    }
 // Assign a color using the closest point position
 color += dot(m_point,vec2(.3,.6));
 color *= 1.0 - blending*m_dist;
 return vec4(color, 1.0);
 }

vec4 osc(vec2 _st, float freq, float sync, float offset){
    vec2 st = _st;
    float r = sin((st.x-offset*2.0/freq+iGlobalTime*sync)*freq)*0.5  + 0.5;
    float g = sin((st.x+iGlobalTime*sync)*freq)*0.5 + 0.5;
    float b = sin((st.x+offset/freq+iGlobalTime*sync)*freq)*0.5  + 0.5;
    return vec4(r, g, b, 1.0);
}

vec4 shape(vec2 _st, float sides, float radius, float smoothing){
    vec2 st = _st * 2. - 1.;
    float a = atan(st.x,st.y) + PI;
    float r = (2.* PI)/sides;
    float d = cos(floor(.5+a/r)*r-a)*length(st);
    return vec4(vec3(1.0-smoothstep(radius,radius + smoothing,d)), 1.0);
}

vec4 gradient(vec2 _st, float speed){
    return vec4(_st, sin(iGlobalTime*speed), 1.0);
}

vec4 solid (float r, float g, float b, float a){
    return vec4(r, g, b, a);
}

vec2 rotate(vec2 _st, float angle, float speed){
    vec2 xy = _st - vec2(0.5);
    float ang = angle + speed * iGlobalTime;
    xy = mat2(cos(angle),-sin(ang), sin(ang),cos(ang))*xy;
    xy += 0.5;
    return xy;
}

vec2 scale(vec2 _st, float amount, float xMult, float yMult, float offsetX, float offsetY){
    vec2 xy = _st - vec2(offsetX, offsetY);
    xy*=(1.0/vec2(amount*xMult, amount*yMult));
    xy+=vec2(offsetX, offsetY);
    return xy;
}

vec2 pixelate (vec2 _st, float pixelX, float pixelY){
    vec2 xy = vec2(pixelX, pixelY);
    return (floor(_st * xy) + 0.5)/xy;
}

vec4 posterize(vec4 _c0, float bins, float gamma){
    vec4 c2 = pow(_c0, vec4(gamma));
    c2 *= vec4(bins);
    c2 = floor(c2);
    c2/= vec4(bins);
    c2 = pow(c2, vec4(1.0/gamma));
    return vec4(c2.xyz, _c0.a);
}

vec4 shift(vec4 _c0, float r, float g, float b, float a){
    vec4 c2 = vec4(_c0);
    c2.r = fract(c2.r + r);
    c2.g = fract(c2.g + g);
    c2.b = fract(c2.b + b);
    c2.a = fract(c2.a + a);
    return vec4(c2.rgba);
}

vec2 repeat(vec2 _st, float repeatX, float repeatY, float offsetX, float offsetY){
    vec2 st = _st * vec2(repeatX, repeatY);
    st.x += step(1., mod(st.y,2.0)) * offsetX;
    st.y += step(1., mod(st.x,2.0)) * offsetY;
    return fract(st);
}

vec2 modulateRepeat(vec2 _st, vec4 _c0, float repeatX, float repeatY, float offsetX, float offsetY){
    vec2 st = _st * vec2(repeatX, repeatY);
    st.x += step(1., mod(st.y,2.0)) + _c0.r * offsetX;
    st.y += step(1., mod(st.x,2.0)) + _c0.g * offsetY;
    return fract(st);
}

vec2 repeatX (vec2 _st, float reps, float offset){
    vec2 st = _st * vec2(1.0, reps);
    //  float f =  mod(_st.y,2.0);
    st.x += step(1., mod(st.x,2.0))* offset;
    return fract(st);
}

vec2 modulateRepeatX(vec2 _st, vec4 _c0, float reps, float offset){
    vec2 st = _st * vec2(reps,1.0);
    st.y += step(1.0, mod(st.x,2.0)) + _c0.r * offset;
    return fract(st);
}

vec2 repeatY (vec2 _st, float reps, float offset){
    vec2 st = _st * vec2(1.0, reps);
    //  float f =  mod(_st.y,2.0);
    st.y += step(1., mod(st.y,2.0))* offset;
    return fract(st);
}

vec2 modulateRepeatY(vec2 _st, vec4 _c0, float reps, float offset){
    vec2 st = _st * vec2(reps,1.0);
    st.x += step(1.0, mod(st.y,2.0)) + _c0.r * offset;
    return fract(st);
}

vec2 kaleid(vec2 _st, float nSides){
    vec2 st = _st;
    st -= 0.5;
    float r = length(st);
    float a = atan(st.y, st.x);
    float pi = 2.* PI;
    a = mod(a,pi/nSides);
    a = abs(a-pi/nSides/2.);
    return r*vec2(cos(a), sin(a));
}

//vec2 modulateKaleid

vec2 scroll(vec2 _st, float scrollX, float scrollY, float speedX, float speedY){
    _st.x += scrollX + iGlobalTime*speedX;
    _st.y += scrollY + iGlobalTime*speedY;
    return _st;
}


vec2 scrollX (vec2 _st, float scrollX, float speed){
    _st.x += scrollX + iGlobalTime*speed;
    return _st;
}

// vec2 modulateScrollX

vec2 scrollY (vec2 _st, float scrollY, float speed){
    _st.y += scrollY + iGlobalTime*speed;
    return _st;
}

// vec2 modulateScrollX

vec4 add(vec4 _c0, vec4 _c1, float amount){
    return (_c0+_c1)*amount + _c0*(1.0-amount);
}

vec4 sub(vec4 _c0, vec4 _c1, float amount){
    return (_c0-_c1)*amount + _c0*(1.0-amount);
}
vec4 layer(vec4 _c0, vec4 _c1){
    return vec4(mix(_c0.rgb, _c1.rgb, _c1.a), _c0.a+_c1.a);
}

vec4 blend(vec4 _c0, vec4 _c1, float amount){
    return _c0*(1.0-amount)+_c1*amount;
}

vec4 mult(vec4 _c0, vec4 _c1, float amount){
    return _c0*(1.0-amount)+(_c0*_c1)*amount;
}

vec4 diff(vec4 _c0, vec4 _c1, float amount){
    return vec4(abs(_c0.rgb-_c1.rgb), max(_c0.a, _c1.a));
}

vec2 modulate(vec2 _st, vec4 _c0, float amount){
    return _st + _c0.xy * amount;
}

vec2 modulateScale(vec2 _st, vec4 _c0, float offset, float multiple){
    vec2 xy = _st - vec2(0.5);
    xy*=(1.0/vec2(offset + multiple*_c0.r, offset + multiple*_c0.g));
    xy+=vec2(0.5);
    return xy;
}

vec2 modulatePixelate(vec2 _st, vec4 _c0, float offset, float multiple){
    vec2 xy = vec2(offset + _c0.x*multiple, offset + _c0.y*multiple);
    return (floor(_st * xy) + 0.5)/xy;
}

vec2 modulateRotate(vec2 _st, vec4 _c0, float offset, float multiple){
    vec2 xy = _st - vec2(0.5);
    float angle = offset + _c0.x * multiple;
    xy = mat2(cos(angle),-sin(angle), sin(angle),cos(angle))*xy;
    xy += 0.5;
    return xy;
}

vec2 modulateHue(vec2 _st,vec4 _c0, float amount){
    vec2 t = _st + vec2(_c0.g - _c0.r, _c0.b - _c0.g) * amount / iResolution.xy;
    return t;
}

vec4 invert(vec4 _c0, float amount){
    return vec4((1.0-_c0.rgb)*amount + _c0.rgb*(1.0-amount), _c0.a);
}

vec4 cotrast(vec4 _c0, float amount){
    vec4 c = (_c0-vec4(0.5))*vec4(amount) + vec4(0.5);
    return vec4(c.rgb, _c0.a);
}

vec4 brightness(vec4 _c0, float amount){
    return vec4(_c0.rgb + vec3(amount), _c0.a);
}

//mask

//luma

//thresh

vec4 color(vec4 _c0, float r, float g, float b, float a){
    vec4 c = vec4(r, g, b, a);
    vec4 pos = step(0.0, c); // detect whether negative
    // if > 0, return r * _c0
    // if < 0 return (1.0-r) * _c0
    return vec4(mix((1.0-_c0)*abs(c), c*_c0, pos));
}

vec4 r(vec4 _c0, float scale, float offset){
    return vec4(_c0.r * scale + offset);
}

vec4 g(vec4 _c0, float scale, float offset){
    return vec4(_c0.g * scale + offset);
}

vec4 b(vec4 _c0, float scale, float offset){
    return vec4(_c0.b * scale + offset);
}

vec4 a(vec4 _c0, float scale, float offset){
    return vec4(_c0.a * scale + offset);
}

vec4 saturate(vec4 _c0, float amount){
    const vec3 W = vec3(0.2125,0.7154,0.0721);
    vec3 intensity = vec3(dot(_c0.rgb,W));
    return vec4(mix(intensity,_c0.rgb,amount), _c0.a);
}

//hue

vec4 colorama(vec4 _c0, float amount){
    vec3 c = _rgbToHsv(_c0.rgb);
    c += vec3(amount);
    c = _hsvToRgb(c);
    c = fract(c);
    return vec4(c, _c0.a);
}

void main()
{
    vec2 p = gl_FragCoord.xy/iResolution.xy;
    vec2 k = kaleid(p,4.0);
    vec2 r = rotate(k,0.5,1.0);
    vec2 h = modulateHue(r,osc(p,1.0,1.0,1.0),50.0);
    //vec4 n = colorama(noise(h,10.5,0.5),sin(iGlobalTime));
    //vec4 r = osc(modulateHue(k,n,50.0),20.0,0.1,0.1);
    gl_FragColor = texture2D(iChannel0,h);
}
