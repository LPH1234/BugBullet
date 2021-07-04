#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float alpha;
uniform float blingValue;

void main()
{ // * alpha
	vec4 tempColor = texture(image, TexCoords);
	if(tempColor.a < 0.1)
        discard;
	vec4 factorColor = vec4(1.0, blingValue, blingValue, alpha);
	color =  tempColor * factorColor;
}