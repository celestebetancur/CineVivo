#pragma include "easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime; 
uniform float     iChannelTime[4];  
uniform vec4      iMouse;           
uniform sampler2D iChannel0;

void main()
{
    vec2 p = 2.0 * gl_FragCoord.xy/iResolution.xy;
    for (int n=1; n<30; n++) {
        float i = float(n);
        p += vec2(0.6/i*sin(i*p.y)+iGlobalTime/300.,  0.8/i*sin(i*p.x) + iGlobalTime / 200.);
    }    
    gl_FragColor = vec4(vec3(0.3*sin(p.x)*sin(p.y)+0.7), 1.0);
}
