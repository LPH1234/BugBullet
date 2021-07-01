#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform float progress;
uniform float length;
uniform float leftTopX;
uniform float leftTopY;
uniform float rightBottomX;
uniform float rightBottomY;

void main()
{

	vec4 fillColor = vec4(1.0,1.0,1.0,1.0);
	if(gl_FragCoord.x - leftTopX < progress)
			fillColor = vec4(1 - (gl_FragCoord.x-leftTopX)/length*1.0, 1.0, 0.0, 0.5);
	//if(point.x > 0.6)
//	if(gl_FragCoord.x > 200.0)
	//	discard;
	
	//color = texture(image, TexCoords) * fillColor;
	color = vec4(1 - (gl_FragCoord.x-leftTopX)/length*1.0, 1.0, 0.0, 0.8);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}