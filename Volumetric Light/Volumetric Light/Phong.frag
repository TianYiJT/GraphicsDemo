#version 420 core
in vec3 N;
in vec3 view;
uniform vec3 lightpos;
uniform vec4 lightcolor;
uniform sampler2D Depth;
uniform sampler2D Normal_Texture;
uniform int shader_type;
uniform vec3 cameraposition;
uniform float LineWidth;
layout(location=0) out vec4 col;
vec4 mycolor=vec4(0.3f,0.3f,0.0f,1.0f);
float _diffuse=0.4f;
float _specular=0.7f;
float _ambient=0.4f;
float r0=2.0f*LineWidth;
int sr=4;
int sh=32;
float PIE_MY=3.1415926f;
float LinearizeDepth(in vec2 uv)
{
    float zNear = 1.0f;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 500.0f; // TODO: Replace by the zFar  of your perspective projection
    float depth = texture2D(Depth, uv).x;
	depth=depth*2-1;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}
float LinearizeDepth(float d)
{
    float zNear = 1.0f;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 50.0f; // TODO: Replace by the zFar  of your perspective projection
    float depth = d*2-1;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}
float hx(float x)
{
	return 3*pow(x,2)-2*pow(x,3);
}

float zoom(int j)
{
	return r0*j;//+float(j)*1.5f;
}
vec2 tran(in vec2 v2)
{
	vec2 vreturn=vec2(v2.x/800.0f,v2.y/600.0f);
	return vreturn;
}
float Visiable(in vec2 where)
{
	if(LinearizeDepth(tran(where))-LinearizeDepth(tran(gl_FragCoord.xy))<0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	//return texture(Depth,gl_FragCoord.st).r;
}

float gd(vec2 where,int j)
{
	/*
	float temp=float(j)/float(sr);
	float tl=pow(1.0f-temp,2.0f);
	float t0=0.0001f;
	float d_temp=LinearizeDepth(tran(gl_FragCoord.xy))-LinearizeDepth(tran(where));
	if(d_temp>tl)
	{
		return 0.0f;
	}
	else if(d_temp<t0)
	{
		return 1.0f;
	}
	else
	{
		return 1-hx((LinearizeDepth(tran(where))-t0)/(tl-t0));
	}
	*/
	float d_temp=texture(Depth,tran(gl_FragCoord.xy)).r-texture(Depth,tran(where)).r;
	if(d_temp>0)
	{
		return 1-hx(d_temp);
	}
	else
	{
		return 0.0f;
	}
}
vec3 getNormal(vec2 my,vec2 last) 
{
	vec3 mlast = vec3(last.xy,1.0f);
	vec3 mmy =vec3(my.xy,1.0f);
	vec3 tangent = mlast - mmy;
	vec3 view = cameraposition.xyz - mmy;
	vec3 vCrosst =cross(tangent,view);
	return normalize(cross(normalize(vCrosst),tangent));
}
float glight(vec2 where)
{
	
	vec3 H=normalize(normalize(lightpos)+normalize(view));
	float spec=dot(H,normalize(N));
	spec=clamp(spec,0,1);
	spec=pow(spec,8);	
	return 1.0f-spec;
}
float count_LineAO()
{
	float LineAO_result=0.0f;
	for(int i=0;i<sr;i++)
	{
		float temp_result=0;
		float temp_r=zoom(i);
		float interval=PIE_MY*float(2)/float(sh);
		for(int j=0;j<sh;j++)
		{
			float x=temp_r*sin(interval*j+float(i)*PIE_MY/6.0f)+gl_FragCoord.x;
			float y=temp_r*cos(interval*j+float(i)*PIE_MY/6.0f)+gl_FragCoord.y;
			vec2 temp_where=vec2(x,y);
			float g_all=gd(temp_where,i)*glight(temp_where);
			temp_result+=(1-Visiable(temp_where))*g_all;
			//temp_result+=1-Visiable(temp_where);
		}
		temp_result/=sh;
		LineAO_result+=temp_result;
	}
	return LineAO_result;
}

void main()
{
	float NdotL=dot(normalize(N),normalize(lightpos));
	NdotL=clamp(NdotL,0,1);
	vec3 H=normalize(normalize(lightpos)+normalize(view));
	float spec=dot(H,normalize(N));
	spec=clamp(spec,0,1);
	switch(shader_type)
	{
	case 0:
	col=mycolor + lightcolor*_ambient;
	break;
	case 1:
	vec4 localLight1=pow(spec,16)*lightcolor*_specular+NdotL*lightcolor*_diffuse + lightcolor*_ambient;
	col=mycolor+localLight1;
	break;
	case 2:
	float LineAO1 = count_LineAO();
	LineAO1=clamp(LineAO1,0,1);
	float depth_float=texture2D(Depth,tran(gl_FragCoord.xy)).r;
	//depth_float=LinearizeDepth(tran(gl_FragCoord.xy));
	//depth_float=LinearizeDepth(gl_FragCoord.z);
	col=vec4(LineAO1,LineAO1,LineAO1,1.0f);
	col=mycolor+(1-LineAO1)*lightcolor*_ambient;
	break;
	case 3:
	float LineAO2 = count_LineAO();
	LineAO2=clamp(LineAO2,0,1);
	vec4 localLight2=pow(spec,2)*lightcolor*_specular+NdotL*lightcolor*_diffuse;
	vec4 allLight=lightcolor*_ambient;
	localLight2+=allLight;
	col=mycolor+(localLight2);
	col*=(1-LineAO2);
	break;
	default:
	break;
	}
}


