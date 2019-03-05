#pragma include "easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime; 
uniform float     iChannelTime[4];  
uniform vec4      iMouse;           
uniform sampler2D iChannel0;

void main( void ) {
    
    vec2 p = ( gl_FragCoord.xy / iResolution.xy );
    p = 2.0 * p - 1.0;
    p.x *= iResolution.x / iResolution.y;
    p*=0.07;
    float color = 0.0;
    
    float dp = length(p);
    
    vec2 q = mod(sin(p * 3.141592 * 2.0) - 0.5, 1.0) - 0.5;
    
    vec2 r = mod(cos(q * 3.141592 * 7.0) - 0.5, 1.0) - 0.5;
    
    float dq = length(q);
    float dr = length(r);
    float w1 = sin(iGlobalTime*1.5 + 5.0 * dq * 3.141592) * 1. ;
    float w2 = cos(8.2 * dr * 3.141592*sin(dq*33. - dr*w1*1.3 + w1*dp*3. + iGlobalTime*1.1)+iGlobalTime*4.) * 1. ;
    
    color = w1*dr - w2*dq + dp*dr*sin(iGlobalTime)*5.;
    
    vec3 c;
    
    c.r = color;
    c.g = color*(w1*dr*cos(w2*dq));
    c.b = c.g - (color-(w1-w2)*dq);
    
    gl_FragColor = vec4( c, 1.0 );
}
