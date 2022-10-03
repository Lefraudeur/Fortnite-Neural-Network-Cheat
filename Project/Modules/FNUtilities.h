#pragma once

// Include Base
#include "../Includes.h"

// Include Headers
#include "../Utilities/FNSettings.h"

namespace FNUtilities {

    bool ThreadRelease = FALSE;

    // Detection Structure
    struct DetectionRect{
        FLOAT x, y, w, h;
        FLOAT HeadX, HeadY;
    };

    // Get the working directory
    const std::string GetCurrentDir() {
        CHAR buffer[MAX_PATH];
        GetModuleFileName(NULL, buffer, MAX_PATH);
        std::string f = std::string(buffer);
        return f.substr(0, f.find_last_of(xor("\\/")));
    }

    // Get timestamp for Discord presence
    int GetCurrentTimestamp() {
        return CAST(int, time(0));
    }

    // Possible Logging entries
    enum class ELogFileLines {
        CaptureSize,
        InferenceSize,
        ScreenResolution,
        MouseMovements,
        Sensitivity,
        Smoothing,
        FieldOfView,
        CaptureCoords,
        CPU,
        GPU
    };

    // Log
    List<std::string> Log = {};

    // Standard Output replacement
    std::string Log2 = "";
    const void WriteCOut(std::string Out) {
        Log2 += Out;
        cout << Out;
    }

    // Function to change an element in the Log
    const void UpdateLog(ELogFileLines Line, std::string NewLog) {
        while (Log.size() < 10) {
            Log.Add(xor(""));
        }
        Log[CAST(int, Line)] = NewLog;
    }

    // Function to get the log
    std::string RenderLog() {
        std::string Out;
        for (int i = 0; i < Log.size(); i++) {
            Out += Log[i];
            if (i != Log.size() - 1) {
                Out += "\n";
            }
        }
        return Out;
    }

    // Generate random string with num chars
    std::string RandomString(int Len) {
        const CHAR* letters[26] = {
            xor("a"),xor("b"),xor("c"),xor("d"),xor("e"),xor("f"),
            xor("g"),xor("h"),xor("i"),xor("j"),xor("k"),xor("l"),
            xor("m"),xor("n"),xor("o"),xor("p"),xor("q"),xor("r"),
            xor("s"),xor("t"),xor("u"),xor("v"),xor("w"),xor("x"),
            xor("y"),xor("z")
        };
        std::string ran = std::string(xor(""));
        for (int i = 0; i < Len; i++)
            ran += letters[rand() % 26];
        return ran;
    }

    // Function to check if a window is focused, namely Fortnite
    bool IsTargetWindowFocused(HWND WindowHandle) {
        HWND Current = GetForegroundWindow();
        return (WindowHandle == Current);
    }

    // Function to check if file exists
    bool FileExists(const std::string& name) {
        struct stat BUFFER;
        return (stat(CSTR(name), &BUFFER) == 0);
    }

    List<std::string> Split(std::string& String, const char& Delimiter) {
        std::string Next;
        List<std::string> Out;

        for (std::string::const_iterator i = String.begin(); i != String.end(); i++) {
            if (*i == Delimiter) {
                if (!Next.empty()) {
                    Out.push_back(Next);
                    Next.clear();
                }
            }
            else {
                Next += *i;
            }
        }
        if (!Next.empty())
            Out.push_back(Next);
        return Out;
    }

    // Show error message and exit
    const void ExitProgram(const EExitCodes ExitCode) {
        ThreadRelease = TRUE;
        ThreadSleep(100);
        //RFunc(PostQuitMessage)(ExitCode);
        EXIT(ExitCode);
    }

    // Shows an API MessageBox and exits if needed.
    const void ShowMessage(const CHAR* Message, EExitCodes ExitCode = EExitCodes::UnknownError) {
        MessageBox(0, Message, xor("Cheat"), MB_OK);
        if (ExitCode != -1) {
            ExitProgram(ExitCode);
        }
    }

    // Remembering this off the top of my head currently
    FLOAT LinearInterpolate(FLOAT Min, FLOAT Max, FLOAT Alpha) {
        return (Max - Min) * Alpha + Min;
    }
    FLOAT ReverseLinearInterpolate(FLOAT Min, FLOAT Max, FLOAT Alpha) {
        return (Alpha - Min) / (Max - Min);
    }
    FLOAT Clamp(FLOAT Input, FLOAT Min = 0.0f, FLOAT Max = 1.0f) {
        if (Input >= Max)
            return Max;
        if (Input <= Min)
            return Min;
        return Input;
    }

    // Function to retrieve window handle
    const HWND GetTargetWindow() {
        return FindWindowA(xor("UnrealWindow"), xor("Fortnite  "));
    }

    // Function to detect whether or not the game is focused
    bool IsTargetWindowFocused() {
        return IsTargetWindowFocused(GetTargetWindow());
    }

    // Fancy UI Smoothing with Sine function
    FLOAT NormalizedSine(FLOAT X) {
        return sin((X - 0.5) * 3.14159265f) * 0.5 + 0.5; // Credit, uh, Desmos graphing calculator?
    }

    // Function to interpolate between two rectangles
    DetectionRect LinearInterpolateRect(DetectionRect R1, DetectionRect R2, FLOAT Alpha) {

        DetectionRect Current;
        Current.x = LinearInterpolate(R1.x, R2.x, Alpha);
        Current.y = LinearInterpolate(R1.y, R2.y, Alpha);
        Current.w = LinearInterpolate(R1.w, R2.w, Alpha);
        Current.h = LinearInterpolate(R1.h, R2.h, Alpha);

        return Current;
    }

    // Detect if point is inside a rectangle
    bool IsPointInsideRect(DetectionRect In, ImVec2 Point) {
        if (Point.x > In.x && Point.x < In.x + In.w) {
            if (Point.y > In.y && Point.y < In.y + In.h) {
                return TRUE;
            }
        }

        return FALSE;
    }
}