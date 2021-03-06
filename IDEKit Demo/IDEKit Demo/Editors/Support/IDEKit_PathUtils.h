//
//  IDEKit_PathUtils.h
//  IDEKit
//
//  Created by Glenn Andreas on Mon Feb 24 2003.
//  Copyright (c) 2003, 2004 by Glenn Andreas
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

#import <Foundation/Foundation.h>


enum {
    IDEKit_kPathRelDontAllowUpPath = 1,	// don't return something like {foo}/../bar
    IDEKit_kPathRelDontGoToRoot = 2,		// if on two different FS, don't iterator back to root
    IDEKit_kPathRelReturnSelfOnErr = 0x8000,	// return self instead of NULL on error

    IDEKit_kPathRelAlwaysRelative = 0,	// always return a relative path, no matter what
    IDEKit_kPathRelSane = IDEKit_kPathRelDontGoToRoot | IDEKit_kPathRelReturnSelfOnErr, // try for relative if it isn't absolute anyway
    IDEKit_kPathRelSubpath = IDEKit_kPathRelDontAllowUpPath | IDEKit_kPathRelReturnSelfOnErr, // try for relative if subpath
};

@interface NSString(IDEKit_FindFolder)
+ (NSString *) findFolder: (OSType) folderType forDomain: (NSInteger) domain;
+ (NSString *) userPrefFolderPath;
+ (NSString *) userAppSupportFolderPath;
+ (NSString *) userScratchFolderPath; // "chewable" - temp and will be deleted at reboot
@end
@interface NSString(IDEKit_EscapedShellChars)
- (NSString *)stringByEscapingShellChars;
- (NSString *)stringByReplacingShellChars;
- (NSString *)stringByReplacingVars: (NSDictionary *)vars;
- (NSString *)stringRelativeTo: (NSString *)path name: (NSString *)name;
- (NSString *)stringRelativeTo: (NSString *)path name: (NSString *)name withFlags: (NSInteger) flags;
@end
@interface NSString(IDEKit_SearchFile)
- (NSArray *)pathsToSubFilesEndingWith: (NSString *)pattern extensions: (NSArray *)extensions glob: (BOOL) glob;
- (NSArray *)pathsToSubFilesEndingWith: (NSString *)pattern extensions: (NSArray *)extensions; // if null, all extenstions
@end

@interface NSString(IDEKit_EscapedXMLChars)
- (NSString *)stringByEscapingXMLChars;
- (NSString *)stringFromEscapedXMLChars;
@end

@interface NSString(IDEKit_LineNumbers)
- (NSRange) nthLineRange: (NSInteger) n;
- (NSInteger) lineNumberFromOffset: (NSUInteger) offset;
@end

@interface NSString(IDEKit_Sorting)
- (NSComparisonResult)caseSensitiveCompare:(NSString *)aString;
- (NSComparisonResult)literalCompare:(NSString *)aString;
@end

@interface NSString(IDEKit_PathlessComparison)
- (BOOL) isEqualToLastPathComponent: (NSString *)what;
@end

@interface NSFileHandle(IDEKit_StringWriting)
- (void) writeString: (NSString *)string;
- (void) writeStringWithFormat: (NSString *)format,...;
@end

// By default, loadNibNamed:owner: looks in the owner's class's bundle first.
// We want to look in the main bundle first, since that allows us to "override" the UI
// in the client
@interface NSBundle(IDEKit_OverrideNibLoading)
+ (BOOL) loadOverridenNibNamed: (NSString *)nibName owner: (id) owner;
@end

// There are two representations for source files - the on-disk version, and the in-memory (in-editor) version
// The latter _always_ uses tabs to handle indentation - a tab moves to the next tab stop (which can be set
// to either an absolute measurement, or some sort of multiple of space width).  Regardless, it always shows these
// indentations using the tab stops.  One tab moves to the next tab stop, "n spaces" also moves to the next tab spot
// (assume we have auto-conversion).
// On the disk, however, we don't want to store these indentation markers as tabs (even though that is what they really
// are in memory - it would be simpler if there was something other than a tab character stored there).
// Instead, we "effectively" convert it all to spaces (as if we are using a mono-spaced font) which converts indents
// to some number of blank spaces (which may not be the same width as we show for tab stops).  We then convert two or more
// spaces in a row to tabs, if appropriate.  Note that we may not use tabs at all in the final file.
//
// This allows us to do things like have "1 tab = 8 spaces" (as far as the file is concerned) and yet show "1 indent = 4 spaces"
// visually (and then have the file have 1 tab = 2 indents).
//
// This category also provides a way to sanitize line feeds
enum {
    IDEKit_kUnixEOL,
    IDEKit_kMacintoshEOL,
    IDEKit_kWindowsEOL,
    IDEKit_kUnicodeEOL, // unicode line separator
    IDEKit_kUnicodeEOP // unicode paragraph separator
};

@interface NSString(IDEKit_TabsAndEOL)
// To convert from a file with 1 tab = 8 spaces, to 1 indent = 4 spaces:
//	convertTabsFrom: 8 to: 4
// To convert from an editor with 1 indent = 4 spaces to a file with no tabs:
//	convertTabsFrom: 4 to: 0
// We can also remove trailing whitespace from lines
- (NSString *) convertTabsFrom: (NSInteger) tabWidth to: (NSInteger) indentWidth removeTrailing: (BOOL) removeTrailing;
- (NSString *) sanitizeLineFeeds: (NSInteger) style;
@end

@interface NSString(IDEKit_Indentation)
- (NSString *) leadingIndentString;
@end