#version 150

// this is how we receive the texture
uniform sampler2DRect pLocalProcessed;
uniform sampler2DRect pRemote;

in vec2 texCoordVarying;

out vec4 outputColor;
 
void main()
{
    //A very simple f() function for green screen is A(r+b) − Bg where A and B are user adjustable constants with a default value of 1.0. A very simple g() is (r, min(g,b), b). This is fairly close to the capabilities of analog and film-based screen pulling. 

    vec4 local = texture(pLocalProcessed, texCoordVarying);
    vec4 remote = texture(pRemote, texCoordVarying);

    float c = mix(local.r, remote.r, 0.5);
    //vec4 c = local + remote;
    
    outputColor = vec4(c,c,0.4-(c),1.);
}