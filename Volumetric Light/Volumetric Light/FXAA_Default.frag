#version 420
uniform sampler2D uSourceTex;
uniform vec2 RCPFrame;
varying vec2 vTexCoord;
layout(location=0) out vec4 col;
float PIE_MY=3.1415926f;
vec4 GaussFilter()
{
	vec2 uv0=vec2(vTexCoord.x-2*RCPFrame.x,vTexCoord.y-2*RCPFrame.y);
	vec4  col=vec4(0.0f,0.0f,0.0f,0.0f);
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			vec2 uv_temp=uv0+vec2(i*RCPFrame.x,j*RCPFrame.y);
			float pow_temp=exp(-(pow(i-2,2)+pow(j-2,2))/2.0f)/(2.0f*PIE_MY);
			//pow_temp*=82.0f*PIE_MY;
			col+=pow_temp*texture2D(uSourceTex,uv_temp);
		}
	}
	//col/=173.0f;
	return col;
}
void FXAA()
{
	float FXAA_SPAN_MAX = 8.0;
	float FXAA_REDUCE_MUL = 1.0/8.0;
	float FXAA_REDUCE_MIN = 1.0/128.0;
	vec2 frameBufSize=vec2(1.0f/RCPFrame.x,1.0f/RCPFrame.y);
	vec3 rgbNW=texture2D(uSourceTex,vTexCoord+(vec2(-1.0f,-1.0f)/frameBufSize)).xyz;
	vec3 rgbNE=texture2D(uSourceTex,vTexCoord+(vec2(1.0f,-1.0f)/frameBufSize)).xyz;
	vec3 rgbSW=texture2D(uSourceTex,vTexCoord+(vec2(-1.0f,1.0f)/frameBufSize)).xyz;
	vec3 rgbSE=texture2D(uSourceTex,vTexCoord+(vec2(1.0f,1.0f)/frameBufSize)).xyz;
	vec3 rgbM=texture2D(uSourceTex,vTexCoord).xyz;

	vec3 luma=vec3(0.299f, 0.587f, 0.114f);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot(rgbM,  luma);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce = max(
						  (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
						  FXAA_REDUCE_MIN);

	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

	dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
			  max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
				  dir * rcpDirMin)) / frameBufSize;

	vec3 rgbA = (1.0/2.0) * (
							 texture2D(uSourceTex, vTexCoord + dir * (1.0/3.0 - 0.5)).xyz +
							 texture2D(uSourceTex, vTexCoord + dir * (2.0/3.0 - 0.5)).xyz);
	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
												texture2D(uSourceTex, vTexCoord + dir * (0.0/3.0 - 0.5)).xyz +
												texture2D(uSourceTex, vTexCoord + dir * (3.0/3.0 - 0.5)).xyz);
	float lumaB = dot(rgbB, luma);
	if((lumaB < lumaMin) || (lumaB > lumaMax)) 
	{
		gl_FragColor.xyz=rgbA;
	}
	else
	{
		gl_FragColor.xyz=rgbB;
	}
}
void main(void)
{
	col=GaussFilter();
}
