#version 330 core

//传入的颜色值
in float alpha;

//采样器 
uniform sampler2D  texture1;

out vec4 fragColor;

void main(void)
{
    //表示当前所在点图元的二维坐标点
   // vec4 tempColor = texture2D(texture1, gl_PointCoord) * sColor;
    vec4 tempColor = texture2D(texture1, gl_PointCoord);
	 if(tempColor.a < 0.1)
        discard;
	tempColor.w = tempColor.w * alpha;
	fragColor = tempColor;
}
