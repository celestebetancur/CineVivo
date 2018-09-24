#pragma include "shaders/easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime; 
uniform float     iChannelTime[4];  
uniform vec4      iMouse;           
uniform sampler2D iChannel0; 

void main(void){  
 gl_FragColor = vec4(0.3,abs(sin(iGlobalTime*10.0)),0.0,1.0);
}