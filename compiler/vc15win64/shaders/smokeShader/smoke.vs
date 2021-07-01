#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float random01;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float  dy;
uniform float  maxY;
uniform float  pointSize;

out float alpha;

void main(void)
{
    //传递顶点
    vec3 vNewPosition = position;

	vNewPosition.y += dy; //设置新的位置的y值要加上new_dy

	gl_PointSize = pointSize * random01;
	alpha = 1.0;
    //粒子爬升时开始渐隐
	if(position.y + maxY/5 < vNewPosition.y)
    alpha = smoothstep(maxY + position.y, maxY/5 + position.y, vNewPosition.y);

    gl_Position = projection * view * model * vec4(vNewPosition, 1.0); 
}