precision mediump float;

attribute vec2 vertPosition; //input
attribute vec3 vertColor; //input

varying fragColor; //output

void main()
{
    fragColor = vertColor;
    gl_Position = vec4(vertPosition, 0.0, 1.0);

}
