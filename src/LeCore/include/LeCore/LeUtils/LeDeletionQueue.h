

#pragma once

// Taken from https://vkguide.dev/docs/new_chapter_2/vulkan_new_rendering/

struct LeDeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function);
	void flush();
};