#pragma once
#include"LoadShaders.h"
#include"vmath.h"
#include"SceneManager.h"
#define DEPTHNORMALSTEX_FRAG_SHADER "DepthNormalsTex.frag"
#define DEPTHNORMALSTEX_VERT_SHADER "DepthNormalsTex.vert"
#define PBAL_FRAG_SHADER "PBAL.frag"
#define PBAL_VERT_SHADER "PBAL.vert"
#define SSR_FRAG_SHADER "SSR.frag"
#define SSR_VERT_SHADER "SSR.vert"

#define CULL_FRAG_SHADER "Cull.frag"
#define CULL_VERT_SHADER "Cull.vert"
#define POS_VERT_SHADER "pos.vert"
#define POS_FRAG_SHADER "pos.frag"
#define BUFFER_OFFSET(offset) ((GLvoid*) (NULL + offset))