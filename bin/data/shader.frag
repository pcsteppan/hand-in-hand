#version 150

// this is how we receive the texture
uniform sampler2DRect localHand;
uniform sampler2DRect remoteHand;
uniform sampler2DRect bg;

in vec2 texCoordVarying;

out vec4 outputColor;
 
void main()
{
    //A very simple f() function for green screen is A(r+b) − Bg where A and B are user adjustable constants with a default value of 1.0. A very simple g() is (r, min(g,b), b). This is fairly close to the capabilities of analog and film-based screen pulling. 

    vec4 local = texture(localHand, texCoordVarying);
    vec4 background = texture(bg, texCoordVarying);
    local.a = step(0.5,
        (local.r - background.r) +
        (local.g - background.g) +
        (local.b - background.b)
    );
    vec4 remote = texture(remoteHand, texCoordVarying);

    // float A = 0.9;
    // float B = 0.4;

    // float lf = A * (l.r + l.b) - B * (l.g);
    // l.a = 1 - step(lf, l.g);

    // float rf = A * (r.r + r.b) - B * (r.g);
    // r.a = 1 - step(rf, r.g);

    // l.g = min(l.g, l.b);
    // r.g = min(r.g, r.b);

    vec4 c = mix(local, remote, 0.5);
    outputColor = c;
}