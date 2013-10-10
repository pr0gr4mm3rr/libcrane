#ifndef _CRANE_H_
#define _CRANE_H_

#ifdef XDG_UNIX 
#define DLLFUNC  
#endif
#ifdef _WINDOWS
#define DLLFUNC __declspec(dllexport)
#endif

extern "C"  {
	DLLFUNC void getApplicationLogFile(char * buf, int maxl, const char* appName);
	DLLFUNC void getApplicationDataDirectory(char * buf, int maxl, const char* appName);
	DLLFUNC void getApplicationScriptsDirectory(char * buf, int maxl, const char* appName);
	DLLFUNC void getUserHomeDirectory(char * buf, int maxl);
	DLLFUNC void getTemporaryDirectory(char * buf, int maxl);
}

#endif
