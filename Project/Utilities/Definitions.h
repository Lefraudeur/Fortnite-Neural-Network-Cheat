#pragma once

// Expanded Type Definitions
#include "TypeDefinitions.h"

// Definitions
#define m_bDisableRainbowTitlebar TRUE
#define m_flConfidenceThreshold 0.08f
#define m_flNMSConfidenceThreshold 0.25f
#define m_flNMSThreshold 0.25f
#define m_iNetworkScale 640
#define m_iNetworkBlobScale m_iNetworkScale / 2 
#define m_iScreenDetectionSize 1080
#define m_iExitKey VK_NONAME
#define m_chrConsoleFontTitle (std::string)xor("Consola")
#define m_iNormalFontSize 17
#define m_iConsoleFontSize 14
#define m_iHeaderFontSize 19
#define m_iApplicationIconId 7
#define m_cvWhite (cv::Scalar(255, 255, 255))
#define m_cvBlack (cv::Scalar(0, 0, 0))

// Ease of use
#define Thread std::thread
#define List std::vector
#define Add push_back
#define Network cv::dnn::Net
#define NMS cv::dnn::NMSBoxes
#define BLOB cv::dnn::blobFromImage
#define THREAD_SLEEP this_thread::sleep_for
#define CUDA_BACKEND cv::dnn::DNN_BACKEND_CUDA
#define CUDA_TARGET cv::dnn::DNN_TARGET_CUDA
#define SCREENX GetSystemMetrics(SM_CXSCREEN)
#define SCREENY GetSystemMetrics(SM_CYSCREEN)
#define CENTERX (SCREENX / 2)
#define CENTERY (SCREENY / 2)
#define ThreadSleep(_ms) std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
#define RDeleteFile remove