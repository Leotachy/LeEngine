
#pragma once

class LeVkObjectCleaner
{

public:
	void Init(void);
	void DeInit(void);

	void PushBuffer(LeVkAllocatedBuffer* pAllocatedBuffer, uint32 FrameIndex);
	void Flush(uint32 FrameIndex);


private:

	std::vector<LeVkAllocatedBuffer> m_Buffers[LE_MAX_FRAMES_IN_FLIGHT];

};