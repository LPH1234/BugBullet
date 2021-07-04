#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float alpha;

void main()
{ // * alpha
	vec4 tempColor = texture(image, TexCoords);
	if(tempColor.a < 0.1)
        discard;
	color =  tempColor;
}