#include <iostream>
#include <fstream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include "wallpaper_data.h"

using namespace std;

string writeImageToTempFile(const string& filename) {
    string tempPath;

#if defined(_WIN32) || defined(_WIN64)
    char buffer[MAX_PATH];
    GetTempPathA(MAX_PATH, buffer);
    tempPath = string(buffer) + filename;
#else
    tempPath = "/tmp/" + filename;
#endif

    ofstream outFile(tempPath, ios::binary);
    if (!outFile) {
        cerr << "Failed to open temp file for writing\n";
        return "";
    }

    // Convert byte array into actual image
    outFile.write(reinterpret_cast<const char*>(wallpaper_jpg), wallpaper_jpg_len);
    outFile.close();

    return tempPath;
}

bool commandExistsLinux(const string& cmd) {
    string checkCmd = "command -v " + cmd + " > /dev/null 2>&1";
    
    // system needs char ptr
    int ret = system(checkCmd.c_str());
    return ret == 0;
}

int main() {
    // Write embedded image to temp file
    string imagePath = writeImageToTempFile("wallpaper.jpg");
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
        cout << "Wallpaper changed successfully!\n";
        return 0;
    } else {
        cerr << "Failed to change wallpaper.\n";
        return 1;
    }

#elif defined(__APPLE__)
    string cmd = "osascript -e 'tell application \"System Events\" to set picture of every desktop to \"" + imagePath + "\"'";
    int ret = system(cmd.c_str());
    if (ret == 0) {
        cout << "macOS wallpaper changed successfully!\n";
        return 0;
    } else {
        cerr << "Failed to change macOS wallpaper.\n";
        return 1;
    }

#elif defined(__linux__)
    string cmd;
    int ret = 1;
    if(commandExistsLinux("gsettings")){
        cmd = "gsettings set org.gnome.desktop.background picture-uri 'file://" + imagePath + "'";
        // system needs char ptr
        ret = system(cmd.c_str());
    }
    else if(commandExistsLinux("swww")){
        cmd = "swww img " + imagePath;
        // system needs char ptr
        ret = system(cmd.c_str());
    }
    else {
        cerr << "No supported wallpaper command found.\n";
        return 1;
    }

     if (ret == 0) {
        cout << "Linux wallpaper changed successfully!\n";
        return 0;
    } else {
        cerr << "Failed to change Linux wallpaper.\n";
        return 1;
    }
    

#else
    cerr << "Unsupported OS.\n";
    return 1;
#endif
}