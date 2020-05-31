// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void *macos_internal_create_window(uint32_t width, uint32_t height) {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSUInteger windowStyle = NSTitledWindowMask  | NSClosableWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask;

    id window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, height)
                                          styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO];
    [window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
    [window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    NSView* view = [window contentView];
    view.wantsLayer = YES;
    view.layer = [CAMetalLayer layer];

    [NSApp finishLaunching];

    return view;
}

void macos_internal_single_loop() {
    @autoreleasepool {
        NSEvent* ev;
        do {
            ev = [NSApp nextEventMatchingMask: NSAnyEventMask
                untilDate: nil
                inMode: NSDefaultRunLoopMode
                dequeue: YES
            ];
            if (ev) {
                [NSApp sendEvent: ev];
            }
        } while(ev);
    }
}

void macos_internal_close_window(void* _view) {
    NSView* view = (NSView*)(_view);
    [[view window] close];
}