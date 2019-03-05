#pragma include "easings.frag"

 // from https://www.geeks3d.com/20110316/shader-library-simple-2d-effects-sphere-and-ripple-in-glsl

uniform vec3      iResolution;
uniform float     iGlobalTime; 
uniform float     iChannelTime[4];  
uniform vec4      iMouse;           
uniform sampler2D iChannel0;

void main( void ) {
    
    vec2 pos=(gl_FragCoord.xy/iResolution.xy)*2.0-1.0;
    pos.x*= iResolution.x/iResolution.y;
    
    float col=1.75;

    col=mod(pos.x,0.2)+mod(pos.y,0.2);
    
    float len=length(pos);
    vec2 uv=vec2(col)+(pos/len)*cos(len*2.0-iGlobalTime*4.0)*0.8;
    vec3 color=vec3(col*abs(sin(iGlobalTime))/2.0,uv);
    gl_FragColor=vec4(color,1.0);
}
