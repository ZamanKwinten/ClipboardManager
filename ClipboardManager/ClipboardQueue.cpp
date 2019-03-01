#include "ClipboardQueue.h"



ClipboardQueue::ClipboardQueue() {
	for (int i = 0; i < this->queue.size(); i++) {
		queue[i] = std::move(std::unique_ptr<ClipboardData>(new ClipboardData(L"<empty>")));
	}
}


ClipboardQueue::~ClipboardQueue() {

}

void ClipboardQueue::add(std::unique_ptr<ClipboardData> data) {
	//First manage the start index: We always need to reduce the start index by 1 when adding data since we are overwriting the furthest element of startindex 
	if (startIndex == 0) {
		//special situation we set it to the size of the queue to ensure a normal handling
		startIndex = static_cast<int>(queue.size());
	}
	--startIndex;

	//overwrite the thing; also releases the original data I hope
	queue[startIndex] = std::move(data);
}

ClipboardData ClipboardQueue::operator[](int index) {
	int actual = (index + startIndex) % queue.size();
	return *queue[actual];
}