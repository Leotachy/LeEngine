

#pragma once


class LeRenderResource
{

protected:
	LeRenderResource()
	{
		m_Flags = 0;
	}
	virtual ~LeRenderResource() {}
public:

	void AddFlags(uint32 f) { m_Flags |= f; }
	void SetFlags(uint32 f) { m_Flags = f; }
	uint32 GetFlags(void) const { return m_Flags; }
	LeBool HasFlags(uint32 f) { return (m_Flags & f) == f; }
	void DeleteFlags(uint32 f) { m_Flags &= ~f; }

protected:

	uint32 m_Flags;
};