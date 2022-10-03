#pragma once

// Include Headers
#include "../Includes.h"
#include "FNUtilities.h"

namespace FNReversal {

    // Check if the cheat is already running
    const void CheckIsRunning() {
        CreateMutexA(0, FALSE, xor("Local\\$myprogram$"));
        if (GetLastError() == ERROR_ALREADY_EXISTS)
            FNUtilities::ShowMessage(xor("Cheat is already running!"), EExitCodes::MoreThanOneInstance);
    }

    // Check if the game is running
    const bool IsTargetWindowRunning() {
        if (FNUtilities::GetTargetWindow())
            return TRUE;
        return FALSE;
    }
}