#include "ClipboardData.h"



ClipboardData::ClipboardData(std::wstring data) {
	this->data = data;
}


ClipboardData::~ClipboardData() = default;

std::wstring ClipboardData::getData() {
	return this->data;
}

std::wstring ClipboardData::getUIRepresentation() {
	return this->data.substr(0, UI_LENGTH);
}