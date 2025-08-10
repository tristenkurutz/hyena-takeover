#include <iostream>
#include <fstream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include "wallpaper_data.h"

std::string writeImageToTempFile(const std::string& filename) {
    std::string tempPath;

#if defined(_WIN32) || defined(_WIN64)
    char buffer[MAX_PATH];
    GetTempPathA(MAX_PATH, buffer);
    tempPath = std::string(buffer) + filename;
#else
    tempPath = "/tmp/" + filename;
#endif

    std::ofstream outFile(tempPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open temp file for writing\n";
        return "";
    }

    // Convert byte array into actual image
    outFile.write(reinterpret_cast<const char*>(wallpaper_jpg), wallpaper_jpg_len);
    outFile.close();

    return tempPath;
}

int main() {
    // Write embedded image to temp file
    std::string imagePath = writeImageToTempFile("wallpaper.jpg");
    if (imagePath.empty()) {
        return 1;
    }

#if defined(_WIN32) || defined(_WIN64)
    BOOL result = SystemParametersInfoA(
        SPI_SETDESKWALLPAPER,
        0,
        (PVOID)imagePath.c_str(),
        SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE
    );

    if (result) {
        std::cout << "Wallpaper changed successfully!\n";
        return 0;
    } else {
        std::cerr << "Failed to change wallpaper.\n";
        return 1;
    }

#elif defined(__APPLE__)
    std::string cmd = "osascript -e 'tell application \"System Events\" to set picture of every desktop to \"" + imagePath + "\"'";
    int ret = system(cmd.c_str());
    if (ret == 0) {
        std::cout << "macOS wallpaper changed successfully!\n";
        return 0;
    } else {
        std::cerr << "Failed to change macOS wallpaper.\n";
        return 1;
    }

#elif defined(__linux__)
    std::string cmd = "gsettings set org.gnome.desktop.background picture-uri 'file://" + imagePath + "'";
    int ret = system(cmd.c_str());
    if (ret == 0) {
        std::cout << "Linux wallpaper changed successfully!\n";
        return 0;
    } else {
        std::cerr << "Failed to change Linux wallpaper.\n";
        return 1;
    }

#else
    std::cerr << "Unsupported OS.\n";
    return 1;
#endif
}