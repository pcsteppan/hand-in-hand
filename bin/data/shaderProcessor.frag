#version 150

// this is how we receive the texture
uniform sampler2DRect pBackground;
uniform sampler2DRect pLocalRaw;
uniform float thresholdB;
uniform float thresholdA;
//uniform float sideMargin;
//uniform float topMargin;

in vec2 texCoordVarying;

out vec4 outputColor;

float bgsub(vec4 p) {
    return (p.r + p.g) * thresholdA - thresholdB * p.b;
}

float vflip(float pY){
    return 480. - pY;
}

float hflip(float pX) {
    return 640. - pX;
}

void main()
{
    float sideMargin = 190;
    float topMargin = 68;

    if(texCoordVarying.x < sideMargin || texCoordVarying.x > (640-sideMargin) || texCoordVarying.y > 480 - (topMargin)){
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
        v = smoothstep(0.01,1.0,v);
        if (v > 0){
                    v = v * 0.5 + 0.5;
        }
        /*
        if((local.r + local.g + local.b) < 0.15) {
            v = 1 - v; //+= local.b - (local.g + local.r);
        }
        */
        //v = smoothstep(0.05, 0.2, v);
        //v -= 0.1;
        outputColor = vec4(v,v,v, 1.);
    }
}