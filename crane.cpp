#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#ifdef XDG_UNIX
#include <dirent.h>
#include <unistd.h>
#endif
#ifdef _WINDOWS
#include <shlobj.h>
#include <Shlwapi.h>
#define snprintf _snprintf
#define SHGFP_TYPE_CURRENT 0
#endif

#include "crane.hpp"

#ifndef _WINDOWS
static void _mkdir(const char *path)
{
#ifdef XDG_UNIX
	/* This is blatantly lifted from the example at 
	 * http://nion.modprobe.de/tmp/mkdir. */
	char opath[256];
	char *p;
	size_t len;
	strncpy(opath, path, sizeof(opath));
	len = strlen(opath);
	if(opath[len - 1] == '/') opath[len - 1] = '\0';
	for(p = opath; *p; p++) if(*p == '/') {
		*p = '\0';
		if(access(opath, F_OK))	mkdir(opath, S_IRWXU);
		*p = '/';
	}
	if(access(opath, F_OK))         /* if path is not terminated with / */
	mkdir(opath, S_IRWXU);
#endif
#ifdef _WINDOWS
    SHCreateDirectoryEx( NULL, path, NULL );
#endif
}
#endif



void searchPathForApp(char * buf, int maxl, const char* backupSuffix, const char* appName, 
#ifdef XDG_UNIX
					  const char* def_env, 
#endif
#ifdef _WINDOWS
					  int folderId,
#endif
					  const char* suffix)
{
#ifdef XDG_UNIX
	const char* templ = "%s/%s/%s/%s";
#endif
#ifdef _WINDOWS
	const char* templ = "%s\\%s\\%s\\%s";
#endif
	size_t templen = strlen(templ);
	const char* templates[5] = {&(templ[templen]),&(templ[9]),&(templ[6]),&(templ[3]),templ};
	const char* fill_ins[4] = {NULL, NULL, NULL, NULL};
	int parts_count = 0;

#ifdef XDG_UNIX
	const char* basedir = getenv(def_env);

	if(basedir==NULL || basedir[0] == '\0'){
		fprintf(stderr, "Warning: Directory $%s unspecified by environment. Defaulting to ~/%s.\n",def_env,backupSuffix);
		fflush(stderr);
		fill_ins[parts_count++] = getenv("HOME");
		fill_ins[parts_count++] = backupSuffix;
#endif
#ifdef _WINDOWS
	char basedir[MAX_PATH];
	HRESULT res = SHGetFolderPath(NULL,folderId,NULL,SHGFP_TYPE_CURRENT,basedir);
	if(res != S_OK){
		fprintf(stderr, "Error: Could not acquire special directory. CSIDL=%d\n",folderId);
		fflush(stderr);
		buf[0] = '\0';
		return;
#endif
	} else {
		fill_ins[parts_count++] = basedir;
	}

	fill_ins[parts_count++] = appName;
	if(suffix!=NULL && suffix[0]!='\0') fill_ins[parts_count++] = suffix;

	templ = templates[parts_count];

	int szc;
	switch(parts_count) {
	 case 2:
		 szc=snprintf(buf, maxl, templ, fill_ins[0], fill_ins[1]);
		 break;
	 case 3:
		 szc=snprintf(buf, maxl, templ, fill_ins[0], fill_ins[1], fill_ins[2]);
		 break;
	 case 4:
		 szc=snprintf(buf, maxl, templ, fill_ins[0], fill_ins[1], fill_ins[2], fill_ins[3]);
		 break;
	 default:
		 szc=0;
		 buf[0] = '\0';
		 fprintf(stderr,"Error: Something has gone terribly wrong inside of libcrane."
				 " We should get parts_count one of 2,3,4; got %d instead.\n",parts_count);
		 fflush(stderr);
		 return;
	}

	if(szc == maxl && buf[maxl-1]!='\0'){
#ifdef XDG_UNIX
		fprintf(stderr, "Warning: Probable buffer overrun retrieving $%s directory (or default). Truncating.\n",def_env);
#endif
#ifdef _WINDOWS
		fprintf(stderr, "Warning: Probable buffer overrun retrieving special directory (CSIDL = %d). Truncating.\n",folderId);
#endif
		fflush(stderr);
		buf[maxl-1] = '\0';

	} else{
		_mkdir(buf);
	}

}

DLLFUNC void getApplicationLogFile(char * buf, int maxl, const char* appName)
{
#ifdef XDG_UNIX
#define backup ".cache"
#define env "XDG_CACHE_HOME"
#define suffix NULL
#define psep "/"
#endif
#ifdef _WINDOWS
#define backup NULL
#define env CSIDL_LOCAL_APPDATA
#define suffix "logs"
#define psep "\\"
#endif
	searchPathForApp(buf, maxl, backup, appName, env, suffix);
	size_t remainder = maxl - strlen(buf);
	
	const char* ext = ".log";
	if (remainder > 1 + strlen(appName) + 4) {
		strcat(buf,psep);
		strcat(buf,appName);
		strcat(buf,ext);
	} else {
		fprintf(stderr, "Warning: Probable buffer overrun in getApplicationLogFile. Truncating.\n");
		fflush(stderr);
		strncat(buf,"/",maxl);		buf[maxl-1] = '\0';
		strncat(buf,appName,maxl);	buf[maxl-1] = '\0';
		strncat(buf,ext,maxl);		buf[maxl-1] = '\0';
	}
#undef backup
#undef env
#undef suffix
#undef psep
}

//-----------------------------------------------------------------------------
DLLFUNC void getApplicationDataDirectory(char * buf, int maxl, const char* appName)
{
#ifdef XDG_UNIX
#define backup ".local/share"
#define env "XDG_DATA_HOME"
#endif
#ifdef _WINDOWS
#define backup NULL
#define env CSIDL_LOCAL_APPDATA
#endif
	searchPathForApp(buf, maxl, backup, appName, env, "data");
#undef backup
#undef env
}

//-----------------------------------------------------------------------------
DLLFUNC void getApplicationScriptsDirectory(char * buf, int maxl, const char* appName)
{
#ifdef XDG_UNIX
#define backup ".local/share"
#define env "XDG_DATA_HOME"
#endif
#ifdef _WINDOWS
#define backup NULL
#define env CSIDL_LOCAL_APPDATA
#endif
	searchPathForApp(buf, maxl, backup, appName, env, "scripts");
#undef backup
#undef env
}

//-----------------------------------------------------------------------------
DLLFUNC void getUserHomeDirectory(char * buf, int maxl)
{
#ifdef XDG_UNIX
	char * homeDir = getenv("HOME");
#endif
#ifdef _WINDOWS
	char homeDir[MAX_PATH];
	HRESULT res = SHGetFolderPath(NULL,CSIDL_PERSONAL,NULL,SHGFP_TYPE_CURRENT,homeDir);
	if(res != S_OK){
		fprintf(stderr, "Error: Could not acquire home directory.\n");
		fflush(stderr);
		buf[0] = '\0';
	}
#endif
	strncpy(buf,homeDir,maxl);
	if(buf[maxl-1]!='\0'){
		fprintf(stderr, "Warning: Probable buffer overrun retrieving home directory. Truncating.\n");
		fflush(stderr);
		buf[maxl-1] = '\0';
	}
}

//-----------------------------------------------------------------------------
DLLFUNC void getTemporaryDirectory(char * buf, int maxl)
{
#ifdef XDG_UNIX
	char ttemp[14] = "/tmp/T-XXXXXX";
	char* tpath = mkdtemp(ttemp);
#endif
#ifdef _WINDOWS
	const char * ttemp = "T-XXXXXX";
	const size_t tbl = 512;
	char tbuf[tbl], tbuf2[tbl];
	size_t tct = snprintf(tbuf,tbl,"%s\\%s",getenv("TEMP"),ttemp);
	if(tct == tbl && tbuf[tct-1]!='\0'){
		fprintf(stderr, "Warning: Probable buffer overrun retrieving %%TEMP%% directory. Truncating.\n");
		fflush(stderr);
		buf[maxl-1] = '\0';
	}
	tct = strlen(tbuf);
	do{
		strcpy(tbuf2,tbuf);
		for(size_t i = tct-1; tbuf[i]=='X';i--){
			int cseed = rand() % (10+26*2);
			cseed = (cseed <= 9) ? (48 + cseed) : ((cseed <= 35) ? (55 + cseed) : (61 + cseed));
			tbuf2[i] = cseed;
		}
		
	} while(PathIsDirectory(tbuf2));
	const char* tpath = tbuf2;
	_mkdir(tpath);
#endif

	if(tpath == NULL){
		buf[0] = '\0';
		fprintf(stderr,"Error: Could not obtain temporary directory.\n");
		fflush(stderr);
	} else {
		strncpy(buf,tpath,maxl);
		if(buf[maxl-1]!='\0'){
			fprintf(stderr,"Warning: Probable buffer overrrun retrieving temporary directory. Truncating.\n");
			fflush(stderr);
			buf[maxl-1] = '\0';
		}
	}
}

