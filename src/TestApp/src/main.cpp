
#include "main.h"



int main()
{




	TestAppWindow Window{ 800, 600, "TestApp" };

	while (!Window.ShouldClose())
	{
		Window.PollEvents();

		Window.Draw();


	}


}