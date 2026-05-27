

#pragma once

// Taken from https://vkguide.dev/docs/new_chapter_2/vulkan_new_rendering/

struct LeDeletionQueue
{
	std::deque<std::function<void()>> Deletors;

	void AddFunction(std::function<void()>&& function);
	void Flush();
};