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
    vec4 sand = vec4(0.76,0.7,0.5,1.);
    vec4 umber = vec4(0.39, 0.32, 0.28, 1.);
    vec4 prussian_blue = vec4(0., 0.05, 0.07, 1.);
    vec4 ochre = vec4(0.8, 0.47, 0.13, 1.);


    vec4 local = texture(pLocalProcessed, texCoordVarying);
    vec4 remote = texture(pRemote, vec2(texCoordVarying.x, vflip(texCoordVarying.y)));

    float m = mix(local.r, remote.r, 0.5);
    // screen
    float c = 1 - (1.-local.r)*(1.-remote.r);

    //float c = 0;
    // overlay (screen + multiply)
    /*
    if ( local.r > 0.5 ){
        c = 1 - (1.-local.r) * (1.-remote.r);
    } else {
        c = local.r * remote.r;
    }
    */

    /*
    vec4 color = vec4(0.5, 0.5, 0.5, 1.);
    
    if ( c < 0.25) {
        color = mix(color, prussian_blue, 1 - smoothstep(0.1, 0.25, c));
    }
    if ( c > 0.1 && c < 0.5 ) {
        color = mix(color, umber, smoothstep(0.1, 0.5, c));
    }
    if (c > 0.5 && c < 0.95) {
        color = mix(color, umber, 1 - smoothstep(0.7, 0.95, c));
    }
    if ( c > 0.9) {
        color = mix(color, ochre, smoothstep(0.9, 0.99, c));
    }
    */

    //vec4 c = local + remote;
    
    outputColor = vec4(c,m /2,1 - (c*1.2),1); //color; //vec4(c*1.2,c*1.,1.2-(c),1.);
}
/*
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

    float c = mix(local.r, remote.r, 0.5);
    float r = 0;
    if (c > 0.5) {
        r = 2;
    }
    //vec4 c = local + remote;
    
    outputColor = vec4((1.5 - c)*r,0.8,1.5 - c,1.);
}
*/