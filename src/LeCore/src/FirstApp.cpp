
#include "FirstApp.h"

namespace Le
{
	void FirstApp::Run()
	{

		while (!m_LeWindow.ShouldClose())
		{
			m_LeWindow.PollEvents();


		}

	}
};

