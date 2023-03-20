#pragma once
#include "../util/util.h"

extern IDirect3D9Ex* p_Object;
extern IDirect3DDevice9Ex* p_Device;
extern D3DPRESENT_PARAMETERS p_Params;

inline bool bStreamProof = true;

class Render {
public:
	static HRESULT DirectXInit();
	static void CreateOverlay();
	static WPARAM MainLoop();
	static void render();
	static void CleanuoD3D();
	static void Menu();
};