
#pragma include "easings.frag"

uniform vec3      iResolution;
uniform float     iGlobalTime;
uniform float     iChannelTime[4];
uniform vec4      iMouse;
uniform sampler2D iChannel0;

vec4 invert(vec4 _c0, float amount){
    return vec4((1.0-_c0.rgb)*amount + _c0.rgb*(1.0-amount), _c0.a);
}

vec2 scale(vec2 _st, float amount, float xMult, float yMult, float offsetX, float offsetY){
    vec2 xy = _st - vec2(offsetX, offsetY);
    xy*=(1.0/vec2(amount*xMult, amount*yMult));
    xy+=vec2(offsetX, offsetY);
    return xy;
}

void main()
{
    vec2 p = gl_FragCoord.xy/iResolution.xy;
    vec4 cam = texture2D(iChannel0,p);
    gl_FragColor = invert(cam,5.0);
}
