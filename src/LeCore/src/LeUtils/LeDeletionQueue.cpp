
#include "LeCore/LeCore.h"
#include "LeCore/LeUtils/LeDeletionQueue.h"

void LeDeletionQueue::AddFunction(std::function<void()>&& function)
{
	Deletors.push_back(function);
}

void LeDeletionQueue::Flush()
{
	// reverse iterate the deletion queue to execute all the functions
	for (auto it = Deletors.rbegin(); it != Deletors.rend(); it++)
	{
		(*it)(); //call functors
	}

	Deletors.clear();
}