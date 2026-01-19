/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@cryfox.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#import <Interface/CryFoxWebView.h>
#import <Interface/CryFoxWebViewWindow.h>

#if !__has_feature(objc_arc)
#    error "This project requires ARC"
#endif

@interface CryFoxWebViewWindow ()
@end

@implementation CryFoxWebViewWindow

- (instancetype)initWithWebView:(CryFoxWebView*)web_view
                     windowRect:(NSRect)window_rect
{
    static constexpr auto style_mask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

    self = [super initWithContentRect:window_rect
                            styleMask:style_mask
                              backing:NSBackingStoreBuffered
                                defer:NO];

    if (self) {
        self.web_view = web_view;

        if (self.web_view == nil)
            self.web_view = [[CryFoxWebView alloc] init:nil];

        [self.web_view setClipsToBounds:YES];
    }

    return self;
}

#pragma mark - NSWindow

- (void)setIsVisible:(BOOL)flag
{
    [self.web_view handleVisibility:flag];
    [super setIsVisible:flag];
}

- (void)setIsMiniaturized:(BOOL)flag
{
    [self.web_view handleVisibility:!flag];
    [super setIsMiniaturized:flag];
}

@end
