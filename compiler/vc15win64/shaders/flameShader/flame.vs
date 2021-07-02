#version 330 core

layout(location = 0) in vec3 position;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float  dy;
uniform float  maxY;
uniform float  pointSize;
uniform float  radis;
uniform float  alpha1;
uniform vec3  initPos;

out float alpha;

void main(void)
{
    //���ݶ���
    vec3 vNewPosition = position;
	alpha = 1.0;

	float dist = sqrt((position.x - initPos.x)*(position.x - initPos.x) + (position.z - initPos.z)*(position.z - initPos.z));

	if(dist < radis/10) dist = radis/10;
	float dyFactor = (1 - dist/(1.5*radis));
	if(dyFactor <= 0)
		dyFactor = 0.1;
	float new_dy = dy * dyFactor;
	float yFactor = (1 - dist/(1.1*radis));
	if(yFactor <= 0)
		yFactor = 0.1;
	float new_maxY = maxY * yFactor;
    while(new_dy > new_maxY){
		new_dy -= new_maxY;
	}

	vNewPosition.y += new_dy; //�����µ�λ�õ�yֵҪ����new_dy

    //���õ�Ĵ�С���߶�Խ�ߣ�ԽС
	gl_PointSize = pointSize * (1 - new_dy/new_maxY);

		
    // If they are very small, fade them up
    //�ж��������̫С�ˣ��ͻᷢ����˸����������Ҫȥʹ��������ɫ�𽥱䰵���Ӷ�ʹ��������Ұ����ʧ��
	float fade_size = 4.0;
    if(gl_PointSize < fade_size)
        alpha = smoothstep(0.0, fade_size, gl_PointSize);
	alpha = alpha * alpha1;
    gl_Position = projection * view * model * vec4(vNewPosition, 1.0); 
}