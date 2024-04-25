#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <wchar.h> // Include wchar.h for swprintf_s

using namespace std;

// Global variables
const wchar_t* songs[] = {
    L"Song 1",
    L"Song 2",
    L"Song 3"
};

const wchar_t* authors[] = {
    L"Author 1",
    L"Author 2",
    L"Author 3"
};

const int numSongs = sizeof(songs) / sizeof(songs[0]);
int currentSongIndex = 0;
bool isPlaying = false;
vector<int> recentlyPlayed; // List of recently played songs
wchar_t lyricsFileName[100]; // File name to read lyrics
wstring lyrics; // Variable to hold the lyrics

// Custom messages
#define WM_UPDATE_LYRICS    (WM_USER + 1)
#define WM_UPDATE_RECENT    (WM_USER + 2)

// Function prototypes
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateRecentlyPlayed(int songIndex);
void LoadLyrics(int songIndex);

// Entry point for Windows GUI applications
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"MusicPlayerClass";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window
    HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"MusicPlayerClass", L"Music Player", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Show the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // Draw text or any other UI elements here
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_UPDATE_LYRICS:
        // Display lyrics
        SetWindowText(hWnd, lyrics.c_str());
        break;
    case WM_UPDATE_RECENT:
    {
        // Display recently played songs
        wstring recentSongs;
        for (int i = 0; i < recentlyPlayed.size(); ++i) {
            recentSongs += songs[recentlyPlayed[i]];
            recentSongs += L"\n";
        }
        SetWindowText(hWnd, recentSongs.c_str());
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Function to update the list of recently played songs
void UpdateRecentlyPlayed(int songIndex) {
    // Add the current song index to the list of recently played songs
    recentlyPlayed.push_back(songIndex);

    // Keep the size of the recently played list to a maximum of 5 songs
    if (recentlyPlayed.size() > 5) {
        recentlyPlayed.erase(recentlyPlayed.begin());
    }

    // Send message to update display
    SendMessage(GetActiveWindow(), WM_UPDATE_RECENT, 0, 0);
}

// Function to load lyrics for the current song
void LoadLyrics(int songIndex) {
    // Generate the filename for the lyrics file
    swprintf_s(lyricsFileName, L"%ls.txt", songs[songIndex]);

    // Read the lyrics from the file
    wifstream lyricsFile(lyricsFileName);
    if (lyricsFile.is_open()) {
        wstring line;
        lyrics.clear();
        while (getline(lyricsFile, line)) {
            lyrics += line;
            lyrics += L"\n";
        }
        lyricsFile.close();
    }
    else {
        lyrics = L"Lyrics not available.";
    }

    // Send message to update display
    SendMessage(GetActiveWindow(), WM_UPDATE_LYRICS, 0, 0);
}