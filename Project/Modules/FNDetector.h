#pragma once

// Include Headers
#include <Xinput.h>
#include "FNUtilities.h"
#include "../Utilities/Definitions.h"
#include "../Utilities/FNSettings.h"
#include "../Utilities/Includes/IncludeOpenCV.h"
#include "../Modules/FNInterface.h"

// Anti-heuristic Detectable Mouse Movement
#include "Mouse/RunMouse.h"
RunMouse* Mouse = new RunMouse();

#define bool bool

// Commandline Interface
namespace CLI{
    const void WriteMessage(std::string Msg, DWORD Color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, bool ShowTag = TRUE) {
        HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(ConsoleHandle, Color);
        if (Msg.starts_with(xor (">"))) {
            FNUtilities::WriteCOut(ShowTag ? xor (" [+] ") : xor(" "));
            SetConsoleTextAttribute(ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN);
            FNUtilities::WriteCOut(Msg);
            SetConsoleTextAttribute(ConsoleHandle, Color);
        }
        else if (Msg != xor (""))
            FNUtilities::WriteCOut((ShowTag ? xor (" [+] ") : xor (" ")) + Msg);
    }
}

namespace FNDetector{

    bool ShowUsingAimbot = FALSE;

    // Capture & inference Framerates
    FLOAT CaptureFramerate;
    FLOAT InferenceFramerate;
    FLOAT ADSAmount;

    namespace Drawing {

        const void DrawRect(ImVec2 Start, ImVec2 End, ImColor Col) {
            ImDrawList* DrawList = ImGui::GetBackgroundDrawList();
            ImColor Black = ImColor(0.0f, 0.0f, 0.0f, 0.75f);
            DrawList->AddRect(Start, End, Black, 0.0f, 0.0f, 2.0f);
            DrawList->AddRect(Start, End, Col, 0.0f, 0.0f, 1.0f);
        }
    }

    // Network
    Network FNetwork;
    bool HasInitialized = false;

    // Screencapture Variables
    HDC DefaultWindowDC;
    HDC ScreenObject;
    HBITMAP ScreenBitmap;
    BITMAPINFO ScreenBitmapInfo;
    int DetectionLeft;
    int DetectionTop;
    int DetectionWidth;
    int DetectionHeight;
    CHAR* ScreenData;
    Mat* Screen;
    HGDIOBJ ScreenGDI;

    List<FNUtilities::DetectionRect> RectOverlayDrawList;

    // Structure to store gamepad state
    struct GamepadState {
        FLOAT LeftTrigger = 0;
        FLOAT RightTrigger = 0;
        FLOAT LeftJoystickX = 0;
        FLOAT LeftJoystickY = 0;
        FLOAT RightJoystickX = 0;
        FLOAT RightJoystickY = 0;

        bool DPadUp = FALSE;
        bool DPadDown = FALSE;
        bool DPadLeft = FALSE;
        bool DPadRight = FALSE;
        bool Start = FALSE;
        bool Back = FALSE;
        bool LeftJoystickDown = FALSE;
        bool RightJoystickDown = FALSE;
        bool A = FALSE;
        bool B = FALSE;
        bool X = FALSE;
        bool Y = FALSE;

        bool Connected = FALSE;
    };

    // Gamepad State
    GamepadState Gamepad;

    // Function to find the first connected controller
    int FindControllerIndex() {

        // Find connected controller
        int ControllerId = -1;
        for (int i = 0; i < XUSER_MAX_COUNT && ControllerId == -1; i++) {

            // Check if controller state exists at that address
            XINPUT_STATE CurrState;
            memset(&CurrState, 0, sizeof(XINPUT_STATE));
            if (XInputGetState(i, &CurrState) == ERROR_SUCCESS) {
                ControllerId = i;
            }
        }
        if (ControllerId == -1) {
            return -32;
        }
        return ControllerId;
    }

    // Function to read the current state of the controller
    GamepadState GetGamepadState() {

        // Find a gamepad
        int GamepadId = FindControllerIndex();
        if (GamepadId == -32) { // No controller detected
            GamepadState C;
            C.Connected = FALSE;
            return C;
        }

        // Read state of gamepad
        XINPUT_STATE InputState;
        XInputGetState(0, &InputState);
        XINPUT_GAMEPAD GPState = InputState.Gamepad;

        // Create and return output
        GamepadState Output;
        Output.LeftTrigger = CAST(FLOAT, GPState.bLeftTrigger);
        Output.RightTrigger = CAST(FLOAT, GPState.bRightTrigger);
        Output.LeftJoystickX = CAST(FLOAT, GPState.sThumbLX / 255);
        Output.LeftJoystickY = CAST(FLOAT, GPState.sThumbLY / 255);
        Output.RightJoystickX = CAST(FLOAT, GPState.sThumbRX / 255);
        Output.RightJoystickY = CAST(FLOAT, GPState.sThumbRY / 255);
        Output.DPadUp = CAST(bool, GPState.wButtons & 0x0001);
        Output.DPadDown = CAST(bool, GPState.wButtons & 0x0002);
        Output.DPadLeft = CAST(bool, GPState.wButtons & 0x0004);
        Output.DPadRight = CAST(bool, GPState.wButtons & 0x0008);
        Output.Start = CAST(bool, GPState.wButtons & 0x0010);
        Output.Back = CAST(bool, GPState.wButtons & 0x0020);
        Output.LeftJoystickDown = CAST(bool, GPState.wButtons & 0x0040);
        Output.RightJoystickDown = CAST(bool, GPState.wButtons & 0x0080);
        Output.A = CAST(bool, GPState.wButtons & 0x1000);
        Output.B = CAST(bool, GPState.wButtons & 0x2000);
        Output.X = CAST(bool, GPState.wButtons & 0x4000);
        Output.Y = CAST(bool, GPState.wButtons & 0x8000);

        Output.LeftJoystickX /= 128;
        Output.LeftJoystickY /= 128;
        Output.RightJoystickX /= 128;
        Output.RightJoystickY /= 128;
        Output.LeftTrigger /= 255;
        Output.RightTrigger /= 255;

        Output.Connected = TRUE;

        return Output;
    }

    ImVec2 MoveAmount = ImVec2(0.0f, 0.0f);
    void MoveMouseThread() {

        int MouseDelay = 1;

        while (!FNUtilities::ThreadRelease) {

            // Official SendInput Moment
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_MOVE;

            // Sending blank mouse inputs is my game man
            bool Focused = GetForegroundWindow() == FNUtilities::GetTargetWindow();
            input.mi.dx = Focused ? MoveAmount.x : 0.0f;
            input.mi.dy = Focused ? MoveAmount.y : 0.0f;

            // Official SendInput Moment 2!
            if (FSettings.Aim.m_bAlwaysOn) {
                SendInput(1, &input, sizeof(input));
            }
            else {
                if (GetAsyncKeyState(FSettings.Aim.m_kbAimBind) || Gamepad.LeftTrigger > 0.25f || Gamepad.RightTrigger > 0.25f) {
                    SendInput(1, &input, sizeof(input));
                }
            }

            MoveAmount = ImVec2(0.0f, 0.0f);

            ThreadSleep(MouseDelay);
        }
    }

    // Function to move the mouse to a specific player
    const void MoveMouse(FNUtilities::DetectionRect TargetPlayer) {

        FLOAT GlobalDivisor = 20;
        int AutoShootThreshold = 10;
        int MaxSmoothedFrames = 2;
        FLOAT HiddenMinSmoothing = 0.2f;

        FLOAT RelativeMovementX = TargetPlayer.HeadX - CENTERX;
        FLOAT RelativeMovementY = TargetPlayer.HeadY - CENTERY;
        FLOAT RNG = CAST(FLOAT, CAST(FLOAT, rand()) / RAND_MAX) * 1;
        if (!GetAsyncKeyState(FSettings.Aim.m_kbADSBind) && Gamepad.LeftTrigger < 0.25f) {
            if (!FSettings.Aim.m_bHardSmoothed) {
                RelativeMovementX /= (((FSettings.Aim.m_flSmoothing + HiddenMinSmoothing + RNG) * MaxSmoothedFrames) + 1);
                RelativeMovementY /= (((FSettings.Aim.m_flSmoothing + HiddenMinSmoothing + RNG) * MaxSmoothedFrames) + 1);
            }
        }
        else {
            if (Gamepad.Connected) {
                float NewADSSmoothing = FNUtilities::LinearInterpolate(1.0f, 0.25f, Gamepad.LeftTrigger);
                if (!FSettings.Aim.m_bAdaptiveSmoothing) {
                    NewADSSmoothing = FSettings.Aim.m_flADSSmoothing;
                }
                RelativeMovementX /= (((NewADSSmoothing + HiddenMinSmoothing) * MaxSmoothedFrames) + 1);
                RelativeMovementY /= (((NewADSSmoothing + HiddenMinSmoothing) * MaxSmoothedFrames) + 1);
            }
            else {
                if (!FSettings.Aim.m_bHardSmoothed) {
                    RelativeMovementX /= (((FSettings.Aim.m_flADSSmoothing + HiddenMinSmoothing + RNG) * MaxSmoothedFrames) + 1);
                    RelativeMovementY /= (((FSettings.Aim.m_flADSSmoothing + HiddenMinSmoothing + RNG) * MaxSmoothedFrames) + 1);
                }
            }
        }

        FLOAT MovementMultiplierX = FSettings.Sensitivity.m_flSensitivityX / GlobalDivisor;
        FLOAT MovementMultiplierY = FSettings.Sensitivity.m_flSensitivityY / GlobalDivisor;
        if (FSettings.Aim.m_bUseADS) {
            if ((GetAsyncKeyState(FSettings.Aim.m_kbADSBind)) || Gamepad.LeftTrigger > 0.25f) {
                MovementMultiplierX *= 100 / FSettings.Sensitivity.m_flTargetingSensitivity;
                MovementMultiplierY *= 100 / FSettings.Sensitivity.m_flTargetingSensitivity;
            }
        }
        RelativeMovementX *= MovementMultiplierX;
        RelativeMovementY *= MovementMultiplierY;

        RelativeMovementY += FSettings.Aim.m_iAntiRecoilPixels;

        MoveAmount.x = RelativeMovementX;
        MoveAmount.y = RelativeMovementY;

        FNUtilities::UpdateLog(FNUtilities::ELogFileLines::MouseMovements, CSTR(std::string(
            std::string(xor("MoveTo: [")) +
            std::string(to_string(RelativeMovementX)) +
            std::string(xor(",")) +
            std::string(to_string(RelativeMovementY)) +
            std::string(xor("]"))
        )));
    }

    // Get output names of the neural network.
    List<cv::String> GetNetworkOutputNames(const Network& NeuralNetwork) {

        // Create a vector to hold all the names.
        static List<cv::String> AllNames;

        List<int> OutLayers = NeuralNetwork.getUnconnectedOutLayers();
        List<cv::String> LayerNames = NeuralNetwork.getLayerNames();
        AllNames.resize(OutLayers.size());
        for (size_t i = 0; i < OutLayers.size(); i++) {
            AllNames[i] = LayerNames[OutLayers[i] - 1];
        }
        return AllNames;
    }

    FNUtilities::DetectionRect Player;
    FNUtilities::DetectionRect PlayerADS;

    // Function to be run after the neural network finishes processing.
    const void ParseNetworkOutput(Mat& CurrentFrame, const List<Mat>& Outs) {
        Player.x = 0.343f * SCREENX;
        Player.y = 0.388f * SCREENY;
        Player.w = 0.140 * SCREENX;
        Player.h = 0.601f * SCREENY;
        PlayerADS.x = 0.05f * SCREENX;
        PlayerADS.y = 0.178f * SCREENY;
        PlayerADS.w = 0.385f * SCREENX;
        PlayerADS.h = 0.826f * SCREENY;
        FNUtilities::DetectionRect PlayerCompensation = FNUtilities::LinearInterpolateRect(Player, PlayerADS, FNUtilities::NormalizedSine(FNDetector::ADSAmount));

        ShowUsingAimbot = FALSE;
        if (FSettings.Aim.m_bAlwaysOn) {
            ShowUsingAimbot = TRUE;
        }
        else {
            if (GetAsyncKeyState(FSettings.Aim.m_kbAimBind) || Gamepad.LeftTrigger > 0.25f || Gamepad.RightTrigger > 0.25f) {
                ShowUsingAimbot = TRUE;
            }
        }

        // Create vectors for the outputs of this function
        List<float> DetectionConfidences;
        List<cv::Rect> DetectionBoxes;

        int DetectionOverallSize = m_iNetworkBlobScale;

        // Parse the network output
        for (size_t i = 0; i < Outs.size(); ++i) {
            float* Data = (float*)Outs[i].data;
            for (int j = 0; j < Outs[i].rows; j++, Data += Outs[i].cols) {
                Mat ClassScores = Outs[i].row(j).colRange(5, Outs[i].cols);
                cv::Point ClassIdPoint;

                // Get Detection Confidence
                double Confidence;
                cv::minMaxLoc(ClassScores, 0, &Confidence, 0, &ClassIdPoint);

                // Check if confidence is above a threshold
                if (Confidence > m_flConfidenceThreshold) {

                    // Parsing Object Bounds (say "cool!" in your head for maximum effect)
                    int CenterX = (int)(Data[0] * CurrentFrame.cols);
                    int CenterY = (int)(Data[1] * CurrentFrame.rows);
                    int BoxWidth = (int)(Data[2] * CurrentFrame.cols);
                    int BoxHeight = (int)(Data[3] * CurrentFrame.rows);
                    int BoxLeft = CenterX - BoxWidth / 2;
                    int BoxTop = CenterY - BoxHeight / 2;

                    // Add those values to the arrays
                    DetectionConfidences.Add(CAST(FLOAT, Confidence));
                    DetectionBoxes.Add(cv::Rect(BoxLeft, BoxTop, BoxWidth, BoxHeight));
                }
            }
        }

        // Perform non-maximum supression algorithm to remove duplicate detections
        List<int> AllIndices;
        NMS(DetectionBoxes, DetectionConfidences, m_flNMSConfidenceThreshold, m_flNMSThreshold, AllIndices);

        List<FNUtilities::DetectionRect> Detections;
        for (size_t i = 0; i < AllIndices.size(); ++i) {
            int Index = AllIndices[i];
            cv::Rect CurrDetectionBox = DetectionBoxes[Index];
            FNUtilities::DetectionRect CurrDetection;
            CurrDetection.x = CurrDetectionBox.x + (CENTERX - DetectionOverallSize / 2);
            CurrDetection.y = CurrDetectionBox.y + (CENTERY - DetectionOverallSize / 2);
            CurrDetection.w = CurrDetectionBox.width;
            CurrDetection.h = CurrDetectionBox.height;

            // Head/Chest/Feet aiming
            FLOAT BoxHeightDivisor = 0.25f;
            switch (FSettings.Aim.m_eAimTargetBone) {
            case EAimTargetBones::Head:
                BoxHeightDivisor = 0.15f;
                break;
            case EAimTargetBones::Chest:
                BoxHeightDivisor = 0.4f;
                break;
            case EAimTargetBones::Feet:
                BoxHeightDivisor = 0.8f;
                break;
            }

            CurrDetection.HeadX = CurrDetectionBox.x + CurrDetectionBox.width / 2;
            CurrDetection.HeadY = CurrDetectionBox.y + CurrDetectionBox.height * BoxHeightDivisor;
            CurrDetection.HeadX += CENTERX - DetectionOverallSize / 2;
            CurrDetection.HeadY += CENTERY - DetectionOverallSize / 2;

            if (!FNUtilities::IsPointInsideRect(PlayerCompensation, ImVec2(CurrDetection.HeadX, CurrDetection.HeadY))) {
                Detections.Add(CurrDetection);
                RectOverlayDrawList.Add(CurrDetection);
            }
        }

        // Set a target player from the detections
        int BaseWidth = SCREENX;
        int BaseHeight = SCREENY;
        FNUtilities::DetectionRect TargetPlayer;
        int ClosestDist = BaseWidth;
        for (int i = 0; i < Detections.size(); i++) {

            // Get current iterating detection
            FNUtilities::DetectionRect CurrDetection = Detections[i];

            // Calculate Distance from Crosshair
            int DistX = (BaseWidth / 2) - CurrDetection.HeadX;
            int DistY = (BaseHeight / 2) - CurrDetection.HeadY;
            int CrosshairDist = sqrt(pow(DistX, 2) + pow(DistY, 2));

            // See if that distance is closer than the currently targeted player
            if (CrosshairDist < ClosestDist) {
                TargetPlayer = CurrDetection;
                ClosestDist = CrosshairDist;
            }
        }

        // Create UserSettings
        // Check if the aim key is pressed before moving the mouse
        if (FSettings.Aim.m_bEnabled && ClosestDist != BaseWidth) {

            // Check if player is inside FOV
            int DistX = (BaseWidth / 2) - TargetPlayer.HeadX;
            int DistY = (BaseHeight / 2) - TargetPlayer.HeadY;
            int CrosshairDist = sqrt(pow(DistX, 2) + pow(DistY, 2));
            int AimFOV = FNUtilities::LinearInterpolate(FSettings.Aim.m_iFOV, FSettings.Aim.m_iADSFOV, FNUtilities::NormalizedSine(FNDetector::ADSAmount));
            if (CrosshairDist < AimFOV / 2) {

                // Perform Mouse Movement
                MoveMouse(TargetPlayer);
            }
        }
    }

    const void CreateBlob(Mat& InputFrame) {
        Mat BlobData;
        BLOB(InputFrame, BlobData, 1 / 255.0, cv::Size(m_iNetworkBlobScale, m_iNetworkBlobScale), m_cvBlack, TRUE, FALSE);
        FNetwork.setInput(BlobData);
    }

    // Main function to detect players in a frame
    const void Infer(Mat& InputFrame) {

        // Create Blob
        //BLOB(InputFrame, BlobData, 1 / 255.0, cv::Size(m_iNetworkBlobScale, m_iNetworkBlobScale), m_cvBlack, TRUE, FALSE);
        //FNetwork.setInput(BlobData);
        List<Mat> NetworkOuts;
        FNetwork.forward(NetworkOuts, GetNetworkOutputNames(FNetwork));
        ParseNetworkOutput(InputFrame, NetworkOuts);
    }

    // Initializes Screenshotter
    const void CreateScreenshotter() {
        SelectObject(ScreenObject, ScreenGDI);
        DeleteObject(ScreenBitmap);
        DeleteDC(ScreenObject);
        delete Screen;
        delete[] ScreenData;

        // Get HWND and DC
        HWND hwnd = GetDesktopWindow();
        DefaultWindowDC = GetWindowDC(hwnd);

        // Create detection bounds
        int DetectionAreaSize = m_iNetworkBlobScale;
        int x2 = CAST(int, CENTERX + DetectionAreaSize / 2);
        int y2 = CAST(int, CENTERY + DetectionAreaSize / 2);
        DetectionLeft = CAST(int, CENTERX - DetectionAreaSize / 2);
        DetectionTop = CAST(int, CENTERY - DetectionAreaSize / 2);
        DetectionWidth = x2 - DetectionLeft;
        DetectionHeight = y2 - DetectionTop;

        FNUtilities::UpdateLog(FNUtilities::ELogFileLines::CaptureCoords, CSTR(std::string(
            std::string(xor("Coord: [")) +
            std::string(to_string(DetectionLeft)) +
            std::string(xor(",")) +
            std::string(to_string(DetectionTop)) +
            std::string(xor("]"))
        )));

        // Create compatible device contexts
        ScreenObject = CreateCompatibleDC(DefaultWindowDC);
        ScreenBitmap = CreateCompatibleBitmap(DefaultWindowDC, DetectionWidth, DetectionHeight);
        ScreenGDI = SelectObject(ScreenObject, ScreenBitmap);

        // Set bitmapinfo parameters
        ScreenBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        ScreenBitmapInfo.bmiHeader.biWidth = DetectionWidth;
        ScreenBitmapInfo.bmiHeader.biHeight = -DetectionHeight;
        ScreenBitmapInfo.bmiHeader.biPlanes = 1;
        ScreenBitmapInfo.bmiHeader.biBitCount = 24;
        ScreenBitmapInfo.bmiHeader.biCompression = BI_RGB;

        int step = CAST(int, ceil(DetectionWidth * 3 / CAST(double, 4))) * 4;
        ScreenData = new CHAR[step * DetectionHeight];
        Screen = new Mat(DetectionHeight, DetectionWidth, CV_8UC3, ScreenData, step);
    }

    // Function to take a screenshot into a MAT object
    Mat PullScreenshot() {
        BitBlt(ScreenObject, 0, 0, DetectionWidth, DetectionHeight, DefaultWindowDC, DetectionLeft, DetectionTop, SRCCOPY);
        GetDIBits(ScreenObject, ScreenBitmap, 0, DetectionHeight, ScreenData, &ScreenBitmapInfo, DIB_RGB_COLORS);
        return *Screen;
    };

    // Creates/Initializes Network and Screenshotter
    const void InitializeDetector(string ConfigPath, string WeightsPath) {

        // Make sure required files exist
        if (!FNUtilities::FileExists(ConfigPath) || !FNUtilities::FileExists(WeightsPath))
            FNUtilities::ShowMessage(xor("Error: Network failed to initialize!"), EExitCodes::NetworkLoadingFailed);

        // Initialize Network
        CLI::WriteMessage(xor ("> Initializing Network...\n"));
        FNetwork = cv::dnn::readNetFromDarknet(ConfigPath, WeightsPath);

        CLI::WriteMessage(xor ("> Initializing Inference Backend/Target...\n"));

        // Check if CUDA-enabled processor is available
        if (cv::cuda::getCudaEnabledDeviceCount() > 0) {
            CLI::WriteMessage(xor("> Compatible CUDA-enabled device found!\n"));
            CLI::WriteMessage(xor("> Switching Inference Backend/Target...\n"));
            FNetwork.setPreferableBackend(CUDA_BACKEND);
            FNetwork.setPreferableTarget(CUDA_TARGET);
        }
        else {
            CLI::WriteMessage(xor ("> No compatible CUDA-enabled device found!\n"));
            CLI::WriteMessage(xor ("> Switching Inference Backend/Target to CPU...\n"));
            FNetwork.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            FNetwork.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }
        
        // Use OpenCL if available
        if (cv::ocl::haveOpenCL()) {
            cv::ocl::setUseOpenCL(true);
            cv::ocl::useOpenCL();
        }
    }
    const void Initialize() {
        std::string ConfigFilename = xor("Network.cfg");
        std::string WeightsFilename = xor("Network.weights");
        if (!(FNUtilities::FileExists(ConfigFilename) && FNUtilities::FileExists(WeightsFilename))) {
            CLI::WriteMessage(xor("Files not found: Network.cfg OR Network.weights\n"));
            FNUtilities::ExitProgram(EExitCodes::NetworkLoadingFailed);
        }
        CLI::WriteMessage("");
        CreateScreenshotter();
        InitializeDetector(ConfigFilename, WeightsFilename);

        HasInitialized = TRUE;
    }
}