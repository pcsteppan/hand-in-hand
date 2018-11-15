#version 150

// this is how we receive the texture
uniform sampler2DRect pBackground;
uniform sampler2DRect pLocalRaw;
uniform float thresholdA;
uniform float thresholdB;

in vec2 texCoordVarying;

out vec4 outputColor;

float bgsub(vec4 p) {
    return thresholdA * (p.r + p.g) - thresholdB * p.b;
}

float vflip(float pY){
    return 480. - pY;
}

float hflip(float pX){
    return 640.-pX;
}

void main()
{
    //A very simple f() function for green screen is A(r+b) − Bg where A and B are user adjustable constants with a default value of 1.0. A very simple g() is (r, min(g,b), b). This is fairly close to the capabilities of analog and film-based screen pulling. 
    float sideMargin = 150.;
    if(texCoordVarying.x < sideMargin || texCoordVarying.x > (640-sideMargin) || texCoordVarying.y > 460 || texCoordVarying.y < 20){
        outputColor = vec4(0,0,0,1.);
    } else {



        vec4 background = texture(pBackground, texCoordVarying);
        vec4 local = texture(pLocalRaw, vec2(hflip(texCoordVarying.x), vflip(texCoordVarying.y)));
        

        /*
        float a = step(threshold * 3,
            (background.r - local.r) +
            (background.g - local.g) +
            (background.b - local.b)
        );
        float v = local. r + local.g + local.b;
        v /= 3;
        v = 1 - v;
        v *= a;
        */

        float v = bgsub(local);
        
        v = smoothstep(0.05, 1., v);

/*
        if((local.r + local.g + local.b) < 0.28) {
            v = (1 - v); //+= local.b - (local.g + local.r);
            v = smoothstep(0,1, v);
        }
  */
        if ( v > 0 ) {
            v = v * 0.5f + 0.5f;
        }      
        /*
        if ( v > 0 ) {
            v = smoothstep(0, 1, v);
        }
        */
        //v = smoothstep(0.05, 0.2, v);
        //v -= 0.15;
        outputColor = vec4(v,v,v, 1.);
    }
}