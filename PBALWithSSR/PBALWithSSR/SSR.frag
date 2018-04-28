
#version 420 core

uniform sampler2D MainTex;
uniform sampler2D depthNormalTex;
uniform sampler2D posTex;
uniform sampler2D DepthTex;

uniform mat4 View;
uniform mat4 invView;
uniform mat4 Proj;
uniform mat4 ViewPortProj;
uniform mat4 invViewPortProj;

in vec2 uv;

out vec4 fragment_color;
vec4 debugColor;

const int maxIter = 10;
const int numBinarySearchSteps = 10;
const int pasDiscret = 5;

float getViewDepth(vec3 viewCoord){
    vec4 projectedCoord = ViewPortProj * vec4(viewCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    return texelFetch(posTex, ivec2(projectedCoord.xy), 0).z;
}

float getDepth(vec3 viewCoord){
    vec4 projectedCoord = ViewPortProj * vec4(viewCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;

    vec4 tmp = invViewPortProj * vec4(projectedCoord.xy, texelFetch(DepthTex, ivec2(projectedCoord.xy), 0).r, 1);

    return tmp.z/tmp.w;
}

vec3 projDroite(vec3 orig3D, vec3 end3D, out vec3 end2D){
    vec4 projectedCoord = ViewPortProj * vec4(orig3D, 1.0);
    vec3 ret = vec3(projectedCoord.xy / projectedCoord.w, orig3D.z);

    projectedCoord = ViewPortProj * vec4(end3D, 1.0);
    end2D = vec3(projectedCoord.xy / projectedCoord.w, end3D.z);

    return ret;
}

int checkSortie(vec3 viewCoord){
    ivec2 texSize = textureSize(MainTex, 0);
    vec4 projectedCoord = ViewPortProj * vec4(viewCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;

    if(projectedCoord.x > texSize.x || projectedCoord.x < 0.0 || projectedCoord.y > texSize.y || projectedCoord.y < 0.0)
        return 0;
    else
        return 1;
}

int checkSortieDiscret(vec3 imgCoord){
    ivec2 texSize = textureSize(MainTex, 0);

    if(imgCoord.x > texSize.x || imgCoord.x < 0.0 || imgCoord.y > texSize.y || imgCoord.y < 0.0)
        return 0;
    else
        return 1;
}

int BinarySearch(vec3 dir, inout vec3 cur){
    float curDepth;
    for(int i = 0; i < numBinarySearchSteps; i++){
        curDepth = getViewDepth(cur);
        if(checkSortie(cur) == 0)
            return 0;
        if(cur.z > curDepth){
            return 1;
        }
        else
            cur -= dir * 1/numBinarySearchSteps;
    }
    return 1;
}

int BinarySearchDiscret(vec3 dir, inout vec3 cur){
	float curDepth;
	dir /= pasDiscret;
	for(int i = 0; i < pasDiscret; i++){

		curDepth = texelFetch(posTex, ivec2(cur.xy), 0).z;
		if(checkSortieDiscret(cur) == 0)
		    return 0;
		if(cur.z >= curDepth)
			return 1;
		else
			cur -= dir;
	}
	return 1;
}

int RayMarch(vec3 dir, inout vec3 cur){

    dir *= 0.1;
  
    float baseDepth = getDepth(cur);
	//debugColor = vec4(-baseDepth,-baseDepth,-baseDepth,1.0);
    for(int pas = 0; pas < maxIter; pas++){

        cur += dir;
        float curDepth = getViewDepth(cur);
//        float curDepth = getDepth(cur);

        if(cur.z < curDepth){
//        if(baseDepth > curDepth){
            int test = BinarySearch(dir, cur);
//            int test = BinarySearch2(dir, cur, curDepth);
            if(test == 1)
                return pas;
            else
                return maxIter;
        }
    }
    return maxIter;
}

vec3 rayMarchDiscret(vec3 orig, vec3 end){

    vec3 dir = end-orig;
    float n;
    if(abs(dir.x) > abs(dir.y)){
        n = dir.x;
        dir.xyz /= dir.x;
    }
    else{
        n = dir.y;
        dir.xyz /= dir.y;
    }

    n /= pasDiscret;
    dir *= pasDiscret;

//    fragment_color = vec4(abs(dir.xy), 0, 1);

    float baseDepth = texelFetch(DepthTex, ivec2(orig.xy), 0).r;

    for(int i = 0; i < n; i++){
        vec3 cur = orig + dir * i;
        vec3 cur2 = orig + dir * (i + 0.5);

        float curDepth = texelFetch(posTex, ivec2(cur.xy), 0).z;
        float curDepth2 = texelFetch(posTex, ivec2(cur2.xy), 0).z;

        if(cur.z >= 0 || checkSortieDiscret(cur) == 0)
            return vec3(0);

        if(cur.z < curDepth){
			if(BinarySearchDiscret(dir, cur) == 0)
			    return vec3(0);
            return cur;
        }
        else if(cur2.z < curDepth2){
			if(BinarySearchDiscret(dir, cur2) == 0)
			    return vec3(0);
            return cur2;
        }
    }

    return vec3(0);
}

void main(){

    vec4 baseColor = texelFetch(MainTex, ivec2(gl_FragCoord.xy), 0);
    float depth = texelFetch(DepthTex,  ivec2(gl_FragCoord.xy), 0).r;
    vec3 normal = texelFetch(depthNormalTex, ivec2(gl_FragCoord.xy), 0).xyz;
    vec3 orig = texelFetch(posTex, ivec2(gl_FragCoord.xy), 0).xyz;

    vec3 dir = normalize(reflect(normalize(orig), normalize(normal)));
    vec3 reflectCoord = orig;
    int pas = RayMarch(dir * max(0.1, -orig.z), reflectCoord);
    if(depth < 1 && pas != maxIter)
	{
        vec4 projectedCoor = ViewPortProj * vec4(reflectCoord, 1.0);
        vec2 coor = projectedCoor.xy / projectedCoor.w;
        vec3 normalPoint = texelFetch(depthNormalTex, ivec2(coor), 0).xyz;
        float cos_theta = max(0, dot(normalize(normalPoint), normalize(orig - reflectCoord)));
        fragment_color = baseColor + (vec4(texelFetch(MainTex, ivec2(coor), 0).rgb, 0) * cos_theta) * 0.3;
    }
    else
        fragment_color = baseColor;
		 fragment_color = baseColor;
	//fragment_color = vec4(depth,0.0,0.0,1.0);
	//fragment_color = vec4(pas/100.0,pas/100.0,pas/100.0,1.0);
	//fragment_color = vec4(getDepth);;
}
