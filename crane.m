#import <Foundation/Foundation.h>


void searchPathForApp(char * buf, int maxl, NSString* backup, NSString* appName, NSString* prefixDir, NSSearchPathDirectory nspd, NSString* postFix)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *ald = nil;
	OSErr err = 0;
	
	if (ald == nil) {
		NSArray *paths = NSSearchPathForDirectoriesInDomains(nspd, NSUserDomainMask, YES);
		if ([paths count] == 1) {
			ald = [paths objectAtIndex:0];
			if(prefixDir != nil) ald = [ald stringByAppendingPathComponent:prefixDir];
			ald = [ald stringByAppendingPathComponent:appName];
			NSFileManager *fileManager = [NSFileManager defaultManager];
			BOOL isDirectory = NO;
			if (![fileManager fileExistsAtPath:ald isDirectory:&isDirectory]) {
				if (![fileManager createDirectoryAtURL:[NSURL fileURLWithPath:ald] withIntermediateDirectories:YES attributes:nil error:nil]) {
					ald = nil;
					err = 1;
				}
			} else {
				if (!isDirectory) {
					ald = nil;
					err = 1;
				}
			}
		}
	}
	
	if (ald == nil || err != 0){
		ald = backup;
	}
	if(postFix != nil)
		ald = [ald stringByAppendingPathComponent:postFix];
	
	
	[ald getFileSystemRepresentation:buf maxLength:maxl];
	[pool release];
}

void getApplicationLogFile(char * buf, int maxl, const char* appName)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *nAppName = [NSString stringWithCString:appName encoding:NSUTF8StringEncoding];
	searchPathForApp(buf, maxl, [[[[@"~/" stringByStandardizingPath] stringByAppendingPathComponent:@""] stringByAppendingPathExtension:nAppName] stringByAppendingPathComponent:@"logs"], nAppName, @"Logs", NSLibraryDirectory, [[[NSBundle mainBundle] bundleIdentifier] stringByAppendingPathExtension:@"log"]);
	[pool release];
}

//-----------------------------------------------------------------------------
void getApplicationDataDirectory(char * buf, int maxl, const char* appName)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *nAppName = [NSString stringWithCString:appName encoding:NSUTF8StringEncoding];
	searchPathForApp(buf, maxl, [[[[@"~/" stringByStandardizingPath] stringByAppendingPathComponent:@""] stringByAppendingPathExtension:nAppName] stringByAppendingPathComponent:@"data"], nAppName, nil, NSApplicationSupportDirectory, nil);
	[pool release];
}

//-----------------------------------------------------------------------------
void getApplicationScriptsDirectory(char * buf, int maxl, const char* appName)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *nAppName = [NSString stringWithCString:appName encoding:NSUTF8StringEncoding];
	searchPathForApp(buf, maxl, [[[[@"~/" stringByStandardizingPath] stringByAppendingPathComponent:@""] stringByAppendingPathExtension:nAppName] stringByAppendingPathComponent:@"scripts"], nAppName, nil, NSApplicationScriptsDirectory, nil);
	[pool release];
}

//-----------------------------------------------------------------------------
void getUserHomeDirectory(char * buf, int maxl)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[[@"~/" stringByStandardizingPath] getFileSystemRepresentation:buf maxLength:maxl];
	[pool release];
}

//-----------------------------------------------------------------------------
void getTemporaryDirectory(char * buf, int maxl)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* tdir = NSTemporaryDirectory();
	[tdir getFileSystemRepresentation:buf maxLength:maxl];
	[pool release];
}

