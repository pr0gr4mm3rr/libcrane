#include <stdlib.h>
#include <stdio.h>
#include "crane.hpp"

int main(){
	const size_t bufl = 1024;
	char buf[bufl];
	getApplicationLogFile(buf,bufl,"net.cemetech.sfgp.freebuild"); 		printf("%s\n",buf);
	getApplicationDataDirectory(buf,bufl,"net.cemetech.sfgp.freebuild");	printf("%s\n",buf);
	getApplicationScriptsDirectory(buf,bufl,"net.cemetech.sfgp.freebuild");	printf("%s\n",buf);
	getUserHomeDirectory(buf,bufl);						printf("%s\n",buf);
	getTemporaryDirectory(buf,bufl);					printf("%s\n",buf);

}
