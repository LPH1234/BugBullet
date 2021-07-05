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
    //���ݶ���
    vec3 vNewPosition = initPos + velocity * currTime + upVec * currTime;
	float dist = distance(initPos, cameraPos);
    //���õ�Ĵ�С���߶�Խ�ߣ�ԽС pointSize : 0      4 : 1000
	float tempPointSize = pointSize - dist/70;
	gl_PointSize = tempPointSize * alpha1;
	//gl_PointSize = tempPointSize * alpha1;
		
    // If they are very small, fade them up
    //�ж��������̫С�ˣ��ͻᷢ����˸����������Ҫȥʹ��������ɫ�𽥱䰵���Ӷ�ʹ��������Ұ����ʧ��
    if(gl_PointSize < fade_size)
        alpha = smoothstep(0.0, fade_size, gl_PointSize);
	alpha = alpha * alpha1;
    gl_Position = projection * view * model * vec4(vNewPosition, 1.0); 
}