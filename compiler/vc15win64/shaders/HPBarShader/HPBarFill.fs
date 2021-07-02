#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform float progress;
uniform float length;
uniform float leftTopX;
//uniform float leftTopY;
//uniform float rightBottomX;
//uniform float rightBottomY;

void main()
{
	if(gl_FragCoord.x - leftTopX > progress/100.0 * length){
		discard;
	}
	color = vec4(1 - (gl_FragCoord.x-leftTopX)/length*1.0, 1.0, 0.0, 0.8);
}