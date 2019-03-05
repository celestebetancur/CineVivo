#pragma include "easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime; 
uniform float     iChannelTime[4];  
uniform vec4      iMouse;           
uniform sampler2D iChannel0;

void main()
{
    vec3 p = vec3((gl_FragCoord.xy)/(iResolution.y),sin(iGlobalTime));
    for (int i = 0; i < 8; i++)
    {
        p.xzy = vec3(1.3,0.999,0.7)*(abs((abs(p)/dot(p,p)-vec3(1.0,1.0,cos(iGlobalTime)*0.5))));
    }
    gl_FragColor.rgb = p;gl_FragColor.a = 1.0;
}
