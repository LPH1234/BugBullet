#version 330 core

layout(location = 0) in vec3 velocity;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float  currTime;
uniform float  pointSize;
uniform float  radis;
uniform float  alpha1;
uniform vec3  initPos;
uniform vec3  cameraPos;

out float alpha;

void main(void)
{
	alpha = 1.0;
	float fade_size = 2.0;
	vec3 upVec = vec3(0.0, -8.0, 0.0);
    //传递顶点
    vec3 vNewPosition = initPos + velocity * currTime + upVec * currTime;
	float dist = distance(initPos, cameraPos);
    //设置点的大小，高度越高，越小 pointSize : 0      4 : 1000
	float tempPointSize = pointSize - dist/70;
	gl_PointSize = tempPointSize * alpha1;
	//gl_PointSize = tempPointSize * alpha1;
		
    // If they are very small, fade them up
    //判断如果粒子太小了，就会发生闪烁，所以我们要去使这个点的颜色逐渐变暗，从而使它们在视野中消失。
    if(gl_PointSize < fade_size)
        alpha = smoothstep(0.0, fade_size, gl_PointSize);
	alpha = alpha * alpha1;
    gl_Position = projection * view * model * vec4(vNewPosition, 1.0); 
}