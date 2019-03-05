#pragma include "easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime; 
uniform float     iChannelTime[4];  
uniform vec4      iMouse;           
uniform sampler2D iChannel0;

// bonniemathew@gmail.com

vec3 COLOR1 = vec3(0.0, 0.0, 0.50);
vec3 COLOR2 = vec3(0.30, .0, 0.0);

float BLOCK_WIDTH = 0.01;

void main( void ) {
    
    
    vec2 position = ( gl_FragCoord.xy / iResolution.xy );
    vec3 final_color = vec3(1.0);
    
    // For creating the BG pattern
    float c1 = mod(position.x, 2.0 * BLOCK_WIDTH);
    c1 = step(position.x*0.01, c1);
    float c2 = mod(position.y, 2.0 * BLOCK_WIDTH);
    c2 = step(BLOCK_WIDTH, c2);
    
    final_color = mix( position.x * COLOR1,  position.y * COLOR2, c1 * c2);
    
    
    // creating the wave
    position = -1.0 + 2.0 * position;
    float lineWidth = 10.0;
    vec2 sPos = position ;
    for( float i = 0.0; i < 20.; i++) {
        sPos.y += ((0.07*cos(iGlobalTime)) * tan(position.x + i/5.0+ sin(iGlobalTime)));
        
        lineWidth  =  abs(1.0 / (750.0 * sPos.y));
        final_color = final_color + vec3(lineWidth * 0.9, lineWidth*1.9, lineWidth * 0.);
        
    }
    
    gl_FragColor = vec4(final_color, 1.0);
}
