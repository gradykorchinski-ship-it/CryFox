/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@cryfox.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#import <Cocoa/Cocoa.h>

@class CryFoxWebView;

@interface CryFoxWebViewWindow : NSWindow

- (instancetype)initWithWebView:(CryFoxWebView*)web_view
                     windowRect:(NSRect)window_rect;

@property (nonatomic, strong) CryFoxWebView* web_view;

@end
