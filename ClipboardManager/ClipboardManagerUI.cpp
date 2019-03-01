#include "ClipboardManagerUI.h"
#include <stdlib.h>
#include <string>


ClipboardManagerUI::ClipboardManagerUI(HINSTANCE hInstance) {
	this->hInstance = hInstance;
	registerClass();
	this->mainWindow = CreateWindowEx(0, szWindowClass, L"Clipboard Contents", WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, NULL, NULL, hInstance, this);
	for (int i = 0; i < labels.size(); i++) {
		labels[i] = createLabel(i);
	}
}


ClipboardManagerUI::~ClipboardManagerUI() {
}

HWND ClipboardManagerUI::createLabel(int labelIndex) {
	
	//Add some random label
	CreateWindowEx(0, L"STATIC", (std::to_wstring(labelIndex)+L":").c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT | BS_CENTER, 5, 5 + (35 * labelIndex), 15, 30, this->mainWindow, 0, 0, 0);
	return CreateWindowEx(0, L"STATIC", L"" , WS_CHILD | WS_VISIBLE | SS_LEFT | BS_CENTER, 20, 5+(35*labelIndex), 410, 30, this->mainWindow, 0, 0, 0);
}

void ClipboardManagerUI::registerClass() {
	WNDCLASS wc = {};
	wc.lpfnWndProc = MESSAGEROUTER;
	wc.hInstance = this->hInstance;
	wc.lpszClassName = szWindowClass;
	RegisterClass(&wc);
}

LRESULT CALLBACK ClipboardManagerUI::MESSAGEROUTER(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	ClipboardManagerUI* app;
	if (msg == WM_CREATE) {
		app = (ClipboardManagerUI*) (((LPCREATESTRUCT)lParam) -> lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)app);

	} else {
		app = (ClipboardManagerUI*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}
	return app->wndProc(hWnd, msg, wParam, lParam);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK ClipboardManagerUI::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		AddClipboardFormatListener(hWnd);
		break;
	case WM_CLIPBOARDUPDATE:
		handleClipboardUpdate(hWnd);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		//At this point we will not run into the 32 bit vs 64 bit miserie
		handleKeyPress(static_cast<int>(wParam));
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
// FUNCTION: handleClipboardUpdate
//
// PURPOSE: Handle a clipboard update, this means pulling the new data from the clipboard and storing it in the datastructure, which schedules a UI update
//
void ClipboardManagerUI::handleClipboardUpdate(HWND hwnd) {
	//pull the latest string from the clipboard
	auto clipboardData = getClipboardData(hwnd);
	if (clipboardData.get() != nullptr) {
		//the clipboardData is actually pointing to something => update the queue
		queue.add(std::move(clipboardData));
	}
	updateLabels();
}

std::unique_ptr<ClipboardData> ClipboardManagerUI::getClipboardData(HWND hwnd) {
	OpenClipboard(hwnd);
	std::unique_ptr<ClipboardData> clipboardData;
	if (EnumClipboardFormats(CF_UNICODETEXT)) {
		auto hData = GetClipboardData(CF_UNICODETEXT);
		WCHAR* data = static_cast<WCHAR*>(GlobalLock(hData));
		clipboardData = std::move(std::unique_ptr<ClipboardData>(new ClipboardData(std::wstring(data))));
		GlobalUnlock(hData);
	}

	CloseClipboard();
	return clipboardData;
}

void ClipboardManagerUI::handleKeyPress(int code) {
	int actual = -1;

	int key = code - VK_NUMPAD0;
	if (key >= 0 && key < 10) {
		//The key is a numpad key in [0,9] => we must handle it
		actual = key;
	} else {
		//make a second check for normal number keys
		key = code - 0x30;
		if (key >= 0 && key < 10) {
			actual = key;
		}
	}


	if (actual != -1) {
		pasteData(queue[actual]);
		hide();
	}
}
void ClipboardManagerUI::pasteData(ClipboardData data) {
	//First focus the window again that we originally started from
	SetForegroundWindow(focusWindow);
	//Update the content of the clipboard 
	setClipboardData(data);
	//Actually paste the content of the clipboard
	sendPasteKeys();
}

void ClipboardManagerUI::setClipboardData(ClipboardData data) {
	std::wstring textToClipboard = data.getData();
	RemoveClipboardFormatListener(mainWindow);
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		size_t size = (textToClipboard.size() + 1) * sizeof(WCHAR);
		HGLOBAL hClipboardData = GlobalAlloc(NULL, size);
		if (hClipboardData) {
			WCHAR* pchData = (WCHAR*)GlobalLock(hClipboardData);
			if (pchData) {
				memcpy(pchData, (WCHAR*)textToClipboard.c_str(), size);
				GlobalUnlock(hClipboardData);
				SetClipboardData(CF_UNICODETEXT, hClipboardData);
			}
		}
		CloseClipboard();
	}
	AddClipboardFormatListener(mainWindow);
}

void ClipboardManagerUI::sendPasteKeys() {
	//Code responsible of sending a ctrl+V key combination
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Press the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));

	// Release the "V" key
	ip.ki.wVk = 'V';
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	// Release the "Ctrl" key
	ip.ki.wVk = VK_CONTROL;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

void ClipboardManagerUI::updateLabels() {
	for (int i = 0; i < labels.size(); i++) {
		SetWindowText(labels[i], queue[i].getUIRepresentation().c_str());
	}
}

void ClipboardManagerUI::show() {
	updateLabels();
	focusWindow = GetForegroundWindow();
	SetForegroundWindow(mainWindow);
	POINT p;
	if (GetCursorPos(&p)) {
		MoveWindow(mainWindow, p.x, p.y, WIDTH, HEIGHT, false);
	}

	ShowWindow(this->mainWindow, SW_SHOW);
	showing = true;
}


void ClipboardManagerUI::hide() {
	ShowWindow(this->mainWindow, SW_HIDE);
	showing = false;
}

void ClipboardManagerUI::toggle() {
	if (showing) {
		hide();
	} else {
		show();
	}
}