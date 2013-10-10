#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

static void _mkdir(const char *path)
{
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
}



void searchPathForApp(char * buf, int maxl, const char* backupSuffix, const char* appName, const char* def_env, const char* suffix)
{
	const char* template = "%s/%s/%s/%s";
	int templen = strlen(template);
	const char* templates[5] = {&(template[templen]),&(template[9]),&(template[6]),&(template[3]),template};
	const char* fill_ins[4] = {NULL, NULL, NULL, NULL};
	int parts_count = 0;

	const char* basedir = getenv(def_env);

	if(basedir==NULL || basedir[0] == '\0'){
		fprintf(stderr, "Warning: Directory $%s unspecified by environment. Defaulting to ~/%s.\n",def_env,backupSuffix);
		fflush(stderr);
		fill_ins[parts_count++] = getenv("HOME");
		fill_ins[parts_count++] = backupSuffix;
	} else {
		fflush(stdout);
		fill_ins[parts_count++] = basedir;
	}


	fill_ins[parts_count++] = appName;
	if(suffix!=NULL && suffix[0]!='\0') fill_ins[parts_count++] = suffix;

	template = templates[parts_count];

	int szc;
	switch(parts_count) {
	 case 2:
		 szc=snprintf(buf, maxl, template, fill_ins[0], fill_ins[1]);
		 break;
	 case 3:
		 szc=snprintf(buf, maxl, template, fill_ins[0], fill_ins[1], fill_ins[2]);
		 break;
	 case 4:
		 szc=snprintf(buf, maxl, template, fill_ins[0], fill_ins[1], fill_ins[2], fill_ins[3]);
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
		fprintf(stderr, "Warning: Probable buffer overrun retrieving $%s directory (or default). Truncating.\n",def_env);
		fflush(stderr);
		buf[maxl-1] = '\0';

	} else{
		_mkdir(buf);
	}
}

void getApplicationLogFile(char * buf, int maxl, const char* appName)
{
	searchPathForApp(buf, maxl, ".cache", appName, "XDG_CACHE_HOME", NULL);
	int remainder = maxl - strlen(buf);
	
	const char* ext = ".log";
	if (remainder > 1 + strlen(appName) + 4) {
		strcat(buf,"/");
		strcat(buf,appName);
		strcat(buf,ext);
	} else {
		fprintf(stderr, "Warning: Probable buffer overrun in getApplicationLogFile. Truncating.\n");
		fflush(stderr);
		strncat(buf,"/",maxl);		buf[maxl-1] = '\0';
		strncat(buf,appName,maxl);	buf[maxl-1] = '\0';
		strncat(buf,ext,maxl);		buf[maxl-1] = '\0';
	}

}

//-----------------------------------------------------------------------------
void getApplicationDataDirectory(char * buf, int maxl, const char* appName)
{
	searchPathForApp(buf, maxl, ".local/share", appName, "XDG_DATA_HOME", "data");
}

//-----------------------------------------------------------------------------
void getApplicationScriptsDirectory(char * buf, int maxl, const char* appName)
{
	searchPathForApp(buf, maxl, ".local/share", appName, "XDG_DATA_HOME", "scripts");
}

//-----------------------------------------------------------------------------
void getUserHomeDirectory(char * buf, int maxl)
{
	char * homeDir = getenv("HOME");
	strncpy(buf,homeDir,maxl);
	if(buf[maxl-1]!='\0'){
		fprintf(stderr, "Warning: Probable buffer overrun retrieving home directory. Truncating.\n");
		fflush(stderr);
		buf[maxl-1] = '\0';
	}
}

//-----------------------------------------------------------------------------
void getTemporaryDirectory(char * buf, int maxl)
{
	char ttemp[14] = "/tmp/T-XXXXXX";
	char* tpath = mkdtemp(ttemp);
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

