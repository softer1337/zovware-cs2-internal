#include "includes.h"
#include "kiero/minhook/include/MinHook.h"
#include "core/hooks/hooks.h"
#include "menu/menu.h"
#include "sdk/interface.h"
#include "state.hpp"
#include <stdio.h>
#include "core/hooks/mouseinp/mouseinp.h"
#include "core/mem.hpp"
#include "core/hooks/chat/chat.h"
#include "sdk/itemschema.hpp"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

static HMODULE GetThisDllHandle()
{
	MEMORY_BASIC_INFORMATION info;
	VirtualQuery(GetThisDllHandle, &info, sizeof(info));
	return (HMODULE)info.AllocationBase;
}

HMODULE g_module = GetThisDllHandle();
HANDLE g_thread = nullptr;

DWORD WINAPI UnloadThread(LPVOID hMod)
{
	Sleep(100);

	g_state.running = false;
	g_state.unloading = true;

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (oWndProc)
		SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);

	if (mainRenderTargetView)
		mainRenderTargetView->Release();

	if (pContext)
		pContext->Release();

	if (pDevice)
		pDevice->Release();

	FreeConsole();

	FreeLibraryAndExitThread((HMODULE)hMod, 0);
}

void StopCheat()
{
	if (g_state.unloading.exchange(true))
		return;

	g_state.running = false;

	CreateThread(nullptr, 0, UnloadThread, g_thread, 0, nullptr);
}

void CheckStopKey()
{
	if (GetAsyncKeyState(VK_END) & 1)
		StopCheat();
}


void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!g_state.running || g_state.unloading)
		return CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);

	static ImVec2 m_vecMousePosSave{};
	if (uMsg == WM_QUIT || uMsg == WM_CLOSE || uMsg == WM_DESTROY)
	{
		return true;
	}
 
	if (1)
	{
		if (uMsg == WM_KEYUP && wParam == VK_INSERT) {
			MENU::menu_open = !MENU::menu_open;
			ImGui::GetIO().MouseDrawCursor = MENU::menu_open;
			ShowCursor(!MENU::menu_open);

			IsRelativeMouseMode_o(Interface::GetInputSystem(), MENU::menu_open ? false : MENU::mainActive);

			if (MENU::menu_open)
			{
				if (m_vecMousePosSave.x == 0.f && m_vecMousePosSave.y == 0.f)
					m_vecMousePosSave = ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f);

				ImGui::GetIO().MousePos = m_vecMousePosSave;

				if (MEM::IsInGame())
				{
					static auto SDL_WarpMouseInWindow = (void(*)(void*, float, float))GetProcAddress(GetModuleHandleA("SDL3.dll"), "SDL_WarpMouseInWindow");

					if (SDL_WarpMouseInWindow)
						SDL_WarpMouseInWindow(nullptr, ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
				}
			}
			else
			{
				m_vecMousePosSave = ImGui::GetIO().MousePos;
			}
		}

		if (MENU::menu_open && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) == 0)
			return true;
	}

	return CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);
}


bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!g_state.running || g_state.unloading)
		return oPresent(pSwapChain, SyncInterval, Flags);

	CheckStopKey();
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}


	MENU::initStyle();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	
	ImGui::NewFrame();
	MENU::render();
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

void CreateConsole() {
	if (AllocConsole()) {
		// Перенаправляем stdout, stderr, stdin в новую консоль
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

		// Очищаем консоль
		system("cls");

		// Устанавливаем заголовок окна
		SetConsoleTitleA("ZZZZZZ CBOOOO");
	}
}


DWORD WINAPI MainThread(LPVOID lpReserved)
{
	CreateConsole();
	Sleep(5000);
	Interface::Init();
	InitHooks();
	initAudioSystem();
	ChatMessage msg;
	msg << ImColor(1.f, 0.f, 1.f) << "[binware] " << ImColor(1.f, 1.f, 1.f) << "Init!";
	msg.send();
	//PlayHitSound(0.3f, BRAINROT);

	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);

	ItemSchema::get()->initialize();

	g_state.initialized = true;
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		g_module = hMod;
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}