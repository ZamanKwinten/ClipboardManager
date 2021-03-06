// ClipboardManager.cpp : Defines the entry point for the application.
//
#include "ClipboardManagerUI.h"
#include <Windows.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //Create the window
    ClipboardManagerUI ui(hInstance);

    //Register the paste hotkey
    RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x56);


    MSG msg = {};

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)){
        if (msg.message == WM_HOTKEY && msg.wParam == 1) {
            //When the ALT+V key combination is pressed don't send the message to the UI but instead show it.
            ui.toggle();
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    //Final message for the destroy
    DispatchMessage(&msg);

    return (int) msg.wParam;
}
