//
//  NSString102.mm
//  IDEKit
//
//  Created by Glenn Andreas on Tue Jul 27 2004.
//  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
//

#import "NSString102.h"

inline NSUInteger hexdigit(unichar c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return 0;
}

@implementation NSString(NSURLUtilities_102)
- (NSString *)stringByAddingPercentEscapesUsingEncoding:(NSStringEncoding)enc
{
    return self; // !!!:gandreas:20040727 
}

- (NSString *)stringByReplacingPercentEscapesUsingEncoding:(NSStringEncoding)enc
{
    NSMutableString *retval = NULL;
    for (NSUInteger i=0;i<[self length];i++) {
	unichar c = [self characterAtIndex:i];
	if (c == '%') {
	    if (i+2 < [self length]) {
		c = hexdigit([self characterAtIndex:i+1]) * 16 + hexdigit([self characterAtIndex:i+2]);
		if (retval == NULL) {
		    retval = [NSMutableString stringWithCapacity:[self length]];
		    if (i>0) {
			// grab everything before this
			[retval appendString: [self substringToIndex:i]];
		    }
		}
	    }
	}
	// add in character, if any
	if (retval) [retval appendFormat:@"%C",c];
    }
    if (retval)
	return retval;
    return self; // we had no escapes, do nothing
}

@end
