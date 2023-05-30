#include "stdafx.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT msg, WPARAM w_param, LPARAM l_param);

LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
		return 0L;

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {
	// init crosshair
	_crosshair.Init();

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProcedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"Crosshair Overlay Class";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
		wc.lpszClassName,
		L"Crosshair Overlay",
		WS_POPUP,
		0,
		0,
		_crosshair.GetScreenSize().x,
		_crosshair.GetScreenSize().y,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);
	
	SetLayeredWindowAttributes(window, RGB(0,0,0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom

		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.OutputWindow = window;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	// create device
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));
	
	if (back_buffer) {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	} else return 1;

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();

	ImGuiIO& _io = ImGui::GetIO(); (void)_io;
	_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	// set window for crosshair
	_crosshair.SetWindow(window);

	bool running = true;
	while (running) {
		MSG msg;

		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				running = false;
			}
		}

		if (!running)
			break;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		_crosshair.Render();

		// render
		ImGui::Render();

		constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(1, 0);

	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	if (swap_chain)
		swap_chain->Release();

	if (device_context)
		device_context->Release();

	if (device)
		device->Release();

	if (render_target_view)
		render_target_view->Release();

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

