#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float alpha;
uniform float blingValue;

void main()
{
	vec4 factorColor = vec4(blingValue, 0.0, 0.0, alpha);
	color = texture(image, TexCoords) * factorColor;
}