
// Include Base
#include "Includes.h"
#include "Utilities/Includes/IncludeImGui.h"

// Include Reversal Prevention
#include "Utilities/FNXor.h"

// Include Modules
#include "Modules/FNUtilities.h"
#include "Modules/FNInterface.h"
#include "Modules/FNDetector.h"
#include "Modules/FNReversal.h"

LPCSTR TargetProcess = xor("FortniteClient-Win64-Shipping.exe");
bool TargetProcessFocusedGlobal = FALSE;

namespace OverlayStructure {
	struct CurrentProcess {
		DWORD ID;
		HANDLE Handle;
		HWND Hwnd;
		WNDPROC WndProc;
		int WindowWidth;
		int WindowHeight;
		int WindowLeft;
		int WindowRight;
		int WindowTop;
		int WindowBottom;
		LPCSTR Title;
		LPCSTR ClassName;
		LPCSTR Path;
	};

	struct OverlayWindow {
		WNDCLASSEX WindowClass;
		HWND Hwnd;
		LPCSTR Name;
	};

	struct DirectX9Interface {
		IDirect3D9Ex * IDirect3D9 = NULL;
		IDirect3DDevice9Ex* pDevice = NULL;
		D3DPRESENT_PARAMETERS pParameters = { NULL };
		MARGINS Margin = { -1 };
		MSG Message = { NULL };
	};
}

// Overlay State
OverlayStructure::CurrentProcess Process;
OverlayStructure::OverlayWindow Overlay;
OverlayStructure::DirectX9Interface DirectX9;

// Keycode Names
const char* KeycodeIds[] = {
	"Unknown",
	"Left Mouse Button",
	"Right Mouse Button",
	"Cancel",
	"Middle Mouse Button",
	"Side Mouse Button 1",
	"Side Mouse Button 2",
	"Unknown",
	"Back",
	"Tab",
	"Unknown",
	"Unknown",
	"Clear",
	"Enter",
	"Unknown",
	"Unknown",
	"Shift",
	"Control",
	"Alt",
	"Pause",
	"Caps Lock",
	"Kana",
	"Unknown",
	"Junja",
	"Final",
	"Kanji",
	"Unknown",
	"Escape",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left Arrow",
	"Up Arrow",
	"Right Arrow",
	"Down Arrow",
	"Select",
	"Print Screen",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"Insert",
	"Delete",
	"Help",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Left Windows",
	"Right Windows",
	"Applications",
	"Unknown",
	"VK_SLEEP",
	"Numpad 0",
	"Numpad 1",
	"Numpad 2",
	"Numpad 3",
	"Numpad 4",
	"Numpad 5",
	"Numpad 6",
	"Numpad 7",
	"Numpad 8",
	"Numpad 9",
	"Multiply",
	"Add",
	"Separator",
	"Subtract",
	"Decimal",
	"Divide",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	"F21",
	"F22",
	"F23",
	"F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Num Lock",
	"Scroll Lock",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Left Shift",
	"Right Shift",
	"Left Control",
	"Right Control",
	"Left Alt",
	"Right Alt"
};

const void RenderOverlay() {

	//ImGui::PopFont();
	ImGui::PushFont(FNInterface::Internal::DefaultFont);

	ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

	// Update background drawing
	if (!FSettings.Interface.m_bIncreaseFPS) {
		FNInterface::Internal::Decoration::UpdateAllPoints();
	}

	ImColor OverlayRender = ImColor(1.0f, 1.0f, 1.0f, 0.6f);
	ImColor SolidRender = ImColor(1.0f, 1.0f, 1.0f, 1.0f);

	bool DownSights = GetAsyncKeyState(FSettings.Aim.m_kbADSBind) || (FNDetector::Gamepad.Connected && FNDetector::Gamepad.LeftTrigger > 0.25f);
	if (DownSights && TargetProcessFocusedGlobal) {
		FNDetector::ADSAmount = FNUtilities::Clamp(FNDetector::ADSAmount + 0.03f);
	}
	else {
		FNDetector::ADSAmount = FNUtilities::Clamp(FNDetector::ADSAmount - 0.03f);
	}
	int AimFOV = FNUtilities::LinearInterpolate(FSettings.Aim.m_iFOV, FSettings.Aim.m_iADSFOV, FNUtilities::NormalizedSine(FNDetector::ADSAmount));

	// Draw FOV Circle
	if (FSettings.Interface.m_bShowFOV) {
		DrawList->AddCircle(ImVec2(CENTERX, CENTERY), AimFOV / 2, OverlayRender);
	}

	DrawList->AddText(ImVec2(10, SCREENY - 40), OverlayRender, CSTR(std::string("Fortnite Neural Network Cheat [Public Version]")));
	DrawList->AddText(ImVec2(10, SCREENY - 25), OverlayRender, CSTR(std::string(std::string(xor("Press ")) + KeycodeIds[FSettings.Interface.m_kbMenuBind] + std::string(xor(" to ") + std::string(ShowMenu ? xor("hide") : xor("show")) + std::string(xor(" menu"))))));

	std::string FPS = to_string(ImGui::GetIO().Framerate);
	FPS = FPS.substr(0, FPS.find(xor("."))) + xor(" FPS");
	std::string CaptureFPS = to_string(FNDetector::CaptureFramerate);
	CaptureFPS = xor(" (") + CaptureFPS.substr(0, CaptureFPS.find(xor("."))) + xor(" Capture, ");
	std::string InferenceFPS = to_string(FNDetector::InferenceFramerate);
	InferenceFPS = InferenceFPS.substr(0, InferenceFPS.find(xor("."))) + xor(" Inference)");
	FPS += CaptureFPS + InferenceFPS;

	if (FSettings.Interface.m_bShowFPS)
		DrawList->AddText(ImVec2(SCREENX - 25 - CalcTextSize(CSTR(FPS)).x, SCREENY - 25), OverlayRender, CSTR(FPS));
	
	// Loop through overlaid boxes
	if (FSettings.Interface.m_bOverlayBoxes) {
		for (int i = 0; i < FNDetector::RectOverlayDrawList.size(); i++) {
			FNUtilities::DetectionRect Curr = FNDetector::RectOverlayDrawList[i];
			ImColor Box = ImColor(1.0f, 1.0f, 1.0f, 0.25f);
			DrawList->AddRectFilled(ImVec2(Curr.x, Curr.y), ImVec2(Curr.x + Curr.w, Curr.y + Curr.h), Box);
			//DrawList->AddRect(ImVec2(Curr.x, Curr.y), ImVec2(Curr.x + Curr.w, Curr.y + Curr.h), Box);
		}

		FNDetector::RectOverlayDrawList.clear();
	}

	// Draw Debugging Capture Rectangle
	FNUtilities::DetectionRect PlayerCompensation = FNUtilities::LinearInterpolateRect(FNDetector::Player, FNDetector::PlayerADS, FNUtilities::NormalizedSine(FNDetector::ADSAmount));
	FNUtilities::DetectionRect CurrPlayerOutBounds = PlayerCompensation;
	if (FSettings.Interface.m_bShowAvoidance) {
		DrawList->AddRectFilled(ImVec2(CurrPlayerOutBounds.x, CurrPlayerOutBounds.y), ImVec2(CurrPlayerOutBounds.x + CurrPlayerOutBounds.w, CurrPlayerOutBounds.y + CurrPlayerOutBounds.h), ImColor(1.0f, 0.0f, 0.0f, 0.1f));
		for (int i = PlayerCompensation.y; i < PlayerCompensation.y + PlayerCompensation.h; i++) {
			if (i % 10 == 0) {
				DrawList->AddLine(ImVec2(CurrPlayerOutBounds.x, i), ImVec2(CurrPlayerOutBounds.x + CurrPlayerOutBounds.w, i), ImColor(1.0f, 0.0f, 0.0f, 0.1f), 1);
			}
		}
		DrawList->AddRect(ImVec2(CurrPlayerOutBounds.x, CurrPlayerOutBounds.y), ImVec2(CurrPlayerOutBounds.x + CurrPlayerOutBounds.w, CurrPlayerOutBounds.y + CurrPlayerOutBounds.h), ImColor(1.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0, 2);
	}

	if (FSettings.Interface.m_bShowDetectionRange) {
		DrawList->AddRectFilled(ImVec2(FNDetector::DetectionLeft, FNDetector::DetectionTop), ImVec2(FNDetector::DetectionLeft + FNDetector::DetectionWidth, FNDetector::DetectionTop + FNDetector::DetectionHeight), ImColor(1.0f, 1.0f, 1.0f, 0.25f));
	}

	FNInterface::Internal::RenderDebugMenu();
}

// Window Process Handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
Mat CurrentFrame;
List<FLOAT> StoredFPS;
namespace FNInterface {
	namespace Window {

		// Get PID from Executable Name
		DWORD GetProcessId(LPCSTR ProcessName) {
			PROCESSENTRY32 pt;
			HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			pt.dwSize = sizeof(PROCESSENTRY32);
			if (Process32First(hsnap, &pt)) {
				while (Process32Next(hsnap, &pt)) {
					if (!lstrcmpiA(pt.szExeFile, ProcessName)) {
						CloseHandle(hsnap);
						return pt.th32ProcessID;
					}
				}
			}
			CloseHandle(hsnap);
			return 0;
		}

		void HandleInput() {

			for (int i = 0; i < 5; i++) {
				ImGui::GetIO().MouseDown[i] = FALSE;
			}
			int Button = -1;
			if (GetAsyncKeyState(VK_LBUTTON)) {
				Button = 0;
			}
			if (Button != -1 && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
				ImGui::GetIO().MouseDown[Button] = TRUE;
			}
		}

		bool HasInitialized = FALSE;
		int RenderLoops = 0;
		void Render() {
			if (GetAsyncKeyState(FSettings.Interface.m_kbMenuBind) & 1) 
				ShowMenu = !ShowMenu;

			if (!HasInitialized) {
				FNInterface::Initialize();
				HasInitialized = TRUE;
			}

			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::GetIO().MouseDrawCursor = ShowMenu;
			FNInterface::SetStyle();

			FNInterface::Internal::Decoration::CurrentHue -= 0.001f;
			if (FNInterface::Internal::Decoration::CurrentHue < 0.0f)
				FNInterface::Internal::Decoration::CurrentHue = 1.0f;

			if (!FSettings.Interface.m_bHideOverlay) {
				RenderOverlay();
			}

			RenderLoops++;
			if (RenderLoops > 50) 
				RenderLoops = 0;

			if (ShowMenu == TRUE) {
				HandleInput();
				FNInterface::Internal::RenderBaseMenu();
				SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
				UpdateWindow(Overlay.Hwnd);
				SetFocus(Overlay.Hwnd);
			}
			else {
				SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
				UpdateWindow(Overlay.Hwnd);
			}
			ImGui::EndFrame();

			DirectX9.pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
			if (DirectX9.pDevice->BeginScene() >= 0) {
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				DirectX9.pDevice->EndScene();
			}

			HRESULT result = DirectX9.pDevice->Present(NULL, NULL, NULL, NULL);
			if (result == D3DERR_DEVICELOST && DirectX9.pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
				ImGui_ImplDX9_InvalidateDeviceObjects();
				DirectX9.pDevice->Reset(&DirectX9.pParameters);
				ImGui_ImplDX9_CreateDeviceObjects();
			}
		}

		// Runs screencapture inside of a seperate thread
		bool HasRanInferenceOnCurrentFrame = FALSE;
		bool RequestUIFPSUpdate = FALSE;
		void DoMultithreadedCapture() {

			// Run while under conditions
			while (DirectX9.Message.message != WM_QUIT && !GetAsyncKeyState(m_iExitKey) && !FNInterface::ShouldExit && !FNUtilities::ThreadRelease) {

				//FNDetector::CaptureFramerate = 0.0f;

				// Make sure that the cheat's neural network has initialized before we infer anything.
				if (FNDetector::HasInitialized) {

					auto StartTime = chrono::high_resolution_clock::now();

					// Capture Screen
					CurrentFrame = FNDetector::PullScreenshot();

					FNUtilities::UpdateLog(FNUtilities::ELogFileLines::CaptureSize, CSTR(std::string(
						std::string(xor("Capture: ")) +
						std::string(to_string(CurrentFrame.cols)) +
						std::string(xor("x")) +
						std::string(to_string(CurrentFrame.rows))
					)));
					FNUtilities::UpdateLog(FNUtilities::ELogFileLines::ScreenResolution, CSTR(std::string(
						std::string(xor("Screen: ")) +
						std::string(to_string(SCREENX)) +
						std::string(xor("x")) +
						std::string(to_string(SCREENY))
					)));
					FNUtilities::UpdateLog(FNUtilities::ELogFileLines::InferenceSize, CSTR(std::string(
						std::string(xor("BlobScale: ")) +
						std::string(to_string(m_iNetworkBlobScale)) +
						std::string(xor("x")) +
						std::string(to_string(m_iNetworkBlobScale))
					)));
					FNDetector::CreateBlob(CurrentFrame);
					HasRanInferenceOnCurrentFrame = FALSE;
					ThreadSleep(1000 / FSettings.Interface.m_iFPSCap);

					auto EndTime = chrono::high_resolution_clock::now();
					DOUBLE ElapsedTimeInMs = std::chrono::duration<DOUBLE, std::milli>(EndTime - StartTime).count();

					if (RequestUIFPSUpdate) {
						FNDetector::CaptureFramerate = CAST(FLOAT, 1000 / ElapsedTimeInMs);

						RequestUIFPSUpdate = FALSE;
					}
				}
			}
		}

		// Runs AI inference inside of a different thread to prioritize performance.
		int MultithreadedInferenceLoops = 0;
		void DoMultithreadedInference() {

			// Run while under conditions
			while (DirectX9.Message.message != WM_QUIT && !GetAsyncKeyState(m_iExitKey) && !FNInterface::ShouldExit && !FNUtilities::ThreadRelease) {

				// Increase Iterator
				MultithreadedInferenceLoops++;
				if (MultithreadedInferenceLoops > 50) {
					MultithreadedInferenceLoops = 0;
				}

				// Make sure that the cheat's neural network has initialized before we infer anything.
				if (FNDetector::HasInitialized) {
					// Only run inference if aim is enabled
					if (FSettings.Aim.m_bEnabled && !HasRanInferenceOnCurrentFrame) {

						auto StartTime = chrono::high_resolution_clock::now();

						FNDetector::Infer(CurrentFrame);
						HasRanInferenceOnCurrentFrame = true;
						ThreadSleep(1000 / FSettings.Interface.m_iFPSCap);

						auto EndTime = chrono::high_resolution_clock::now();
						DOUBLE ElapsedTimeInMs = std::chrono::duration<DOUBLE, std::milli>(EndTime - StartTime).count();

						if (MultithreadedInferenceLoops == 50) {
							FNDetector::InferenceFramerate = CAST(FLOAT, 1000 / ElapsedTimeInMs);
							RequestUIFPSUpdate = true;
						}
					}
				}
			}
		}

		// Rendering pipeline and "main loop"
		void MainLoop() {
			// State Variables
			int Loops = 0;
			static RECT OldRect;
			memset(&DirectX9.Message, 0, sizeof(MSG));

			SetWindowText(GetConsoleWindow(), CSTR(FNUtilities::RandomString(10)));

			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DoMultithreadedCapture, 0, 0, 0);
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DoMultithreadedInference, 0, 0, 0);
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)FNDetector::MoveMouseThread, 0, 0, 0);

			while (DirectX9.Message.message != WM_QUIT && !GetAsyncKeyState(m_iExitKey) && !FNInterface::ShouldExit) {
				// Get the gamepad state
				if (!FSettings.Interface.m_bIncreaseFPS)
					FNDetector::Gamepad = FNDetector::GetGamepadState();

				// Check every 500 frames
				// I think this fixes crashes - Zanelul
				if (Loops % 500 == 0) {

					// Check if game is closed
					if (!FNUtilities::GetTargetWindow()) {
						FNUtilities::ExitProgram(EExitCodes::None);
					}
				}
				Loops++;

				if (PeekMessage(&DirectX9.Message, Overlay.Hwnd, 0, 0, PM_REMOVE)) {
					TranslateMessage(&DirectX9.Message);
					DispatchMessage(&DirectX9.Message);
				}
				HWND ForegroundWindow = GetForegroundWindow();
				if (ForegroundWindow == Process.Hwnd) {
					HWND TempProcessHwnd = GetWindow(ForegroundWindow, GW_HWNDPREV);
					SetWindowPos(Overlay.Hwnd, TempProcessHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}

				RECT TempRect;
				POINT TempPoint;
				memset(&TempRect, 0, sizeof(RECT));
				memset(&TempPoint, 0, sizeof(POINT));

				GetClientRect(Process.Hwnd, &TempRect);
				ClientToScreen(Process.Hwnd, &TempPoint);

				TempRect.left = TempPoint.x;
				TempRect.top = TempPoint.y;
				ImGuiIO& io = ImGui::GetIO();
				io.ImeWindowHandle = Process.Hwnd;

				if (TempRect.left != OldRect.left || TempRect.right != OldRect.right || TempRect.top != OldRect.top || TempRect.bottom != OldRect.bottom) {
					OldRect = TempRect;
					Process.WindowWidth = TempRect.right;
					Process.WindowHeight = TempRect.bottom;
					DirectX9.pParameters.BackBufferWidth = Process.WindowWidth;
					DirectX9.pParameters.BackBufferHeight = Process.WindowHeight;
					SetWindowPos(Overlay.Hwnd, (HWND)0, TempPoint.x, TempPoint.y, Process.WindowWidth, Process.WindowHeight, SWP_NOREDRAW);
					DirectX9.pDevice->Reset(&DirectX9.pParameters);
				}
				Render();

				// Prevent CPU throttling, apply FPS limiting
				ThreadSleep(1000 / FSettings.Interface.m_iFPSCap);
			}
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			if (DirectX9.pDevice != NULL) {
				DirectX9.pDevice->EndScene();
				DirectX9.pDevice->Release();
			}
			if (DirectX9.IDirect3D9 != NULL) {
				DirectX9.IDirect3D9->Release();
			}
			DestroyWindow(Overlay.Hwnd);
			UnregisterClass(Overlay.WindowClass.lpszClassName, Overlay.WindowClass.hInstance);
		}

		LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
			if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
				return true;

			switch (Message) {
			case WM_DESTROY:
				if (DirectX9.pDevice != NULL) {
					DirectX9.pDevice->EndScene();
					DirectX9.pDevice->Release();
				}
				if (DirectX9.IDirect3D9 != NULL) {
					DirectX9.IDirect3D9->Release();
				}
				PostQuitMessage(0);
				exit(4);
				break;
			case WM_SIZE:
				if (DirectX9.pDevice != NULL && wParam != SIZE_MINIMIZED) {
					ImGui_ImplDX9_InvalidateDeviceObjects();
					DirectX9.pParameters.BackBufferWidth = LOWORD(lParam);
					DirectX9.pParameters.BackBufferHeight = HIWORD(lParam);
					HRESULT hr = DirectX9.pDevice->Reset(&DirectX9.pParameters);
					if (hr == D3DERR_INVALIDCALL)
						IM_ASSERT(0);
					ImGui_ImplDX9_CreateDeviceObjects();
				}
				break;
			default:
				return DefWindowProc(hWnd, Message, wParam, lParam);
				break;
			}
			return 0;
		}
		bool DirectXInit() {
			if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9.IDirect3D9))) {
				return false;
			}

			D3DPRESENT_PARAMETERS Params = { 0 };
			Params.Windowed = TRUE;
			Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
			Params.hDeviceWindow = Overlay.Hwnd;
			Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
			Params.BackBufferFormat = D3DFMT_A8R8G8B8;
			Params.BackBufferWidth = Process.WindowWidth;
			Params.BackBufferHeight = Process.WindowHeight;
			Params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			Params.EnableAutoDepthStencil = TRUE;
			Params.AutoDepthStencilFormat = D3DFMT_D16;
			Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			if (FAILED(DirectX9.IDirect3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Overlay.Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9.pDevice))) {
				DirectX9.IDirect3D9->Release();
				return false;
			}

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui_ImplWin32_Init(Overlay.Hwnd);
			ImGui_ImplDX9_Init(DirectX9.pDevice);
			DirectX9.IDirect3D9->Release();
			return true;
		}
		void SetupWindow() {
			Overlay.WindowClass = {
				sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, Overlay.Name, LoadIcon(nullptr, IDI_APPLICATION)
			};

			RegisterClassEx(&Overlay.WindowClass);
			if (Process.Hwnd) {
				static RECT TempRect = { NULL };
				static POINT TempPoint;
				GetClientRect(Process.Hwnd, &TempRect);
				ClientToScreen(Process.Hwnd, &TempPoint);
				TempRect.left = TempPoint.x;
				TempRect.top = TempPoint.y;
				Process.WindowWidth = TempRect.right;
				Process.WindowHeight = TempRect.bottom;
			}

			Overlay.Hwnd = CreateWindowEx(NULL, Overlay.Name, Overlay.Name, WS_POPUP | WS_VISIBLE, Process.WindowLeft, Process.WindowTop, Process.WindowWidth, Process.WindowHeight, NULL, NULL, 0, NULL);
			DwmExtendFrameIntoClientArea(Overlay.Hwnd, &DirectX9.Margin);
			SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
			ShowWindow(Overlay.Hwnd, SW_SHOW);
			UpdateWindow(Overlay.Hwnd);
		}
	}
}

// Initialization
const void InitializeFN() {
	// Display Welcome Message
	if (GetConsoleWindow()) {
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}
	SetWindowText(GetConsoleWindow(), xor("Cheat"));
	CLI::WriteMessage(xor("Starting... (") + std::string(m_chrAppVersion) + xor(")\n"));
	CLI::WriteMessage(xor("Make sure the game is using \"Fullscreen Windowed\" mode!\n"));

	// Check if the cheat is already running
	FNReversal::CheckIsRunning();

	// Replace EAC Config
	//FNUtilities::ReplaceEACConfig();

	// Check if the game is running
	if (!FNUtilities::GetTargetWindow()) {
		FNUtilities::ShowMessage(xor("The game is not running! Please start Fortnite before running the cheat."), EExitCodes::GameNotStarted);
	}

	ThreadSleep(1000);
}
const void SetupCheat() {
	HWND Current = FNUtilities::GetTargetWindow();

	if (Current) {
		CLI::WriteMessage(xor("Found Target Window!\n"));

		// Get PID and Handle
		CLI::WriteMessage(xor("> Getting Target PID...\n"));
		Process.ID = GetCurrentProcessId();
		CLI::WriteMessage(xor("> Getting Target Handle...\n"));
		Process.Handle = GetCurrentProcess();
		Process.Hwnd = Current;

		// Get Window Size
		RECT TempRect;
		GetWindowRect(Process.Hwnd, &TempRect);
		Process.WindowWidth = TempRect.right - TempRect.left;
		Process.WindowHeight = TempRect.bottom - TempRect.top;
		Process.WindowLeft = TempRect.left;
		Process.WindowRight = TempRect.right;
		Process.WindowTop = TempRect.top;
		Process.WindowBottom = TempRect.bottom;

		CLI::WriteMessage(xor("> Cloning Window Class...\n"));
		CHAR TempTitle[MAX_PATH];
		GetWindowText(Process.Hwnd, TempTitle, sizeof(TempTitle));
		Process.Title = TempTitle;
		CHAR TempClassName[MAX_PATH];
		GetClassName(Process.Hwnd, TempClassName, sizeof(TempClassName));
		Process.ClassName = TempClassName;
		CHAR TempPath[MAX_PATH];
		GetModuleFileNameEx(Process.Handle, NULL, TempPath, sizeof(TempPath));
		Process.Path = TempPath;

		// Setup overlay and DX9
		CLI::WriteMessage(xor("> Initializing Renderer...\n"));
		Overlay.Name = CSTR(FNUtilities::RandomString(10));
		CLI::WriteMessage(xor("> Creating Menu...\n"));
		FNInterface::Window::SetupWindow();
		FNInterface::Window::DirectXInit();

		// This inference call forces the detector to initialize prematurely instead of initializing when license key is entered
		CLI::WriteMessage(xor("> Initializing Detector...\n"));
		FNDetector::Initialize();
		CurrentFrame = FNDetector::PullScreenshot();
		FNDetector::CreateBlob(CurrentFrame);
		FNDetector::Infer(CurrentFrame);

		SetForegroundWindow(Current);
		CLI::WriteMessage(xor(""));
		CLI::WriteMessage(xor("Finished!\n"));
		ThreadSleep(500);
		ShowMenu = TRUE;
	}
}

int main() {

	// Initialize
	InitializeFN();
	SetupCheat();

	// Hide Console
	#ifndef DISABLE_DEBUGGER_CHECKS
	if (GetConsoleWindow()) {
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}
	#endif

	// Render User Interface
	FNInterface::Window::MainLoop();

	// Exit
	return EXIT_SUCCESS;
}