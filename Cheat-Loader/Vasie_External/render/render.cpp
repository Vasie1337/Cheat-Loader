#include "render.h"
#include <Windows.h>
#include <iostream>
#include "../cheat/esp/esp.h"
#include "Font/font.h"
#include "../antidebug/strings/xorstr.hpp"

IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

HWND GameHwnd = NULL;
RECT GameRect = { NULL };
HWND MyHwnd = NULL;
MSG Message = { NULL };

uintptr_t procid = NULL;

HRESULT Render::DirectXInit()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = MyHwnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, MyHwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(MyHwnd);
	ImGui_ImplDX9_Init(p_Device);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);

	colors[ImGuiCol_Border] = ImColor(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImColor(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	
	colors[ImGuiCol_CheckMark] = ImColor(51, 214, 255, 255);
	colors[ImGuiCol_SliderGrab] = ImColor(51, 214, 255, 255);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Separator] = ImColor(51, 214, 255, 255);
	colors[ImGuiCol_SeparatorHovered] = ImColor(51, 214, 255, 255);
	colors[ImGuiCol_SeparatorActive] = ImColor(51, 214, 255, 255);

	colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TabActive] = ImColor(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowPadding = ImVec2(8.00f, 8.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);

	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.FramePadding = ImVec2(5.00f, 2.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.IndentSpacing = 25;
	
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;

	style.WindowBorderSize = 2;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;

	style.WindowRounding = 7;
	style.ChildRounding = 7;
	style.FrameRounding = 7;
	style.PopupRounding = 7;
	style.ScrollbarRounding = 7;
	style.GrabRounding = 7;
	style.TabRounding = 7;

	style.WindowTitleAlign = { 0.5f, 0.5f };

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromMemoryTTF(FontRawBytes, 18.0f, 18.0f);
	io.IniFilename = nullptr;

	p_Object->Release();
	return S_OK;
}

void Render::CreateOverlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		_s("Hack Class"),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	RECT Rect;
	GetWindowRect(GetDesktopWindow(), &Rect);

	RegisterClassExA(&wcex);

	MyHwnd = CreateWindowExA(NULL, _s("Hack Class"), _s("Hack Window"), WS_POPUP, Rect.left, Rect.top, Rect.right, Rect.bottom, NULL, NULL, wcex.hInstance, NULL);

	SetWindowLong(MyHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(MyHwnd, &margin);
	ShowWindow(MyHwnd, SW_SHOW);
	SetWindowPos(MyHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetLayeredWindowAttributes(MyHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	UpdateWindow(MyHwnd);
}

void Render::CleanuoD3D()
{
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}

void Render::render() {

	Esp::ActorLoop();

	{
		Vector2 ScreenCentre = { (double)Width / 2 , (double)Height / 2 };

		if (bCrossHair)
		{
			Util::DrawLine(ImVec2(ScreenCentre.x + 10.f, ScreenCentre.y), ImVec2(ScreenCentre.x - 10.f, ScreenCentre.y), ImColor(51, 214, 255, 160), 1.8f);
			Util::DrawLine(ImVec2(ScreenCentre.x, ScreenCentre.y + 10.f), ImVec2(ScreenCentre.x, ScreenCentre.y - 10.f), ImColor(51, 214, 255, 160), 1.8f);
		}

	}

}

WPARAM Render::MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyHwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == GameHwnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyHwnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameHwnd, &rc);
		ClientToScreen(GameHwnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameHwnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyHwnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		render();
		Menu();

		ImGui::EndFrame();
		p_Device->SetRenderState(D3DRS_ZENABLE, false);
		p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

		if (p_Device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_Device->EndScene();
		}
		HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

		if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Device->Reset(&p_Params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanuoD3D();
	DestroyWindow(MyHwnd);

	return Message.wParam;
}

void Render::Menu() 
{
	static bool bMenu = true;
	static bool bInit = false;

	if (GetAsyncKeyState(VK_INSERT) & 1) 
		bMenu = !bMenu;

	if (!bMenu)
		return;

	if (!bInit) {
		ImGui::SetNextWindowSize({ 500.f,300.f });
		ImGui::SetNextWindowPos(ImVec2(Width / 4 - 250.f, Height / 2 - 150.f));
		bInit = true;
	}
	ImGui::Begin(_s("Vasie External"), 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::BeginTabBar(_s("TabBar"), 0))
	{
		if (ImGui::BeginTabItem(_s("Visuals")))
		{
			ImGui::Checkbox(_s("Corner Box"), &bCornerBox);
			ImGui::Checkbox(_s("Snaplines"), &bSnapLines);
			ImGui::Checkbox(_s("CrossHair"), &bCrossHair);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(_s("Misc")))
		{
			if (ImGui::Checkbox(_s("StreamProof"), &bStreamProof))
			{
				if (bStreamProof)
				{
					SetWindowDisplayAffinity(MyHwnd, WDA_EXCLUDEFROMCAPTURE);
				}
				else
				{
					SetWindowDisplayAffinity(MyHwnd, WDA_NONE);
				}
			}

			if (ImGui::Button(_s("Unload")))
				exit(0);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}


	ImGui::End();
}
