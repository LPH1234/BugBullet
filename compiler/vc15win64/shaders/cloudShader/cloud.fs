#version 330 core
in vec2 TexCoords;
in float alpha;

out vec4 color;

uniform sampler2D texture1;

void main()
{
    //color = texture(texture1, TexCoords);
	vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
	texColor.w = texColor.w * alpha;
    color = texColor;
}