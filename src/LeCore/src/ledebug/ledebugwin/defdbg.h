
#pragma once

// Default windows debug handler

#include <windows.h>

#include "lecore/lecore.h"

// #define LOG_ONLY

#define LOG_PATH (LECORE_LOG_PATH "ledebug.log")

static LeBool bInitialisedLog = FALSE;

static void le_log_init(void)
{
	FILE* fP = fopen(LOG_PATH, "w");

	if (fP)
	{
		fclose(fP);

		/* Delete old log file */
		remove(LOG_PATH);
	}


	bInitialisedLog = TRUE;
}

static void le_log_fired(LeChar* string)
{

	/* Have we intialised the file?	*/
	if (!bInitialisedLog)
	{
		le_log_init();
	}

	/* Open a file for appending, creating one if it doesn't yet exist. */
	FILE* fP = fopen(LOG_PATH, "a+");

	if (!fP)
	{
		return;
	}

	fprintf(fP, "%s", string);
	fclose(fP);

}


static void DefaultDebugHandler(LeDebugType type, const LeChar* string)
{
	switch (type)
	{
	case leDEBUGASSERT:

		le_log_fired(const_cast<LeChar*> (string));

#ifndef LOG_ONLY
		MessageBox(NULL, string, "LE ASSERT FIRED", MB_OK | MB_ICONERROR | MB_TOPMOST);
#endif

		exit(1);

		break;

	case leDEBUGERROR:

		le_log_fired(const_cast<LeChar*> (string));

#ifndef LOG_ONLY
		MessageBox(NULL, string, "LE ERROR", MB_OK | MB_ICONERROR | MB_TOPMOST);
#endif

		exit(1);

		break;

		break;


	case leDEBUGWARN:
	case leDEBUGMESSAGE:
	
		le_log_fired(const_cast<LeChar*> (string));
	
	break;

	default:
		break;
	}
}


