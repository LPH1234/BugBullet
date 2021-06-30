#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float  dy;
uniform float  maxY;

out float alpha;

void main()
{
	//传递顶点
    vec3 vNewPosition = position;

	vNewPosition.y += dy; //设置新的位置的y值要加上new_dy

	alpha = 1.0;
    //粒子爬升时开始渐隐
	if(vNewPosition.y > position.y + maxY/5)
		alpha = smoothstep(maxY + position.y, maxY/5 + position.y, vNewPosition.y);


    gl_Position = projection * view * model * vec4(vNewPosition, 1.0f);
    TexCoords = texCoords;
}