//
//  IDEKit_TextViewExtensions.h
//  IDEKit
//
//  Created by Glenn Andreas on Sun Aug 17 2003.
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

#import <AppKit/AppKit.h>


@protocol IDEKit_NSTextViewExtendedDelegate <NSObject, NSTextViewDelegate>
- (BOOL)textView:(NSTextView *)textView shouldInterpretKeyEvents: (NSArray *)eventArray;
@end

@interface NSTextView(IDEKit_TextViewExtensions)
// utils
- (NSFont *)currentFont;
- (NSRange) visibleRange;

// line number support
- (NSRange) nthLineRange: (NSInteger) n;
- (NSInteger) lineNumberFromOffset: (NSUInteger) offset;
- (void) selectNthLine: (NSInteger) line;


// Tab stops
- (float) indentWidthFromSpaces: (float) num;
- (void) setUniformTabStops: (float) tabStops; // a negative number is how many spaces wide
- (void) filterRangeToAscii: (NSRange) range;
- (IBAction) filterSelectionToAscii: (id) sender;
- (IBAction) filterAllToAscii: (id) sender;
// page break
- (IBAction) insertPageBreak: (id) sender;

// indentation
- (IBAction) undent: (id) sender;
- (IBAction) dedent: (id) sender;
- (IBAction) indent: (id) sender;
- (IBAction) insertNewlineAndDent: (id) sender;
- (IBAction) insertNewlineAndDedent: (id) sender;
- (IBAction) insertNewlineAndIndent: (id) sender;
- (NSString *) getCurrentIndent;
- (NSString *) getCurrentIndentLimited: (BOOL) limit; // limit to up to selection

// balancing
- (NSInteger) balanceForwards: (NSInteger) location endCharacter: (unichar) rparen;
- (NSInteger) balanceBackwards: (NSInteger) location startCharacter: (unichar) lparen;
- (NSRange) balanceFrom: (NSInteger) location startCharacter: (unichar) lparen endCharacter: (unichar) rparen;
- (NSRange) balanceFrom: (NSInteger) location;
- (IBAction) balance: (id) sender;

// prefixing
- (void) prefixSelectedLinesWith: (NSString *) prefix;
- (void) unprefixSelectedLinesWith: (NSString *) prefix;
- (void) suffixSelectedLinesWith: (NSString *) prefix;
- (void) unsuffixSelectedLinesWith: (NSString *) prefix;

// XML
- (IBAction) escapeXMLCharacters: (id) sender;
- (IBAction) unescapeXMLCharacters: (id) sender;

// Popup at insertion
- (NSPoint) insertionPointLocalCoordinate;
- (NSPoint) insertionPointWindowCoordinate;
- (NSPoint) insertionPointGlobalCoordinate;
- (void) popupMenuAtInsertion: (NSMenu *)menu;
- (void) popupSmallMenuAtInsertion: (NSMenu *)menu;
- (void) popupMenuAtInsertion: (NSMenu *)menu size: (float) size;
- (void) popupHelpTagAtInsertion: (NSAttributedString *)tagContent;
- (id) popupCompletionAtInsertion: (NSArray *)completionList;
@end
