#pragma once
#include<string>

class ClipboardData {
public:
	ClipboardData(std::wstring);
	~ClipboardData();

	/*
	Get the actual data within the ClipboardData object (this is the full content that was originally on the clipboard)
	*/
	std::wstring getData();
	/*
	Get teh UI Representation of the data this is defined by the <UI_LENGTH> member 
	*/
	std::wstring getUIRepresentation();
private:
	std::wstring data;
	static constexpr std::size_t UI_LENGTH = 50;
};

