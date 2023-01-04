#include <windows.h>
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_dx9.h>
#include <backends/imgui_impl_win32.h>

#include <dwmapi.h>

namespace d3d_data
{
	IDirect3D9* d3d9 = NULL;
	IDirect3DDevice9* d3d_device = NULL;
}

namespace overlay_data
{
	HWND overlay_hwnd = NULL;
}

static int width = GetSystemMetrics(SM_CXSCREEN);
static int height = GetSystemMetrics(SM_CYSCREEN);

namespace menu_data
{
	static bool show_menu = true;

	static bool enable_drawing = false;
	static int draw_mode = 0;
	static float time_shown = 1.25f;
}

std::vector<ImVec2> mouse_entries;

void Draw_Personal_Project_Menu()
{
	ImGui::SetNextWindowSize(ImVec2(600.000f, 400.000f), ImGuiCond_Once);
	ImGui::Begin("Personal Project Menu", NULL, 2);

	ImVec2 P1, P2;
	ImDrawList* pDrawList;
	const auto& CurrentWindowPos = ImGui::GetWindowPos();
	const auto& pWindowDrawList = ImGui::GetWindowDrawList();
	const auto& pBackgroundDrawList = ImGui::GetBackgroundDrawList();
	const auto& pForegroundDrawList = ImGui::GetForegroundDrawList();

	P1 = ImVec2(0.000f, 125.000f);
	P1.x += CurrentWindowPos.x;
	P1.y += CurrentWindowPos.y;
	P2 = ImVec2(600.000f, 125.000f);
	P2.x += CurrentWindowPos.x;
	P2.y += CurrentWindowPos.y;
	pDrawList = pWindowDrawList;
	pDrawList->AddLine(P1, P2, ImColor(1.000f, 0.000f, 0.000f, 1.000f), 2.000f);

	ImGui::SetCursorPos(ImVec2(8.000f, 30.000f));
	ImGui::Checkbox("Enable Drawing", &menu_data::enable_drawing);

	ImGui::SetNextItemWidth(150.000f);
	ImGui::SetCursorPos(ImVec2(9.000f, 90.000f));
	ImGui::Combo("Draw Mode", &menu_data::draw_mode, "Constant\0Point\0");

	ImGui::SetNextItemWidth(150.000f);
	ImGui::SetCursorPos(ImVec2(8.000f, 60.000f));
	ImGui::SliderFloat("Time Shown", &menu_data::time_shown, 0.f, 10.f);

	ImGui::SetCursorPos(ImVec2(8.000f, 140.000f));
	ImGui::Text("Made by Harvey");

	ImGui::SameLine();
	ImGui::Text("--> School Project");

	ImGui::SetCursorPos(ImVec2(275.000f, 30.000f));
	if (ImGui::Button("Clear Drawings", ImVec2(0.000f, 0.000f)))
	{
		mouse_entries.clear();
	}

	ImGui::End();
}

// this will render d3d to the screen including anything we may draw
bool RenderFunc()
{
	// if the device does not exist, create it
	if (!d3d_data::d3d_device)
	{
		// create the d3d object
		d3d_data::d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		if (!d3d_data::d3d9)
			return false;

		// initialise params (see msdn)
		D3DPRESENT_PARAMETERS present_parameters;

		present_parameters.Flags = 0;
		present_parameters.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
		present_parameters.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
		present_parameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		present_parameters.Windowed = true;
 		present_parameters.BackBufferCount = 1;
 		present_parameters.MultiSampleType = D3DMULTISAMPLE_NONE; // 0
 		present_parameters.MultiSampleQuality = D3DMULTISAMPLE_NONE; // 0
		present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		present_parameters.EnableAutoDepthStencil = true;
		present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
		present_parameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; // 0
		present_parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		present_parameters.hDeviceWindow = overlay_data::overlay_hwnd;

		// create d3d device
		d3d_data::d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, overlay_data::overlay_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &present_parameters, &d3d_data::d3d_device);

		// initialise ImGui
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(overlay_data::overlay_hwnd);
		ImGui_ImplDX9_Init(d3d_data::d3d_device);

		// prevents any cursor adjustments
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

		// initialise with our created overlay (canvas)
		io.ImeWindowHandle = overlay_data::overlay_hwnd;
	}

	// clears the surface before rendering next set of info as well as the depth buffer
	d3d_data::d3d_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// begin drawing whatever comes after this line
	d3d_data::d3d_device->BeginScene();

	// initialise ImGui frame
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX9_NewFrame();
	ImGui::NewFrame();

	if (GetAsyncKeyState(VK_INSERT) & 1)
		menu_data::show_menu = !menu_data::show_menu;

	// draw menu
	if (menu_data::show_menu)
		Draw_Personal_Project_Menu();

	// draw within here

	// store mouse position
	mouse_entries.emplace_back(ImGui::GetIO().MousePos);

	if (menu_data::draw_mode == 0) // constant drawing mode
	{
		// loop all mouse positions stored
		for (int i = 0; i < mouse_entries.size() - 1; i++)
		{
			// draw line from mouse position stored at i (current) to mouse position stored at i + 1 (next)
			ImGui::GetBackgroundDrawList()->AddLine(mouse_entries[i], mouse_entries[i + 1], ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 255, 255)));
		}
	}

	// drawing is over

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	// end the scene, we dont want to draw anything more
	d3d_data::d3d_device->EndScene();
		
	// we present the scene, 
	d3d_data::d3d_device->Present(0, 0, 0, 0);

	return true;

}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// handles keyboard / mouse input for the menu so clicks get registered
	if (d3d_data::d3d9)
	{
		ImGuiIO& io = ImGui::GetIO();

		POINT mouse_position;
		GetCursorPos(&mouse_position);
		ScreenToClient(overlay_data::overlay_hwnd, &mouse_position);
		ImGui::GetIO().MousePos.x = mouse_position.x;
		ImGui::GetIO().MousePos.y = mouse_position.y;

		// separate ImGui WndProcHandler
		ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

		// sets the different mouse buttons as either inactive or active depending if they have been clicked
		io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;
		io.MouseDown[2] = GetAsyncKeyState(VK_MBUTTON) & 0x8000;
		io.MouseDown[3] = GetAsyncKeyState(VK_XBUTTON1) & 0x8000;
		io.MouseDown[4] = GetAsyncKeyState(VK_XBUTTON2) & 0x8000;

		for (int i = 6; i < 512; i++) 
		{
			io.KeysDown[i] = (GetKeyState(i) & 0x8000) != 0;
		}
	}

	switch (Msg)
	{
	case WM_PAINT:
		// makes it so clicks go on to the menu and not through the menu
		if (menu_data::show_menu)
		{
			SetWindowLongA(overlay_data::overlay_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
			SetLayeredWindowAttributes(overlay_data::overlay_hwnd, RGB(255, 255, 255), 0, LWA_COLORKEY);
		}
		else
		{
			SetWindowLongA(overlay_data::overlay_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT);
			SetLayeredWindowAttributes(overlay_data::overlay_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
		}

		// call render function
		RenderFunc();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: 
		return DefWindowProcA(hWnd, Msg, wParam, lParam);
	}

	return 0;
}

HWND CreateOverlay(HINSTANCE hInstance)
{
	// this is similar to a real life canvas where one can draw on

	// we start by initialising the class
	WNDCLASSEXA window_class;
	window_class.cbSize = sizeof(WNDCLASSEXA);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = OverlayWndProc;
	window_class.cbClsExtra = NULL;
	window_class.cbWndExtra = NULL;
	window_class.hInstance = hInstance;
	window_class.hIcon = NULL;
	window_class.hCursor = LoadCursor(nullptr, IDC_ARROW); // default cursor
	window_class.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); // black background (transparent anyway)
	window_class.lpszMenuName = "Overlay";
	window_class.lpszClassName = "Overlay";
	window_class.hIconSm = NULL;

	// register the class
	RegisterClassExA(&window_class);

	// get the monitor width and height
	auto screen_x = GetSystemMetrics(SM_CXSCREEN);
	auto screen_y = GetSystemMetrics(SM_CYSCREEN);

	// create the window with parameters (guided by msdn docs) --> obviously we want WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT because we want it to be click through, invisible and layered on top of other applications
	auto overlay_instance = CreateWindowExA(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT, "Overlay", "Overlay", WS_POPUP, NULL, NULL, screen_x, screen_y, NULL, NULL, hInstance, NULL);

	// basic error checks
	if (!overlay_instance)
		std::cout << "LastError: " << GetLastError() << "\n";

	return overlay_instance;
}

int main()
{
	overlay_data::overlay_hwnd = CreateOverlay((HINSTANCE)GetModuleHandle(0));

	// fail check
	if (!overlay_data::overlay_hwnd)
	{
		std::cout << "Failed To Create Overlay\n";
		return 0;
	}
	
	// initialises the layered window attributes
	SetLayeredWindowAttributes(overlay_data::overlay_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	// show the window obviously because we dont want it in the background for whatever reason
	ShowWindow(overlay_data::overlay_hwnd, SW_SHOW);

	MSG msg;
	while (GetMessageA(&msg, 0, 0, 0))
	{
		// calls into WndProc
		TranslateMessage(&msg);
		DispatchMessage(&msg); 

		// we could update some data here like overlay (canvas) size or something
	}

	return 0;
}