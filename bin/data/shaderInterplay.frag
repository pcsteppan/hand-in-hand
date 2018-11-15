#version 150

// this is how we receive the texture
uniform sampler2DRect pLocalProcessed;
uniform sampler2DRect pRemote;

in vec2 texCoordVarying;

out vec4 outputColor;
 
float vflip(float pY){
    return 480. - pY;
}



void main()
{
    //A very simple f() function for green screen is A(r+b) − Bg where A and B are user adjustable constants with a default value of 1.0. A very simple g() is (r, min(g,b), b). This is fairly close to the capabilities of analog and film-based screen pulling. 

    vec4 local = texture(pLocalProcessed, texCoordVarying);
    vec4 remote = texture(pRemote, vec2(texCoordVarying.x, vflip(texCoordVarying.y)));

    //float c = local.r * remote.r;
    float c = mix(local.r, remote.r, 0.5);
    c *= 1.1;
    //vec4 c = local + remote;
    
    outputColor = vec4(c*1.5,c*1.,1.2-(c),1.);
}