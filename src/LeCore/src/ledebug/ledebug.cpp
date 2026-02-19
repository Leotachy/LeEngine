
#include "lecore/lecore.h"

#include "ledebugwin/defdbg.h"


#define MAX_DEBUG_MSG_LENGTH (2048)

static LeDebugHandler CurrentDebugHandler = DefaultDebugHandler;



LeDebugHandler LeDebugSetHandler(LeDebugHandler handler)
{
	if (handler)
	{
		CurrentDebugHandler = handler;
	}
	else
	{
		CurrentDebugHandler = NULL;
	}

	return CurrentDebugHandler;
}



void _LeDebugSendMessage(LeDebugType type, const LeChar* file, const int32 line, const LeChar* message, ...)
{

	const char* MessagePrefix = "";

	switch (type)
	{


	case leDEBUGASSERT:
		MessagePrefix = "ASSERT FIRED:";
		break;
	case leDEBUGERROR:
		MessagePrefix = "ERROR:";
		break;
	case leDEBUGWARN:
		MessagePrefix = "WARNING:";
		break;
	default:
		break;
	}

	

	LeChar msg[MAX_DEBUG_MSG_LENGTH];

	if (message)
	{

		LeChar vaBuf[MAX_DEBUG_MSG_LENGTH];

		va_list args;
		va_start(args, message);
		vsnprintf(vaBuf, sizeof(vaBuf), message, args);
		va_end(args);


		sprintf(msg, "%s file: %s line: %i message: %s\n", MessagePrefix, file, line, vaBuf);
	}
	else
	{
		sprintf(msg, "%s file: %s line: %i\n", MessagePrefix, file, line);
	}

	CurrentDebugHandler(type, msg);

}