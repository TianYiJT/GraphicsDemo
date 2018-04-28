#version 420 core

uniform sampler3D DiffuseLookUpTex;
uniform sampler3D SpecularLookUpTex;
uniform sampler2D TexLookUpTex;
uniform float cd;
uniform float Intensity;
uniform float ln;
uniform float rMax;
uniform float glossness;
uniform float frensel;

uniform mat4 light1;
uniform mat4 light2;
uniform mat4 light3;
uniform mat4 light4;
uniform mat4 light5;
uniform mat4 light6;
uniform mat4 light7;

uniform int lightType1;
uniform int lightType2;
uniform int lightType3;
uniform int lightType4;
uniform int lightType5;
uniform int lightType6;
uniform int lightType7;

uniform int lightnumber;

in vec3 Nor;
in float Roughness;
in vec3 View;
in vec3 position;
in vec3 color_;
layout(location = 0) out vec4 color;
float _Fresnel = 0.5f;
float _Rough = 0.7f;
#define E 2.71828183f 
vec3 interactByLineAndPlane(vec3 pos1,vec3 dir1,vec3 pos2,vec3 dir2)
{
	float d = dot(pos2-pos1,normalize(dir2))/dot(normalize(dir1),normalize(dir2));
	return normalize(dir1)*d + pos1;
}
vec2 PointTran(vec3 pos,vec3 center,vec3 nor)
{
	vec3 a1 = vec3(nor.z,nor.z,-(nor.x+nor.y));
	vec3 a2 = cross(nor,a1);
	vec3 v = pos-center;
	vec2 tex;
	tex.x = distance(v,vec3(0,0,0))*dot(normalize(v),normalize(a1));
	tex.y = distance(v,vec3(0,0,0))*dot(normalize(v),normalize(a2));
	return tex;
}
vec3 PointTranToWorld(vec2 tex,vec3 center,vec3 nor)
{
	vec3 a1 = vec3(nor.z,nor.z,-(nor.x+nor.y));
	vec3 a2 = cross(nor,a1);
	vec3 v = normalize(a1) * tex.x + normalize(a2)*tex.y;
	vec3 pos = v+center;
	return pos;
}
float countArea(float range,int type)
{
	if(type==0)
		return range*range;
	else
		return range*range*3.1415926;
}
float BRDF_D(vec3 h,float g)
{
	float NdotH = dot(normalize(h),normalize(Nor));
	float alpha2 = g*g;
	float NdotH2 = NdotH*NdotH;
	return alpha2/(3.1415926*pow((NdotH*NdotH*(alpha2-1)+1),2));
	//return NdotH2;
}
float F(in vec3 V,in vec3 H)
{
	float _f = _Fresnel + (1-_Fresnel)* pow(1-dot(V,H),4.0f);
	return _f;
}
float G( vec3 N, vec3 H, vec3 L, vec3 V)
{
	float NdotH = dot(N,H);
	float NdotL = dot(N,L);
	float VdotH = dot(V,H);
	float NdotV = dot(N,V);
	
	float G1 = 2*NdotH*NdotL/(VdotH);
	float G2 = 2*NdotH*NdotV/(VdotH);

	if(G1<=G2)
	{
		if(G1<=1)
		{
			return G1;
		}
		else
		{
			return 1.0f;
		}	
	}
	else
	{
		if(G2<=1)
		{
			return G2;
		}
		else
		{
			return 1.0f;
		}
	}

}
float BRDF_F(vec3 w,vec3 h)
{
	
	float WDotH = clamp(dot(normalize(w),normalize(h)),0.0,1.0);
	float f1 = 0.555473;
	float f2 = 0.698316;
	float f3 = (-f1*WDotH-f2)*WDotH;
	return frensel+(1-frensel)*pow(2,f3);
	
//	return 1.0;
}
float D(in vec3 N,in vec3 H)
{
	float NdotH = dot(N,H);
	float M2 = pow(_Rough,2.0f);
	float fir = 1.0f/(M2*pow(NdotH,4.0f));
	float secFir1 = pow(NdotH,2.0f)-1.0f;
	float secFir2 = pow(_Rough,2.0f);
	float secFir3 = pow(NdotH,2.0f);
	float secFir =secFir1 /(secFir2*secFir3);
	float sec = pow(E,secFir);
	return fir*sec;
}
float BRDF_G1(vec3 v)
{
	float k = (Roughness+1)*(Roughness+1)/8;
	float VDotN = dot(normalize(v),normalize(Nor));
	VDotN = (VDotN+1)/2;
	return VDotN/(VDotN*(1-k)+k);
}
float BRDF_G(vec3 w1,vec3 w2,vec3 h)
{
	return clamp(1/(BRDF_G1(w1)*BRDF_G1(w2)),0.0,1.0);
}
float countAlpha()
{
	float a = 2.0f*pow(2.0f/(glossness+2.0f),0.5f);
	return a;
}
float F_BRDF(float g,float r)
{
	return g*r/rMax;
}
float countAS(vec3 pos1,vec3 nor1,vec3 pos2,vec3 nor2,float range,int type)
{
	float Area = countAlpha()*pow(distance(pos1,pos2),2);
	float r1;
	vec3 reflect1 = reflect(View,nor1);
	vec3 center1;
	center1= interactByLineAndPlane(pos1,reflect1,pos2,nor2);
		
	if(type==1)
	{
		r1 = pow(Area/3.1415926,0.5);
		float disAll = distance(center1,pos2);
		float disa = ((pow(r1,2)-pow(range,2))/disAll+disAll)/2;
		float disc = pow(pow(r1,2)-pow(disa,2),0.5f);
		float dish = 2*(r1+range)-disAll;
		return Area/60.0f;
	}
	else
	{
		r1 = pow(Area,0.5f);
		vec2 pos1 = PointTran(center1,pos2,nor2);
		vec2 pos2 = PointTran(pos2,pos2,nor2);
		vec2 tl = max(pos1-vec2(r1/2,r1/2),pos2-vec2(range/2,range/2));
		vec2 br = min(pos1+vec2(r1/2,r1/2),pos2+vec2(range/2,range/2));
		return Area/60.0f;
	}
}
vec3 countSC(vec3 pos1,vec3 nor1,vec3 pos2,vec3 nor2,float range,int type)
{
	float Area = countAlpha()*pow(distance(pos1,pos2),2);
	float r1;
	vec3 reflect1 = reflect(View,nor1);
	vec3 center1;
	center1= interactByLineAndPlane(pos1,reflect1,pos2,nor2);

	if(type==1)
	{
		r1 = pow(Area/3.1415926,0.5);
		if(dot(normalize(reflect1),normalize(nor2))>=0)
			return pos1+reflect1;
		float disAll = distance(center1,pos2);
		float disa = ((pow(r1,2)-pow(range,2))/disAll+disAll)/2;
		return pos1+reflect1;
		return center1+(pos2-center1)*(disa/disAll);
	}
	else
	{
		if(dot(normalize(reflect1),normalize(nor2))>=0)
			return pos1+reflect1;
		r1 = pow(Area,0.5f);
		vec2 pos1_ = PointTran(center1,pos2,nor2);
		vec2 pos2_ = PointTran(pos2,pos2,nor2);
		vec2 tl = max(pos1_-vec2(r1/2,r1/2),pos2_-vec2(range/2,range/2));
		vec2 br = min(pos1_+vec2 (r1/2,r1/2),pos2_+vec2(range/2,range/2));
		return PointTranToWorld((tl+br)/2,pos2,nor2);
	}
	
}

vec4 countKD(mat4 lightPara,int type)
{
	vec4 pos= lightPara[0];
	vec4 nor= lightPara[1];
	vec4 col= lightPara[2];
	float range= lightPara[3][0];
	float dis = distance(position,pos.xyz);
	vec3 p1,p2;
	p1 = interactByLineAndPlane(position,Nor,pos.xyz,nor.xyz);
	p2 = interactByLineAndPlane(position,-nor.xyz,pos.xyz,nor.xyz);
	vec3 pd = (p1+p2)/2;
	float cospd;
	cospd = dot(normalize(normalize(Nor)+normalize(nor.xyz)),normalize(Nor));
	cospd=(cospd+1)/2;
	float cos0 = clamp(dot(normalize(Nor),normalize(nor.xyz)),0.0f,1.0f);
	cos0 = (cos0+1)/2;
	vec2 texcoord = PointTran(pd,pos.xyz,nor.xyz);
	float diffLookup = texture3D(DiffuseLookUpTex,vec3(texcoord,dis/rMax)).r;
	float Area = countArea(range,type);
	vec4 result = col * Intensity * cd * cospd * Area * cos0 * diffLookup/(dis*dis);
	//return result;
	return col * Intensity * cd * cos0 *Area*cospd/(dis*dis) ;
	//return vec4(normalize(pd-position),1.0f);

	//return vec4(normalize(Nor),1.0f);
	//return vec4((normalize(pd-position)+vec3(1.0,1.0,1.0))/2,1.0f);
}
vec4 countKS(mat4 lightPara,int type)
{
	vec4 pos= lightPara[0];
	vec4 nor= lightPara[1];
	vec4 col = lightPara[2];
	float range = lightPara[3][0];
	float dis = distance(position,pos.xyz);
	vec3 psc = countSC(position,Nor,pos.xyz,nor.xyz,range,type);
	vec3 vpsc = psc - position;
	vec3 H = (normalize(vpsc)+normalize(View))/2;
	float cos0 = dot(normalize(Nor),normalize(nor.xyz));
	if(cos0>-1)
		cos0 = (cos0+1)/2;
	else
		cos0 = 0.8;
	float Area = countAS(position,Nor,pos.xyz,nor.xyz,range,type);
	float g = F_BRDF(Roughness,dis);
	vec2 texcoord = PointTran(psc,pos.xyz,nor.xyz);
	float SpecularLookUp = texture3D(SpecularLookUpTex,vec3(texcoord,g)).r;
	vec4 result = col * BRDF_D(H,Roughness)*BRDF_F(View,H)*BRDF_G(View,vpsc,H)*ln*SpecularLookUp/(4*cos0*Area);
	//return result;
	return col * BRDF_D(H,Roughness)*BRDF_F(View,H)*BRDF_G(View,vpsc,H)*ln/(4*Area*cos0);
	
	//return vec4(psc/2,1.0f);
//	return vec4(psc+vec3(100,100,100),1.0f);
}

void main(void)
{
	
	vec4 col1 = countKD(light1,lightType1)+countKS(light1,lightType1);
	vec4 col2 = countKD(light2,lightType2)+countKS(light2,lightType2);
	vec4 col3 = countKD(light3,lightType3)+countKS(light3,lightType3);
	vec4 col4 = countKD(light4,lightType4)+countKS(light4,lightType4);
	vec4 col5 = countKD(light5,lightType5)+countKS(light5,lightType5);
	vec4 col6 = countKD(light6,lightType6)+countKS(light6,lightType6);
	vec4 col7 = countKD(light7,lightType7)+countKS(light7,lightType7);
	switch(lightnumber)
	{
	case 7:
		color = (col1+col2+col3+col4+col5+col6+col7)/7;
		break;
	case 6:
		color = (col1+col2+col3+col4+col5+col6)/6;
		break;
	case 5:
		color = (col1+col2+col3+col4+col5)/5;
		break;
	case 4:
		color = (col1+col2+col3+col4)/4;
		break;
	case 3:
		color = (col1+col2+col3)/3;
		break;
	case 2:
		color = (col1+col2)/2;
		break;
	case 1:
		color = col1;
		break;
	}
	color.r = clamp(color.r,0.0,1.0);
	color.g = clamp(color.g,0.0,1.0);
	color.b = clamp(color.b,0.0,1.0);
	color.a = clamp(color.a,0.0,1.0);
	color = vec4(color.r+color_.r,color.g+color_.g,color.b+color_.b,1.0f);	
//	color = countKD(light2,lightType2);
	//color = vec4(1.0f,0.0f,0.0f,1.0f);
}
