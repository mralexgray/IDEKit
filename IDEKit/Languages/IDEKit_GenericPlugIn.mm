//
//  IDEKit_GenericPlugIn.mm
//  IDEKit
//
//  Created by Glenn Andreas on Fri Aug 6 2004.
//  Copyright (c) 2004 by Glenn Andreas
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Library General Public License for more details.
//  
//  You should have received a copy of the GNU Library General Public
//  License along with this library; if not, write to the Free
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#import "IDEKit_GenericPlugIn.h"
//#import "PythonInterface.h"
#import "IDEKit_LexParser.h"
#import "IDEKit_Delegate.h"
#import "IDEKit_UserSettings.h"
#import "IDEKit_PathUtils.h"

#import <objc/objc-runtime.h>
#import <objc/objc-class.h>

static NSMutableDictionary *genericLangClassVars = NULL;
@implementation IDEKit_GenericLanguage
+ (NSArray *)genericLanguages
{
    NSMutableArray *retval = [NSMutableArray array];
    NSString *root = [[NSBundle bundleForClass:self] pathForResource:@"GenericLanguages" ofType:NULL];
    NSEnumerator *plistEnums = [[NSFileManager defaultManager] enumeratorAtPath:root];
    NSString *path;
    while ((path = [plistEnums nextObject]) != NULL) {
	NSDictionary *language = [NSDictionary dictionaryWithContentsOfFile:[root stringByAppendingPathComponent:path]];
	if (language)
	    [retval addObject: language];
    }
    root = [@"~/Library/Application Support/IDEKit/GenericLanguages/" stringByExpandingTildeInPath];
    plistEnums = [[NSFileManager defaultManager] enumeratorAtPath:root];
    while ((path = [plistEnums nextObject]) != NULL) {
	NSDictionary *language = [NSDictionary dictionaryWithContentsOfFile:[root stringByAppendingPathComponent:path]];
	if (language)
	    [retval addObject: language];
	else
	    NSLog(@"Error in generic language definition %@",[root stringByAppendingPathComponent:path]);
    }
    return retval;
}
+ (void)load
{
    // since all subclasses end up calling this, we get called multiple times, all nice
    // for each class we find
    genericLangClassVars = [[NSMutableDictionary dictionary] retain];
    NSArray *availableGenerics = [self genericLanguages];
    NSEnumerator *langEnums = [availableGenerics objectEnumerator];
    NSString *lang;
    int langID = 1;
    while ((lang = [langEnums nextObject]) != NULL) {
	NSString *langClassName = [NSString stringWithFormat: @"%@%.4d",NSStringFromClass(self),langID];
	// construct the class
	struct objc_class *cls = (struct objc_class *)calloc(sizeof(struct objc_class),1);
	struct objc_class *meta = (struct objc_class *)calloc(sizeof(struct objc_class),1);
	struct objc_class *thisclass = (struct objc_class *)self;
	struct objc_class *thismeta = thisclass->isa;
	cls->isa = meta;
	meta->isa = thismeta->isa;
	cls->super_class = thisclass;
	meta->super_class = thismeta;
	meta->name = cls->name = strdup([langClassName UTF8String]);
	meta->version = cls->version = thisclass->version;
	cls->info = CLS_CLASS;
	meta->info = CLS_META;
	cls->instance_size = thisclass->instance_size; // same size as us
	meta->instance_size = thismeta->instance_size;
	cls->methodLists = (objc_method_list**)calloc(sizeof(void *),1);
	meta->methodLists = (objc_method_list**)calloc(sizeof(void *),1);
	// and leave the rest as NULL, since we add nothing
	objc_addClass((Class)cls); // and hook it all together (and pray)
	[IDEKit_GetLanguagePlugIns() addObject: (Class)cls]; // add that new class
	[genericLangClassVars setObject: lang forKey: langClassName]; // and map the class to our classvar
	langID++;
    }
    //[IDEKit_GetLanguagePlugIns() addObject: self];
}
// all of these are then actually used by synthesized (sub)classes
+ (id)classVar: (NSString *)key
{
    return [[genericLangClassVars objectForKey: NSStringFromClass([self class])] objectForKey: key];
}
- (id)classVar: (NSString *)key
{
    return [[genericLangClassVars objectForKey: NSStringFromClass([self class])] objectForKey: key];
}
+ (NSString *)languageName
{
    return [self classVar: @"Name"];
}
+ (IDEKit_LexParser *)makeLexParser
{
    IDEKit_LexParser *lex = [[IDEKit_LexParser alloc] init];
    NSEnumerator *i = [[self classVar: @"Keywords"] objectEnumerator];
    id x;
    while ((x = [i nextObject]) != NULL) {
	[lex addKeyword: x color: IDEKit_kLangColor_Keywords lexID: 0];
    }
    i = [[self classVar: @"AltKeywords"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addKeyword: x color: IDEKit_kLangColor_AltKeywords lexID: 0];
    }
    i = [[self classVar: @"DocCommentKeywords"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addKeyword: x color: IDEKit_kLangColor_DocKeywords lexID: 0];
    }
    i = [[self classVar: @"MultiLineComment"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addCommentStart: [x objectAtIndex: 0] end: [x objectAtIndex: 0]];
    }
    i = [[self classVar: @"SingleLineComment"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addSingleComment: x];
    }
    i = [[self classVar: @"String"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addStringStart: [x objectAtIndex: 0] end: [x objectAtIndex: 0]];
    }
    i = [[self classVar: @"Character"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addCharacterStart: [x objectAtIndex: 0] end: [x objectAtIndex: 0]];
    }
    if ([self classVar: @"PreprocessorKeywordStart"]) {
	[lex setPreProStart: [self classVar: @"PreprocessorKeywordStart"]];
	i = [[self classVar: @"PreprocessorKeywords"] objectEnumerator];
	while ((x = [i nextObject]) != NULL) {
	    [lex addPreProcessor: x];
	}
    }
    i = [[self classVar: @"Operators"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addOperator: x lexID: 0];
    }
    if ([[self classVar: @"CaseSensitive"] isEqualTo: @"NO"])
	[lex setCaseSensitive: NO];
    i = [[self classVar: @"Markup"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	[lex addMarkupStart: [x objectAtIndex: 0] end: [x objectAtIndex: 0]];
    }
    if ([self classVar: @"IdentifierChars"])
	[lex setIdentifierChars: [NSCharacterSet characterSetWithCharactersInString:[self classVar: @"IdentifierChars"]]];
    if ([self classVar: @"IdentifierStartChars"])
	[lex setFirstIdentifierChars: [NSCharacterSet characterSetWithCharactersInString:[self classVar: @"IdentifierStartChars"]]];
    return [lex autorelease];
}
- (NSString *) getLinePrefixComment
{
    if ([self classVar: @"LinePrefixComment"])
	return [self classVar: @"LinePrefixComment"];// use modified comment
    return [[self classVar: @"SingleLineComment"] objectAtIndex: 0]; // use first single line comment
}

+ (BOOL)isYourFile: (NSString *)name
{
    NSEnumerator *i = [[self classVar: @"Extensions"] objectEnumerator];
    id x;
    while ((x = [i nextObject]) != NULL) {
	if ([[name pathExtension] isEqualToString: x])
	    return YES;
    }
    return NO;
}
+ (BOOL)isYourFile: (NSString *)name withContents: (NSString *)contents
{
    NSEnumerator *i = [[self classVar: @"Extensions"] objectEnumerator];
    id x;
    while ((x = [i nextObject]) != NULL) {
	if ([[name pathExtension] isEqualToString: x])
	    return YES;
    }
    i = [[self classVar: @"MagicWord"] objectEnumerator];
    while ((x = [i nextObject]) != NULL) {
	if ([contents hasPrefix: x])
	    return YES;
    }
  
    return NO;
}

- (BOOL)wantsBreakpoints
{
    // we are a programming language, let IDE delegate determine if debugger available for us
    return [IDEKit languageSupportDebugging: self];
}

- (NSArray *)functionList: (NSString *)source // for popup funcs - return a list of TextFunctionMarkers
{
    NSMutableArray *retval = NULL;
    NSEnumerator *i = [[self classVar: @"FunctionRegexs"] objectEnumerator];
    NSString *x;
    regex_t preg;
    int cflags = REG_EXTENDED|REG_NLSTOP|REG_NEWLINE /*| REG_PROGRESS | REG_DUMP*/;
    if ([[self classVar: @"CaseSensitive"] isEqualTo: @"NO"])
	cflags |= REG_ICASE;
    while ((x = [i nextObject]) != NULL) {
	NSData *stringData = [x dataUsingEncoding: NSUnicodeStringEncoding];
	if (!stringData) continue;
	int err = re_unicomp(&preg, ((unichar *)[stringData bytes])+1, [x length], cflags); // skip BOM
	if (err == 0) {
	    retval = [IDEKit_TextFunctionMarkers makeAllMarks: source inArray: retval
							    fromRegex: &preg withNameIn: 1];
	    regfree(&preg);
	} else {
	    char errbuf[64];
	    regerror(err,&preg,errbuf,sizeof(errbuf));
	    NSLog(@"Generic language %@ function regex %@ error: %s (%d)",[self classVar: @"Name"],x,errbuf,err);
	}
    }
    return retval;
}

- (NSString *) complete: (NSString *)name withParams: (NSArray *)array
{
    return nil;
}


@end
