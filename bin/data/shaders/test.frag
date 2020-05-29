#pragma include "easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime; 

void main( void ) {
   
	vec3 c;
	float l, z = iGlobalTime;
	for(int i=0;i<7;i++) {
		vec2 uv, p = gl_FragCoord.xy/iResolution.xy;
		uv = p;
		p -= .5;
		p.x *= iResolution.x/iResolution.y;
		z += .07;
		l = length(p);
		uv += p/l*(sin(z)+1.)*abs(sin(l*9.-z*4.));
		c[i] = .009/length(abs(mod(uv,1.)-.5));
	}

    gl_FragColor = vec4( c*2, iGlobalTime );
}
