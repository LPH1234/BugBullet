#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float progress;
uniform float length;

void main()
{
    //color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
	vec2 point = gl_PointCoord;
	//if(point.x > 0.6)
//	if(gl_FragCoord.x > 200.0)
	//	discard;
	//color = texture(image, TexCoords);
	color = vec4(1 - gl_FragCoord.x/length*1.0, 1.0, 0.0, 0.8);

}