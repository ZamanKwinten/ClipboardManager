#pragma once
#include <optional>
#include <array>
#include <Windows.h>
#include "ClipboardQueue.h"
#include "ClipboardData.h"

class ClipboardManagerUI {
public:
	ClipboardManagerUI(HINSTANCE);
	~ClipboardManagerUI();

	void show();
	void hide();
	void toggle();

private:
	static constexpr WCHAR szWindowClass[] = L"ClipboardManager";  // the ClipboardManager classname 

	HINSTANCE hInstance;
	HWND mainWindow;
	HWND focusWindow;
	bool showing = false;

	std::array<HWND, 10> labels;
	ClipboardQueue queue;

	HWND createLabel(int);
	void updateLabels();
	void registerClass();
	void handleKeyPress(int);
	static LRESULT CALLBACK MESSAGEROUTER(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
	void handleClipboardUpdate(HWND);
	void pasteData(ClipboardData);
	void setClipboardData(ClipboardData);
	void sendPasteKeys();
	std::unique_ptr<ClipboardData> getClipboardData(HWND);
	
	static const int WIDTH = 450;
	static const int HEIGHT = 395;
};

