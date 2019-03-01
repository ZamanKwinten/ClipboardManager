#pragma once
#include "ClipboardData.h"
#include <array>
/*
This class is used as a datastructure for the clipboard, we want to ensure that there are exactly 10 items in the queue at all times
*/
class ClipboardQueue {
public:
	ClipboardQueue();
	~ClipboardQueue();

	void add(std::unique_ptr<ClipboardData>);

	ClipboardData operator[](int);

private:
	std::array<std::unique_ptr<ClipboardData>, 10> queue;
	//the current index in the area which represents the first element in the queue
	int startIndex = 0;
};

