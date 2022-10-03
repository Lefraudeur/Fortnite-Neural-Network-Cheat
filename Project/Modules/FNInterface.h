#pragma once

// Include Headers
#include "../Includes.h"
#include "FNDetector.h"
#include "FNUtilities.h"
#include "GUI/Fonts/FNEmbeddedFonts.h"
#include "../Utilities/FNSettings.h"

// Include GUI
#include "../Includes.h"
#include "../Utilities/Includes/IncludeImGui.h"

// Options
#undef DISABLE_INCREASE_FPS
#undef DISABLE_ADAPTIVE_SMOOTHING

bool ShowMenu = true;
bool HasTriedSerial = false;
bool AcceptedToS = false;
bool HasTriedCachedLogin = false;

// ImGui Window Message handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

using namespace ImGui;

// GUI Namespace
namespace FNInterface{

    // Window Parameters
    constexpr int WindowWidth = 720;
    constexpr int WindowHeight = 360;
    const ImVec4 NegativeColor = ImVec4(1.00f, 0.25f, 0.25f, 1.00f);
    FLOAT ImHue = 0.0f;
    HWND Handle = nullptr;
    FLOAT MaxDrawOpacity = 0.00f;

    // State Variables
    std::string WindowTitle = m_chrAppTitle;
    std::string ExpirationDateString = "";
    bool HasUsedLicenseKey = FALSE;

    // Should window not close?
    inline bool ShouldExit = FALSE;

    // Drawing Menus
    namespace Internal{

        // Responsible for rendering decorations on the menu window
        namespace Decoration {

            ImVec4 BorderColor = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);

            FLOAT CurrentHue = 0.0f; // It's 4:20AM

            const void DrawRainbowHeader(ImVec2 WindowPos, ImVec2 WindowSize, FLOAT Thickness = 5.0f, FLOAT Saturation = 1.0f, FLOAT Value = 1.0f, bool Border = TRUE) {
                WindowSize.x -= 1;
                for (int i = 0; i < WindowSize.x; i++) {
                    ImVec4 CurrentColor;
                    CurrentColor.w = 1.0f;
                    ColorConvertHSVtoRGB(CurrentHue + CAST(FLOAT, i / WindowSize.x), Saturation, Value, CurrentColor.x, CurrentColor.y, CurrentColor.z);
                    GetForegroundDrawList()->AddLine(ImVec2(i + WindowPos.x, 0 + WindowPos.y), ImVec2(i + WindowPos.x, Thickness + WindowPos.y), GetColorU32(CurrentColor), 1.0f);
                }
                if (Border) {
                    GetForegroundDrawList()->AddLine(ImVec2(WindowPos.x, WindowPos.y), ImVec2(WindowPos.x + WindowSize.x, WindowPos.y), GetColorU32(BorderColor), 1.0f);
                    GetForegroundDrawList()->AddLine(ImVec2(WindowPos.x, WindowPos.y), ImVec2(WindowPos.x, WindowPos.y + Thickness), GetColorU32(BorderColor), 1.0f);
                    GetForegroundDrawList()->AddLine(ImVec2(WindowPos.x + WindowSize.x, WindowPos.y), ImVec2(WindowPos.x + WindowSize.x, WindowPos.y + Thickness), GetColorU32(BorderColor), 1.0f);
                }
            }

            // State Variables
            FLOAT MaxSpeed = 0.25;

            // Point structure
            struct RenderPoint {
                ImVec2 CurrentPosition;
                FLOAT Rotation;
                FLOAT Speed;
            };

            // All Points
            List<RenderPoint> RenderPoints;

            // Generate random float between 0 and N
            FLOAT RandomBelowN(FLOAT N) {
                return CAST(FLOAT, rand()) / (CAST(FLOAT, RAND_MAX / N));
            }

            // Create points and add them to the array
            const void InitializePoints(int NumPoints) {
                for (int x = 0; x < NumPoints; x++) {

                    // Initialize with random Position, Rotation, and Speed
                    RenderPoint NewPoint;
                    NewPoint.CurrentPosition = ImVec2(RandomBelowN(SCREENX), RandomBelowN(SCREENY));
                    NewPoint.Rotation = RandomBelowN(360);
                    NewPoint.Speed = RandomBelowN(MaxSpeed / 2) + MaxSpeed / 2;

                    // Add it to the array
                    RenderPoints.Add(NewPoint);
                }
            }

            ImVec2 RadiusPoint(ImVec2 Origin, FLOAT Rotation, FLOAT Distance) {
                float X = Origin.x + Distance * cos(Rotation);
                float Y = Origin.y + Distance * sin(Rotation);
                return ImVec2(X, Y);
            }

            // Recursive function to update the position of a single point
            const RenderPoint UpdatePositionOfPoint(RenderPoint Current) {
                ImVec2 OldPosition = Current.CurrentPosition;
                RenderPoint NewRenderPoint;
                NewRenderPoint.CurrentPosition = RadiusPoint(OldPosition, Current.Rotation, Current.Speed);
                NewRenderPoint.Rotation = Current.Rotation;
                NewRenderPoint.Speed = Current.Speed;

                // Check if new point is out-of-bounds
                if (NewRenderPoint.CurrentPosition.x < 0 || NewRenderPoint.CurrentPosition.y < 0 || NewRenderPoint.CurrentPosition.x > SCREENX || NewRenderPoint.CurrentPosition.y > SCREENY) {
                    NewRenderPoint.Speed *= -1;
                    NewRenderPoint.Rotation += RandomBelowN(45);
                }

                return NewRenderPoint;
            }

            // Thread to handle the above functions
            const void UpdateAllPoints() {

                ImDrawList* FGDrawList = GetBackgroundDrawList();

                // Update position of every point
                for (int i = 0; i < RenderPoints.size(); i++) {
                    RenderPoints[i] = UpdatePositionOfPoint(RenderPoints[i]);
                }

                // Code flexing basically
                if (ShowMenu && FSettings.Interface.m_AnimateBackground) {
                    MaxDrawOpacity = FNUtilities::Clamp(MaxDrawOpacity + 0.01f, 0.0f, 0.5f);
                }
                else {
                    MaxDrawOpacity = FNUtilities::Clamp(MaxDrawOpacity - 0.01f, 0.0f, 0.5f);
                }
                for (int i = 0; i < RenderPoints.size(); i++) {
                    RenderPoint NewRenderPoint = RenderPoints[i];
                    FGDrawList->AddCircle(NewRenderPoint.CurrentPosition, 1, ImColor(1.0f, 1.0f, 1.0f, MaxDrawOpacity * 2));

                    // Draw line to each point
                    for (int x = 0; x < RenderPoints.size(); x++) {

                        ImColor LineColor = ImColor(1.0f, 1.0f, 1.0f);
                        FLOAT Distance = sqrt(pow(NewRenderPoint.CurrentPosition.x - RenderPoints[x].CurrentPosition.x, 2) + pow(NewRenderPoint.CurrentPosition.y - RenderPoints[x].CurrentPosition.y, 2));
                        if (Distance < WindowWidth / 10) {
                            LineColor.Value.w = FNUtilities::LinearInterpolate(MaxDrawOpacity, 0, Distance / (WindowWidth / 10));
                            FGDrawList->AddLine(NewRenderPoint.CurrentPosition, RenderPoints[x].CurrentPosition, LineColor);
                        }
                    }
                }
            }
        }

        // Fonts
        ImFont* ConsoleFont;
        ImFont* DefaultFont;
        ImFont* HeadingFont;

        enum EMenuTabs {
            Softaim,
            Settings,
            Credits,
            Licensing,
            About
        };
        EMenuTabs CurrentTab = EMenuTabs::Softaim;
        const ImVec2 MenuSize = ImVec2(WindowWidth, WindowHeight);

        // Padding between controls
        const void Space(bool NoSeparator = FALSE) {
            Spacing();
            if (!NoSeparator) {
                Spacing();
                Separator();
            }
            Spacing();
            Spacing();
        }

        // Draw a fancy colored header
        const void DrawHeader(const CHAR* Text, ImVec4 MainColor = FSettings.Interface.m_cDetailColor) {
            PopFont();
            PushFont(HeadingFont);

            FLOAT Width = ImGui::GetWindowSize().x;
            FLOAT TextWidth = ImGui::CalcTextSize(Text).x;

            //ImGui::SetCursorPosX((Width - TextWidth) * 0.5f);

            TextColored(
                ImVec4(
                    MainColor.x + (1.0 - MainColor.x) / 2,
                    MainColor.y + (1.0 - MainColor.y) / 2,
                    MainColor.z + (1.0 - MainColor.z) / 2,
                    0.9f
                ),
                Text
            );
            PopFont();
            PushFont(DefaultFont);
        }

        // Draw text with a monospace font
        const void TextMonospace(const CHAR* Text) {
            PopFont();
            PushFont(ConsoleFont);
            ImGui::Text(Text);
            PopFont();
            PushFont(DefaultFont);
        }

        // Do Login Stuff
        std::string PadZeroes(std::string Input) {
            return (Input.length() < 2 ? "0" + Input : Input);
        }

        // Automatically fills in ImGuiColorEditFlags
        bool ColorEdit(const CHAR* label, FLOAT col[4]) {
            auto Flags =
                ImGuiColorEditFlags_AlphaPreviewHalf |
                ImGuiColorEditFlags_PickerHueWheel |
                ImGuiColorEditFlags_NoAlpha |
                ImGuiColorEditFlags_NoOptions |
                ImGuiColorEditFlags_NoTooltip |
                ImGuiColorEditFlags_NoSidePreview |
                ImGuiColorEditFlags_NoDragDrop |
                ImGuiColorEditFlags_NoInputs;

            return ColorEdit4(label, col, Flags);
        }

        // Help Marker
        const void DrawHelpMarker(const CHAR* desc) {
            SameLine(GetWindowWidth() - 50);
            TextDisabled(xor("[?]"));
            if (IsItemHovered()) {
                BeginTooltip();
                PushTextWrapPos(GetFontSize() * 35.0f);
                TextUnformatted(desc);
                PopTextWrapPos();
                EndTooltip();
            }
        }

        // Render GUI Tabs
        const void RenderSoftaim() {
            DrawHeader(xor("Softaim"));
            Spacing();
            Checkbox(FNDetector::Gamepad.Connected ? xor("Enable Controller Softaim") : xor("Enable Softaim"), &FSettings.Aim.m_bEnabled, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor("Enables the softaim."));
            Space();

            if (FSettings.Aim.m_bEnabled) {
                DrawHeader(xor("Softaim Settings"));
                Spacing();
                if (FNDetector::Gamepad.Connected) {
                    #ifndef DISABLE_ADAPTIVE_SMOOTHING
                    Checkbox(xor("Adaptive Smoothing (Controller)"), &FSettings.Aim.m_bAdaptiveSmoothing, FSettings.Interface.m_cDetailColor);
                    DrawHelpMarker(xor("Automatically sets your smoothing based on how far down Left Trigger is being held."));
                    #endif
                    if (!FSettings.Aim.m_bAdaptiveSmoothing) {
                        SliderFloat(xor("Smoothing"), &FSettings.Aim.m_flADSSmoothing, 0.00f, 1.00f);
                        DrawHelpMarker(xor("Moves the mouse to the target player natrually instead of snapping instantly.\n\nTriggers: When aiming down sights (ADS)\n\nRecommended: 0.2"));
                    }
                    TextDisabled(xor("If you are playing on keyboard/mouse, make sure to unplug any\ncontrollers!"));
                }
                else {
                    if (!FSettings.Aim.m_bHardSmoothed) {
                        SliderFloat(xor("Smoothing"), &FSettings.Aim.m_flSmoothing, 0.00f, 1.00f);
                        DrawHelpMarker(xor("Moves the mouse to the target player natrually instead of snapping instantly.\n\nTriggers: When hip-firing (Not aiming down sights)\n\nRecommended: 0.2"));
                        SliderFloat(xor("ADS Smoothing"), &FSettings.Aim.m_flADSSmoothing, 0.00f, 1.00f);
                        DrawHelpMarker(xor("Moves the mouse to the target player natrually instead of snapping instantly.\n\nTriggers: When aiming down sights (ADS)\n\nRecommended: 0.2"));
                        Space(true);
                    }
                    Checkbox(xor("Disable Smoothing (Experimental)"), &FSettings.Aim.m_bHardSmoothed, FSettings.Interface.m_cDetailColor);
                    DrawHelpMarker(xor("Disables smoothing entirely, but can cause jittery or indecisive aiming!"));
                }

                FNUtilities::UpdateLog(FNUtilities::ELogFileLines::Smoothing, CSTR(std::string(
                    std::string(xor("Smooth: [")) +
                    std::string(to_string(FSettings.Aim.m_flSmoothing)) +
                    std::string(xor(",")) +
                    std::string(to_string(FSettings.Aim.m_flADSSmoothing)) +
                    std::string(xor("]"))
                )));
                Space();

                SliderInt(xor("FOV"), &FSettings.Aim.m_iFOV, 1, m_iNetworkBlobScale);
                DrawHelpMarker(xor("This is the maximum amount of pixels that the softaim can move the mouse.\n\nRecommended: 150-300"));
                SliderInt(xor("ADS FOV"), &FSettings.Aim.m_iADSFOV, 1, m_iNetworkBlobScale);
                DrawHelpMarker(xor("This is the maximum amount of pixels that the softaim can move the mouse while you are aiming down-sights.\n\nRecommended: 150-300"));
                FNUtilities::UpdateLog(FNUtilities::ELogFileLines::FieldOfView, CSTR(std::string(
                    std::string(xor("Field: [")) +
                    std::string(to_string(FSettings.Aim.m_iFOV)) +
                    std::string(xor(",")) +
                    std::string(to_string(FSettings.Aim.m_iADSFOV)) +
                    std::string(xor("]"))
                )));
                FNUtilities::UpdateLog(FNUtilities::ELogFileLines::Sensitivity, CSTR(std::string(
                    std::string(xor("Sens: [")) +
                    std::string(to_string(FSettings.Sensitivity.m_flSensitivityX)) +
                    std::string(xor(",")) +
                    std::string(to_string(FSettings.Sensitivity.m_flSensitivityY)) +
                    std::string(xor(",")) +
                    std::string(to_string(FSettings.Sensitivity.m_flTargetingSensitivity)) +
                    std::string(xor("]"))
                )));
                Space(true);

                Checkbox(xor("Show FOV"), &FSettings.Interface.m_bShowFOV, FSettings.Interface.m_cDetailColor);
                DrawHelpMarker(xor("Shows the softaim's FOV on the overlay."));
                if (FSettings.Interface.m_bHideOverlay) {
                    TextColored(NegativeColor, xor ("Warning: Hide Overlay is turned on!"));
                }
                Space();

                DrawHeader(xor("Target Location"));
                Spacing();
                ImGuiStyle& Style = GetStyle();
                Style.FrameBorderSize = 1;
                ImVec4* colors = GetStyle().Colors;
                int AimButtonPad = 12;
                if (FSettings.Aim.m_eAimTargetBone == EAimTargetBones::Head)
                    colors[ImGuiCol_Button] = FSettings.Interface.m_cDetailColor;
                if (Button(xor("Head"), ImVec2(CalcTextSize(xor("Head")).x + AimButtonPad * 2, CalcTextSize(xor("Head")).y + AimButtonPad))) {
                    FSettings.Aim.m_eAimTargetBone = EAimTargetBones::Head;
                }
                if (FSettings.Aim.m_eAimTargetBone == EAimTargetBones::Head)
                    colors[ImGuiCol_Button] = ImVec4(0.83f, 0.83f, 0.83f, 0.00f);
                SameLine();
                if (FSettings.Aim.m_eAimTargetBone == EAimTargetBones::Chest)
                    colors[ImGuiCol_Button] = FSettings.Interface.m_cDetailColor;
                if (Button(xor("Chest"), ImVec2(CalcTextSize(xor("Head")).x + AimButtonPad * 2, CalcTextSize(xor("Chest")).y + AimButtonPad))) {
                    FSettings.Aim.m_eAimTargetBone = EAimTargetBones::Chest;
                }
                if (FSettings.Aim.m_eAimTargetBone == EAimTargetBones::Chest)
                    colors[ImGuiCol_Button] = ImVec4(0.83f, 0.83f, 0.83f, 0.00f);
                SameLine();
                if (FSettings.Aim.m_eAimTargetBone == EAimTargetBones::Feet)
                    colors[ImGuiCol_Button] = FSettings.Interface.m_cDetailColor;
                if (Button(xor("Feet"), ImVec2(CalcTextSize(xor("Head")).x + AimButtonPad * 2, CalcTextSize(xor("Feet")).y + AimButtonPad))) {
                    FSettings.Aim.m_eAimTargetBone = EAimTargetBones::Feet;
                }
                if (FSettings.Aim.m_eAimTargetBone == EAimTargetBones::Feet)
                    colors[ImGuiCol_Button] = ImVec4(0.83f, 0.83f, 0.83f, 0.00f);
                Style.FrameBorderSize = 0;
                SameLine();
                switch (FSettings.Aim.m_eAimTargetBone) {
                case EAimTargetBones::Head:
                    TextDisabled(xor(" Selected: Head"));
                    break;
                case EAimTargetBones::Chest:
                    TextDisabled(xor(" Selected: Chest"));
                    break;
                case EAimTargetBones::Feet:
                    TextDisabled(xor(" Selected: Feet"));
                    break;
                }
                DrawHelpMarker(xor("Where the softaim should aim on the target.\n\nRecommended: Chest"));
                Space(true);

                if (FSettings.Aim.m_iAntiRecoilPixels > MAX(FSettings.Aim.m_iFOV, FSettings.Aim.m_iADSFOV)) {
                    FSettings.Aim.m_iAntiRecoilPixels = MAX(FSettings.Aim.m_iFOV, FSettings.Aim.m_iADSFOV);
                }
                SliderInt(xor("Lower Target"), &FSettings.Aim.m_iAntiRecoilPixels, 0, 15);
                DrawHelpMarker(xor("Controls how far to aim downward to compensate for weapon recoil.\nShould be used for SMGs that shoot too high.\n\nRecommended: 5-10"));
                Space(true);

                Checkbox(xor("Show Detections"), &FSettings.Interface.m_bOverlayBoxes, FSettings.Interface.m_cDetailColor);
                DrawHelpMarker(xor("Draws lines on the overlay to visualize inference results."));
                if (FSettings.Interface.m_bHideOverlay) {
                    TextColored(NegativeColor, xor ("Warning: Hide Overlay is turned on!"));
                }
                Space();

                DrawHeader(xor("Keybinds"));
                Spacing();
                if (!FSettings.Aim.m_bAlwaysOn) {
                    Hotkey(xor ("Softaim Bind"), &FSettings.Aim.m_kbAimBind, FSettings.Interface.m_cDetailColor);
                    DrawHelpMarker(xor ("Keybind to activate the softaim.\n\nRecommended: Left Alt"));
                }
                Hotkey(xor("Menu Toggle"), &FSettings.Interface.m_kbMenuBind, FSettings.Interface.m_cDetailColor);
                DrawHelpMarker(xor("Toggles the menu on or off with a keybind"));
                if (FNDetector::Gamepad.Connected) {
                    Space(true);
                    Text(xor("Softaim Bind (Controller)"));
                    SameLine();
                    TextDisabled(xor("Left/Right Trigger"));
                    DrawHelpMarker(xor("Changing the Softaim Bind on controller is disabled."));
                    Text(xor("ADS Bind (Controller)"));
                    SameLine();
                    TextDisabled(xor("Left Trigger"));
                    DrawHelpMarker(xor("Changing the ADS Bind on controller is disabled."));
                }
                Spacing();
                TextDisabled(xor("Don't forget to change your sensitivity settings!"));
            }
        }
        const void RenderSettings() {
            DrawHeader(xor("Sensitivity Settings"));
            DrawHelpMarker(xor("Make these the same as your in-game sensitivity!"));
            Spacing();
            SliderFloat(xor("X Sensitivity"), &FSettings.Sensitivity.m_flSensitivityX, 0.1f, 100.0f, xor("%.1f"));
            SliderFloat(xor("Y Sensitivity"), &FSettings.Sensitivity.m_flSensitivityY, 0.1f, 100.0f, xor("%.1f"));
            TextDisabled(xor("Make sure you use your in-game mouse sensitivity here, even if you are playing on a\ncontroller."));

            Space(true);

            Checkbox(xor("Faster ADS Aiming"), &FSettings.Aim.m_bUseADS, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor("Uses targeting sensitivity when aiming down sights, but may cause jitter or worse aim if set up incorrectly.\n\nRecommended: On"));
            if (FSettings.Aim.m_bUseADS) {
                Spacing();
                SliderFloat(xor("Targeting"), &FSettings.Sensitivity.m_flTargetingSensitivity, 0.1f, 100.0f, xor("%.1f"));
                DrawHelpMarker(xor("This should be the same as your in-game targeting/scope sensitivity."));
            }
            else {
                TextColored(NegativeColor, xor("Warning: Mouse will move slower while aiming down sights!"));
            }
            Space();

            #ifndef DISABLE_INCREASE_FPS
            Checkbox(xor("Increase FPS"), &FSettings.Interface.m_bIncreaseFPS, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor("Disables controller input, window focus checks, and lots of other stuff in an effort to prioritize performance."));
            Spacing();
            SliderInt(xor("FPS Limit"), &FSettings.Interface.m_iFPSCap, 20, 500);
            DrawHelpMarker(xor("Forces cheat to run slower to free up system resources. This is very useful if your monitor runs at 144hz, but you don't need the cheat running at 240."));
            if (FSettings.Interface.m_bIncreaseFPS) {
                TextColored(NegativeColor, xor("Warning: Controller input is disabled!"));
                FNDetector::Gamepad.Connected = FALSE;
            }
            Space();
            #endif

            DrawHeader(xor("Accent Color"));
            Spacing();
            Checkbox(xor("Cycle RGB"), &FSettings.Interface.m_bCycleRGB, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor("Makes the UI accent color cycle RGB."));
            if (FSettings.Interface.m_bIncreaseFPS) {
                TextColored(NegativeColor, xor("Cycle RGB is disabled! (FPS Increase)"));
                FSettings.Interface.m_bCycleRGB = false;
            }
            Spacing();

            if (FSettings.Interface.m_bCycleRGB) {
                SliderFloat(xor("Cycle Speed"), &FSettings.Interface.m_flCycleRGBSpeed, 0.0f, 1.0f);
                DrawHelpMarker(xor("Sets the speed of the RGB cycle animation."));
            }
            else {
                SliderFloat(xor("Hue"), &FSettings.Interface.m_flUIHue, 0.0f, 1.0f);
            }
            SliderFloat(xor("Saturation"), &FSettings.Interface.m_flUISaturation, 0.0f, 1.0f);
            SliderFloat(xor("Brightness"), &FSettings.Interface.m_flUIBrightness, 0.0f, 1.0f);
            Space();

            DrawHeader(xor("UI Settings"));
            Spacing();
            SliderInt(xor("Rounding"), &FSettings.Interface.m_iRounding, 0, 15);
            DrawHelpMarker(xor("Rounds the corners of the interface."));
            Space(true);
            Checkbox(xor("Hide Overlay"), &FSettings.Interface.m_bHideOverlay, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor("Completely removes the cheat's screen overlay."));
            Spacing();
            Checkbox(xor("Animated Menu Background"), &FSettings.Interface.m_AnimateBackground, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor("Always shows the animated background instead of only showing it when the window isn't focused"));
            if (FSettings.Interface.m_bIncreaseFPS) {
                TextColored(NegativeColor, xor("Animated Background is disabled! (FPS Increase)"));
                FSettings.Interface.m_AnimateBackground = false;
            }
            else if (FSettings.Interface.m_bHideOverlay) {
                TextColored(NegativeColor, xor ("Warning: Hide Overlay is turned on!"));
            }
            Spacing();
            Checkbox(xor ("Show Debug Menu"), &FSettings.Interface.m_bShowDebugMenu, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor ("Shows a useful debugging menu on the left side of the screen"));
            if (FSettings.Interface.m_bHideOverlay) {
                TextColored(NegativeColor, xor ("Warning: Hide Overlay is turned on!"));
            }
            Space();

            Checkbox(xor("Show FPS"), &FSettings.Interface.m_bShowFPS, FSettings.Interface.m_cDetailColor);
            DrawHelpMarker(xor("Shows the cheat's capturing FPS in the bottom left corner"));
            if (FSettings.Interface.m_bHideOverlay) {
                TextColored(NegativeColor, xor ("Warning: Hide Overlay is turned on!"));
            }
        }
        const void RenderAbout() {

            DrawHeader(xor("About"));
            std::string Label1(m_chrAppTitle);
            std::string Label2(m_chrAppVersion);
            std::string Label3 = Label1 + xor(" Version ") + Label2;
            DrawHelpMarker(CSTR(Label3));
            Spacing();

            if (FNDetector::Gamepad.Connected) {

                DrawHeader(xor("Controller"));
                Spacing();

                if (FSettings.Interface.m_bIncreaseFPS) {
                    TextColored(NegativeColor, xor("Controller input is disabled! (FPS Increase)"));
                }
                else {

                    Text(xor("Left Joystick:"));
                    SameLine();
                    Joystick(ImVec2((FNDetector::Gamepad.LeftJoystickX + 1) / 2, (FNDetector::Gamepad.LeftJoystickY + 1) / 2 * -1), xor("##GamepadLeftJoystick"));

                    Text(xor("Right Joystick:"));
                    SameLine();
                    Joystick(ImVec2((FNDetector::Gamepad.RightJoystickX + 1) / 2, (FNDetector::Gamepad.RightJoystickY + 1) / 2 * -1), xor("##GamepadRightJoystick"));

                    TextDisabled(xor("Cheat will have less smoothing the further you hold Left Trigger."));
                }
                Space();
            }

            DrawHeader(xor("Performance"));
            Spacing();
            ImGuiIO io = GetIO();
            ImGuiStyle* style = &GetStyle();
            Text(xor("DirectX Window (%.0f FPS, %.1f ms Frame Time)"), io.Framerate, 1000 / io.Framerate);
            std::string CaptureFPS = to_string(FNDetector::CaptureFramerate);
            CaptureFPS = CaptureFPS.substr(0, CaptureFPS.find(xor("."))) + xor(" FPS (Capture), ");
            std::string InferenceFPS = to_string(FNDetector::InferenceFramerate);
            CaptureFPS += InferenceFPS.substr(0, InferenceFPS.find(xor("."))) + xor(" FPS (Inference)");
            Text(CSTR(CaptureFPS));
            TextDisabled(xor("%d Vertices, %d Indices (%d Triangles), %d Active Allocations"), io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderVertices / 3, io.MetricsActiveAllocations);
            Space();

            DrawHeader(xor ("Log"));
            Spacing();
            PopFont();
            PushFont(ConsoleFont);
            const char* Delimiter = xor ("\n");
            List<std::string> LogLines = FNUtilities::Split(FNUtilities::Log2, *Delimiter);
            for (int i = 0; i < LogLines.size(); i++) {
                ImVec4 Color = LogLines[i].starts_with(xor (" [+] >")) ? ImVec4(1.0f, 1.0f, 0.0f, 0.75f) : ImVec4(1.0f, 1.0f, 1.0f, 0.75f);
                TextColored(Color, CSTR(LogLines[i]));
            }
            PopFont();
            PushFont(DefaultFont);
        }

        // Main Drawlist
        ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;
        ImVec2 CurrentWindowPosition = ImVec2(0, 0);
        const void DoBoundsCheck() {

            // Make sure windows can't be dragged outside of the area.
            int Padding = 0;
            ImVec2 WindowPosition = CurrentWindowPosition;
            if (WindowPosition.x < 0) {
                SetNextWindowPos(ImVec2(Padding, WindowPosition.y));
                ImGui::GetIO().MouseDown[VK_LBUTTON] = false;
            }
            if (WindowPosition.y < 0) {
                SetNextWindowPos(ImVec2(WindowPosition.x, Padding));
                ImGui::GetIO().MouseDown[VK_LBUTTON] = false;
            }
            if (WindowPosition.x + MenuSize.x > SCREENX) {
                SetNextWindowPos(ImVec2(SCREENX - MenuSize.x - Padding, WindowPosition.y));
                ImGui::GetIO().MouseDown[VK_LBUTTON] = false;
            }
            if (WindowPosition.y + MenuSize.y > SCREENY) {
                SetNextWindowPos(ImVec2(WindowPosition.x, SCREENY - MenuSize.y - Padding));
                ImGui::GetIO().MouseDown[VK_LBUTTON] = false;
            }
        }
        const void RenderBaseMenu() {

            // Make sure windows aren't dragged outside of the usable area.
            DoBoundsCheck();
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImGui::GetStyle().Alpha = 0.75f;
            }

            // Set the position and size of the ImGui window to the bounds of the window
            SetNextWindowSize({ WindowWidth, WindowHeight });

            // Setup flags and begin window
            if (Begin(xor("##CheatMainWindow"), &ShowMenu, WindowFlags)) {
                if (!m_bDisableRainbowTitlebar) {
                    Decoration::DrawRainbowHeader(ImGui::GetWindowPos(), ImGui::GetWindowSize());
                }
                CurrentWindowPosition = GetWindowPos();

                // Setup "MenuSize" so that we can calculate control positions
                ImVec2 MenuSize = GetWindowSize();

                // Render Left Panel
                SetCursorPos(ImVec2(10, 32));

                ImVec4* colors = GetStyle().Colors;
                colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                if (BeginChild(xor("##LeftPanel"), ImVec2(MenuSize.x / 4 - 5, MenuSize.y - 43), TRUE)) {
                    colors[ImGuiCol_Border] = FSettings.Interface.m_cDetailColor;
                    // Color the background of the tab button brighter if it is the active tab
                    ImVec4* colors = GetStyle().Colors;
                    ImVec4 Button1 = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button2 = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button3 = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button4 = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button5 = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button6 = FSettings.Interface.m_cDetailColor;
                    Button1.w = 0.00f;
                    Button2.w = 0.00f;
                    Button3.w = 0.00f;
                    Button4.w = 0.00f;
                    Button5.w = 0.00f;
                    Button6.w = 0.00f;
                    ImVec4 Button1Hovered = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button2Hovered = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button3Hovered = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button4Hovered = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button5Hovered = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button6Hovered = FSettings.Interface.m_cDetailColor;
                    Button1Hovered.w = 0.250f;
                    Button2Hovered.w = 0.250f;
                    Button3Hovered.w = 0.250f;
                    Button4Hovered.w = 0.250f;
                    Button5Hovered.w = 0.250f;
                    Button6Hovered.w = 0.250f;
                    ImVec4 Button1Text = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button2Text = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button3Text = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button4Text = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button5Text = FSettings.Interface.m_cDetailColor;
                    ImVec4 Button6Text = FSettings.Interface.m_cDetailColor;
                    switch (CurrentTab)
                    {
                    case EMenuTabs::Softaim:
                        Button1.w = 0.25f;
                        Button1Hovered.w = 1.0f;
                        Button1Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                        break;
                    case EMenuTabs::Settings:
                        Button2.w = 0.25f;
                        Button2Hovered.w = 1.0f;
                        Button2Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                        break;
                    case EMenuTabs::Credits:
                        Button3.w = 0.25f;
                        Button3Hovered.w = 1.0f;
                        Button3Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                        break;
                    case EMenuTabs::Licensing:
                        Button4.w = 0.25f;
                        Button4Hovered.w = 1.0f;
                        Button4Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                        break;
                    case EMenuTabs::About:
                        Button5.w = 0.25f;
                        Button5Hovered.w = 1.0f;
                        Button5Text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 
                        break;
                    }
                    ImGuiStyle& Style = GetStyle();
                    constexpr int ButtonHeight = 30;
                    Style.FrameBorderSize = 1;

                    colors[ImGuiCol_ButtonHovered] = Button1Hovered;
                    colors[ImGuiCol_ButtonActive] = Button1Hovered;
                    colors[ImGuiCol_Button] = Button1;
                    colors[ImGuiCol_Text] = Button1Text;
                    if (Button(xor("Softaim"), ImVec2(MenuSize.x / 4 - 35, ButtonHeight))) {
                        CurrentTab = EMenuTabs::Softaim;
                    }

                    colors[ImGuiCol_ButtonHovered] = Button2Hovered;
                    colors[ImGuiCol_ButtonActive] = Button2Hovered;
                    colors[ImGuiCol_Button] = Button2;
                    colors[ImGuiCol_Text] = Button2Text;
                    if (Button(xor("Settings"), ImVec2(MenuSize.x / 4 - 35, ButtonHeight))) {
                        CurrentTab = EMenuTabs::Settings;
                    }
                    else {
                        colors[ImGuiCol_ButtonHovered] = Button5Hovered;
                        colors[ImGuiCol_ButtonActive] = Button5Hovered;
                        colors[ImGuiCol_Button] = Button5;
                        colors[ImGuiCol_Text] = Button5Text;
                        if (Button(xor("About"), ImVec2(MenuSize.x / 4 - 35, ButtonHeight))) {
                            CurrentTab = EMenuTabs::About;
                        }
                    }
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);
                    Space(true);

                    colors[ImGuiCol_Border] = FSettings.Interface.m_cDetailColor;
                    colors[ImGuiCol_ButtonHovered] = Button6Hovered;
                    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                    colors[ImGuiCol_Button] = Button6;
                    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    if (Button(xor("Exit"), ImVec2(MenuSize.x / 4 - 35, ButtonHeight))) {
                        ShowMenu = FALSE;
                        FNUtilities::ThreadRelease = TRUE;
                        FNUtilities::ExitProgram(EExitCodes::None);
                    }
                    Style.FrameBorderSize = 0;
                    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

                    EndChild();
                }
                const CHAR* TopLeftText = CSTR(WindowTitle);
                ImVec2 WindowPosition = GetWindowPos();
                GetForegroundDrawList()->AddText(ImVec2(WindowPosition.x + 12, WindowPosition.y + 12), ImColor(1.0, 1.0f, 1.0f, 1.0f), TopLeftText);
                //GetForegroundDrawList()->AddText(ImVec2(WindowPosition.x + 16 + CalcTextSize(TopLeftText).x, WindowPosition.y + 12), ImColor(1.0, 1.0f, 1.0f, 0.5f), m_chrAppVersion);

                // Render Right (Main) Panel
                SetCursorPos(ImVec2(MenuSize.x / 4 + 10, 32));
                colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                if (BeginChild(xor("##RightPanel"), ImVec2(MenuSize.x / 4 * 3 - 20, MenuSize.y - 43), TRUE))
                {
                    colors[ImGuiCol_Border] = FSettings.Interface.m_cDetailColor;
                    // Render Tabs
                    switch (CurrentTab)
                    {
                    case EMenuTabs::Softaim:
                        RenderSoftaim();
                        break;
                    case EMenuTabs::Settings:
                        RenderSettings();
                        break;
                    case EMenuTabs::About:
                        RenderAbout();
                        break;
                    }
                    EndChild();
                }

                // Add Vertical Separator Line
                ImVec2 C = GetWindowPos();
                const ImVec2 Point1 = ImVec2(MenuSize.x / 4 + 6.5f + C.x, 32 + C.y);
                const ImVec2 Point2 = ImVec2(MenuSize.x / 4 + 6.5f + C.x, 32 + (MenuSize.y - 43) + C.y);
                GetWindowDrawList()->AddLine(Point1, Point2, ColorConvertFloat4ToU32(FSettings.Interface.m_cDetailColor), 1.0f);

                // End Drawlist Calls
                End();
            }

            // Tick Accent Color
            bool HasInitializedColor = FALSE;
            if (!FSettings.Interface.m_bIncreaseFPS || !HasInitializedColor) {

                // RGB Cycling
                if (FSettings.Interface.m_bCycleRGB) {
                    FSettings.Interface.m_flUIHue += FSettings.Interface.m_flCycleRGBSpeed * 0.01f;
                    if (FSettings.Interface.m_flUIHue > 1.0f)
                        FSettings.Interface.m_flUIHue = 0.0f;
                }

                ColorConvertHSVtoRGB(
                    FSettings.Interface.m_flUIHue,
                    FSettings.Interface.m_flUISaturation,
                    FSettings.Interface.m_flUIBrightness,
                    FSettings.Interface.m_cDetailColor.x,
                    FSettings.Interface.m_cDetailColor.y,
                    FSettings.Interface.m_cDetailColor.z
                );

                HasInitializedColor = TRUE;
            }
            ImGui::GetStyle().Alpha = 1.0f;
        }
        bool InitializedDebugMenu = FALSE;
        const void RenderDebugMenu() {

            ImVec4* Colors = GetStyle().Colors;
            ImGuiWindowFlags WindowFlagsDebug = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
            if (FSettings.Interface.m_bShowDebugMenu) {

                if (!InitializedDebugMenu) {
                    SetNextWindowPos(ImVec2(10, SCREENY / 3));
                    SetNextWindowSize(ImVec2(300, 550));
                    InitializedDebugMenu = TRUE;
                }

                Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
                if (Begin(xor("##CheatDebugMenu")), &FSettings.Interface.m_bShowDebugMenu, WindowFlagsDebug) {
                    if (!m_bDisableRainbowTitlebar) {
                        Decoration::DrawRainbowHeader(ImGui::GetWindowPos(), ImGui::GetWindowSize());
                    }

                    Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

                    DrawHeader(CSTR(std::string(xor("Cheat (Release ") + std::string(m_chrAppVersion) + xor(")"))), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    Spacing();

                    TextDisabled(xor("GUI Render Metrics:"));
                    ImGuiIO io = GetIO();
                    Text(xor("%.0f FPS, %.1f ms Frame Time"), io.Framerate, 1000 / io.Framerate);
                    Text(xor("%d Vertices, %d Triangles"), io.MetricsRenderVertices, io.MetricsRenderVertices / 3);
                    Spacing();

                    TextDisabled(xor("Capture/Inference Metrics:"));
                    Text(xor("%.0f FPS (Inference)"), FNDetector::InferenceFramerate);
                    Text(xor("%.0f FPS (Capture)"), FNDetector::CaptureFramerate);
                    Spacing();

                    TextDisabled(xor("Gamepad:"));
                    SameLine();
                    Text(FNDetector::Gamepad.Connected ? xor("Connected") : xor("Not Connected"));
                    if (FNDetector::Gamepad.Connected) {
                        Spacing();

                        Text(xor("Left Joystick:"));
                        SameLine();
                        Joystick(ImVec2((FNDetector::Gamepad.LeftJoystickX + 1) / 2, (FNDetector::Gamepad.LeftJoystickY + 1) / 2 * -1), xor("##GamepadLeftJoystick"));

                        Text(xor("Right Joystick:"));
                        SameLine();
                        Joystick(ImVec2((FNDetector::Gamepad.RightJoystickX + 1) / 2, (FNDetector::Gamepad.RightJoystickY + 1) / 2 * -1), xor("##GamepadRightJoystick"));

                        Text(xor("Left Trigger:"));
                        SameLine();
                        Trigger(FNDetector::Gamepad.LeftTrigger, xor("##GamepadLeftTrigger"));

                        Text(xor("Right Trigger:"));
                        SameLine();
                        Trigger(FNDetector::Gamepad.RightTrigger, xor("##GamepadRightTrigger"));
                    }
                    Spacing();

                    DrawHeader(xor("Debug Metrics"), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    TextDisabled(CSTR(FNUtilities::RenderLog()));
                    Space();
                    Checkbox(xor("Show Avoidance"), &FSettings.Interface.m_bShowAvoidance, FSettings.Interface.m_cDetailColor);
                    Space(true);
                    Checkbox(xor("Show Range"), &FSettings.Interface.m_bShowDetectionRange, FSettings.Interface.m_cDetailColor);
                    End();
                }
            }
        }
    }

    // ImGui Functions
    const void SetStyle() {
        ImGuiStyle& Style = GetStyle();
        Style.WindowPadding = ImVec2(14, 14);
        Style.FramePadding = ImVec2(9, 4);
        Style.ItemSpacing = ImVec2(8, 4);
        Style.ItemInnerSpacing = ImVec2(7, 7);
        Style.TouchExtraPadding = ImVec2(0, 0);
        Style.IndentSpacing = 0.0f;
        Style.ScrollbarSize = 12.0f;
        Style.GrabMinSize = 20.0f;
        Style.WindowBorderSize = 1;
        Style.ChildBorderSize = 1;
        Style.PopupBorderSize = 1;
        Style.FrameBorderSize = 0;
        Style.TabBorderSize = 0;
        Style.WindowRounding = 0;
        Style.ChildRounding = 0;
        Style.FrameRounding = FSettings.Interface.m_iRounding;
        Style.PopupRounding = FSettings.Interface.m_iRounding;
        Style.ScrollbarRounding = FSettings.Interface.m_iRounding;
        Style.GrabRounding = FSettings.Interface.m_iRounding;
        Style.TabRounding = 0;
        Style.WindowTitleAlign = ImVec2(0.5, 0.5);
        Style.WindowMenuButtonPosition = ImGuiDir_::ImGuiDir_Down;
        Style.ColorButtonPosition = ImGuiDir_::ImGuiDir_Left;
        Style.ButtonTextAlign = ImVec2(0.5, 0.4);
        Style.SelectableTextAlign = ImVec2(0.5, 0.5);
        Style.DisplaySafeAreaPadding = ImVec2(3, 3);

        FLOAT Dark = 0.035f;
        FLOAT Light = 0.975f;

        ImVec4* colors = GetStyle().Colors;
        ImVec4 BackColor = ImVec4(Dark, Dark, Dark, 0.95f);
        ImVec4 ForeColor = ImVec4(Light, Light, Light, 1.00f);
        colors[ImGuiCol_Text] = ForeColor;
        colors[ImGuiCol_TextDisabled] = ImVec4(ForeColor.x, ForeColor.y, ForeColor.z, 0.5f);
        colors[ImGuiCol_WindowBg] = BackColor;
        colors[ImGuiCol_ChildBg] = ImVec4(BackColor.x, BackColor.y, BackColor.z, 0.00f);;
        colors[ImGuiCol_PopupBg] = ImVec4(BackColor.x, BackColor.y, BackColor.z, 1.00f);
        colors[ImGuiCol_Border] = Internal::Decoration::BorderColor;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_FrameBgActive] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_TitleBg] = BackColor;
        colors[ImGuiCol_TitleBgActive] = BackColor;
        colors[ImGuiCol_TitleBgCollapsed] = BackColor;
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_ScrollbarGrabActive] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_CheckMark] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.83f, 0.83f, 0.83f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_ButtonActive] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_Header] = ImVec4(0.09f, 0.09f, 0.09f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_HeaderActive] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_Separator] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_SeparatorHovered] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_SeparatorActive] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.98f, 0.98f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_ResizeGripActive] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 0.86f);
        colors[ImGuiCol_TabHovered] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_TabActive] = FSettings.Interface.m_cDetailColor;
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.42f, 0.33f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.44f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    // Initialization Functions
    const void Initialize() {

        ImFontConfig FontConfig;
        FontConfig.OversampleH = 1;
        FontConfig.OversampleV = 1;
        FontConfig.PixelSnapH = 1;

        static const ImWchar FontConfigRanges[] = {
            // Credit https://jrgraphix.net/research/unicode_blocks.php

            0x0020,0x007F,0x2580,0x259F,0x00A0,0x00FF,0x25A0,0x25FF,0x0100,0x017F,
            0x2600,0x26FF,0x0180,0x024F,0x2700,0x27BF,0x0250,0x02AF,0x27C0,0x27EF,
            0x02B0,0x02FF,0x27F0,0x27FF,0x0300,0x036F,0x2800,0x28FF,0x0370,0x03FF,
            0x2900,0x297F,0x0400,0x04FF,0x2980,0x29FF,0x0500,0x052F,0x2A00,0x2AFF,
            0x0530,0x058F,0x2B00,0x2BFF,0x0590,0x05FF,0x2E80,0x2EFF,0x0600,0x06FF,
            0x2F00,0x2FDF,0x0700,0x074F,0x2FF0,0x2FFF,0x0780,0x07BF,0x3000,0x303F,
            0x0900,0x097F,0x3040,0x309F,0x0980,0x09FF,0x30A0,0x30FF,0x0A00,0x0A7F,
            0x3100,0x312F,0x0A80,0x0AFF,0x3130,0x318F,0x0B00,0x0B7F,0x3190,0x319F,
            0x0B80,0x0BFF,0x31A0,0x31BF,0x0C00,0x0C7F,0x31F0,0x31FF,0x0C80,0x0CFF,
            0x3200,0x32FF,0x0D00,0x0D7F,0x3300,0x33FF,0x0D80,0x0DFF,0x3400,0x4DBF,
            0x0E00,0x0E7F,0x4DC0,0x4DFF,0x0E80,0x0EFF,0x4E00,0x9FFF,0x0F00,0x0FFF,
            0xA000,0xA48F,0x1000,0x109F,0xA490,0xA4CF,0x10A0,0x10FF,0xAC00,0xD7AF,
            0x1100,0x11FF,0xD800,0xDB7F,0x1200,0x137F,0xDB80,0xDBFF,0x13A0,0x13FF,
            0xDC00,0xDFFF,0x1400,0x167F,0xE000,0xF8FF,0x1680,0x169F,0xF900,0xFAFF,
            0x16A0,0x16FF,0xFB00,0xFB4F,0x1700,0x171F,0xFB50,0xFDFF,0x1720,0x173F,
            0xFE00,0xFE0F,0x1740,0x175F,0xFE20,0xFE2F,0x1760,0x177F,0xFE30,0xFE4F,
            0x1780,0x17FF,0xFE50,0xFE6F,0x1800,0x18AF,0xFE70,0xFEFF,0x1900,0x194F,
            0xFF00,0xFFEF,0x1950,0x197F,0xFFF0,0xFFFF,0x19E0,0x19FF,0x10000,0x1007F,
            0x1D00,0x1D7F,0x10080,0x100FF,0x1E00,0x1EFF,0x10100,0x1013F,0x1F00,0x1FFF,
            0x10300,0x1032F,0x2000,0x206F,0x10330,0x1034F,0x2070,0x209F,0x10380,0x1039F,
            0x20A0,0x20CF,0x10400,0x1044F,0x20D0,0x20FF,0x10450,0x1047F,0x2100,0x214F,
            0x10480,0x104AF,0x2150,0x218F,0x10800,0x1083F,0x2190,0x21FF,0x1D000,0x1D0FF,
            0x2200,0x22FF,0x1D100,0x1D1FF,0x2300,0x23FF,0x1D300,0x1D35F,0x2400,0x243F,
            0x1D400,0x1D7FF,0x2440,0x245F,0x20000,0x2A6DF,0x2460,0x24FF,0x2F800,0x2FA1F,
            0x2500,0x257F,0xE0000,0xE007F
        };

        ImGuiIO& io = GetIO();
        std::string ConsoleFontPath = xor("C:\\Windows\\Fonts\\") + m_chrConsoleFontTitle + xor(".ttf");
        Internal::HeadingFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(EmbeddedBinaryFontBold, m_iHeaderFontSize, &FontConfig, FontConfigRanges);
        Internal::DefaultFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(EmbeddedBinaryFontRegular, m_iNormalFontSize, &FontConfig, FontConfigRanges);
        Internal::ConsoleFont = io.Fonts->AddFontFromFileTTF(CSTR(ConsoleFontPath), m_iConsoleFontSize, &FontConfig, FontConfigRanges);

        SetNextWindowSize(ImVec2(WindowWidth, WindowHeight));

        // Start drawing the background in a seperate thread
        Internal::Decoration::InitializePoints(500);

        ShouldExit = FALSE;
    }
}