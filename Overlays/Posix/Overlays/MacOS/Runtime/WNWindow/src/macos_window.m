// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import "WNWindow/inc/helpers.h"
#include <Carbon/Carbon.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

enum macos_key_code keycode_to_macos_key_code(UInt16 virtual_key) {
    switch (virtual_key) {
        case kVK_ANSI_A:
            return macos_key_a;
        case kVK_ANSI_S:
            return macos_key_s;
        case kVK_ANSI_D:
            return macos_key_d;
        case kVK_ANSI_F:
            return macos_key_f;
        case kVK_ANSI_H:
            return macos_key_h;
        case kVK_ANSI_G:
            return macos_key_g;
        case kVK_ANSI_Z:
            return macos_key_z;
        case kVK_ANSI_X:
            return macos_key_x;
        case kVK_ANSI_C:
            return macos_key_c;
        case kVK_ANSI_V:
            return macos_key_v;
        case kVK_ANSI_B:
            return macos_key_b;
        case kVK_ANSI_Q:
            return macos_key_q;
        case kVK_ANSI_W:
            return macos_key_w;
        case kVK_ANSI_E:
            return macos_key_e;
        case kVK_ANSI_R:
            return macos_key_r;
        case kVK_ANSI_Y:
            return macos_key_y;
        case kVK_ANSI_T:
            return macos_key_t;
        case kVK_ANSI_1:
            return macos_key_1;
        case kVK_ANSI_2:
            return macos_key_2;
        case kVK_ANSI_3:
            return macos_key_3;
        case kVK_ANSI_4:
            return macos_key_4;
        case kVK_ANSI_6:
            return macos_key_6;
        case kVK_ANSI_5:
            return macos_key_5;
        case kVK_ANSI_Equal:
            return macos_key_unimplemented;
        case kVK_ANSI_9:
            return macos_key_9;
        case kVK_ANSI_7:
            return macos_key_7;
        case kVK_ANSI_Minus:
            return macos_key_unimplemented;
        case kVK_ANSI_8:
            return macos_key_8;
        case kVK_ANSI_0:
            return macos_key_0;
        case kVK_ANSI_RightBracket:
            return macos_key_unimplemented;
        case kVK_ANSI_O:
            return macos_key_o;
        case kVK_ANSI_U:
            return macos_key_u;
        case kVK_ANSI_LeftBracket:
            return macos_key_unimplemented;
        case kVK_ANSI_I:
            return macos_key_i;
        case kVK_ANSI_P:
            return macos_key_p;
        case kVK_ANSI_L:
            return macos_key_l;
        case kVK_ANSI_J:
            return macos_key_j;
        case kVK_ANSI_Quote:
            return macos_key_unimplemented;
        case kVK_ANSI_K:
            return macos_key_k;
        case kVK_ANSI_Semicolon:
            return macos_key_unimplemented;
        case kVK_ANSI_Backslash:
            return macos_key_unimplemented;
        case kVK_ANSI_Comma:
            return macos_key_unimplemented;
        case kVK_ANSI_Slash:
            return macos_key_unimplemented;
        case kVK_ANSI_N:
            return macos_key_n;
        case kVK_ANSI_M:
            return macos_key_m;
        case kVK_ANSI_Period:
            return macos_key_unimplemented;
        case kVK_ANSI_Grave:
            return macos_key_unimplemented;
        case kVK_ANSI_KeypadDecimal:
            return macos_key_unimplemented;
        case kVK_ANSI_KeypadMultiply:
            return macos_key_unimplemented;
        case kVK_ANSI_KeypadPlus:
            return macos_key_unimplemented;
        case kVK_ANSI_KeypadClear:
            return macos_key_unimplemented;
        case kVK_ANSI_KeypadDivide:
            return macos_key_unimplemented;
        case kVK_ANSI_KeypadEnter:
            return macos_key_return;
        case kVK_ANSI_KeypadMinus:
            return macos_key_unimplemented;
        case kVK_ANSI_KeypadEquals:
            return macos_key_unimplemented;
        case kVK_ANSI_Keypad0:
            return macos_key_num_0;
        case kVK_ANSI_Keypad1:
            return macos_key_num_1;
        case kVK_ANSI_Keypad2:
            return macos_key_num_2;
        case kVK_ANSI_Keypad3:
            return macos_key_num_3;
        case kVK_ANSI_Keypad4:
            return macos_key_num_4;
        case kVK_ANSI_Keypad5:
            return macos_key_num_5;
        case kVK_ANSI_Keypad6:
            return macos_key_num_6;
        case kVK_ANSI_Keypad7:
            return macos_key_num_7;
        case kVK_ANSI_Keypad8:
            return macos_key_num_8;
        case kVK_ANSI_Keypad9:
            return macos_key_num_9;
        case kVK_Return:
            return macos_key_return;
        case kVK_Tab:
            return macos_key_tab;
        case kVK_Space:
            return macos_key_space;
        case kVK_Delete:
            return macos_key_backspace;
        case kVK_Escape:
            return macos_key_esc;
        case kVK_Command:
            return macos_key_unimplemented;
        case kVK_Shift:
            return macos_key_lshift;
        case kVK_CapsLock:
            return macos_key_unimplemented;
        case kVK_Option:
            return macos_key_lalt;
        case kVK_Control:
            return macos_key_lctrl;
        case kVK_RightShift:
            return macos_key_rshift;
        case kVK_RightOption:
            return macos_key_ralt;
        case kVK_RightControl:
            return macos_key_rctrl;
        case kVK_Function:
            return macos_key_unimplemented;
        case kVK_F17:
            return macos_key_unimplemented;
        case kVK_VolumeUp:
            return macos_key_unimplemented;
        case kVK_VolumeDown:
            return macos_key_unimplemented;
        case kVK_Mute:
            return macos_key_unimplemented;
        case kVK_F18:
            return macos_key_unimplemented;
        case kVK_F19:
            return macos_key_unimplemented;
        case kVK_F20:
            return macos_key_unimplemented;
        case kVK_F5:
            return macos_key_unimplemented;
        case kVK_F6:
            return macos_key_unimplemented;
        case kVK_F7:
            return macos_key_unimplemented;
        case kVK_F3:
            return macos_key_unimplemented;
        case kVK_F8:
            return macos_key_unimplemented;
        case kVK_F9:
            return macos_key_unimplemented;
        case kVK_F11:
            return macos_key_unimplemented;
        case kVK_F13:
            return macos_key_unimplemented;
        case kVK_F16:
            return macos_key_unimplemented;
        case kVK_F14:
            return macos_key_unimplemented;
        case kVK_F10:
            return macos_key_unimplemented;
        case kVK_F12:
            return macos_key_unimplemented;
        case kVK_F15:
            return macos_key_unimplemented;
        case kVK_Help:
            return macos_key_unimplemented;
        case kVK_Home:
            return macos_key_unimplemented;
        case kVK_PageUp:
            return macos_key_unimplemented;
        case kVK_ForwardDelete:
            return macos_key_unimplemented;
        case kVK_F4:
            return macos_key_unimplemented;
        case kVK_End:
            return macos_key_unimplemented;
        case kVK_F2:
            return macos_key_unimplemented;
        case kVK_PageDown:
            return macos_key_unimplemented;
        case kVK_F1:
            return macos_key_unimplemented;
        case kVK_LeftArrow:
            return macos_key_left;
        case kVK_RightArrow:
            return macos_key_right;
        case kVK_DownArrow:
            return macos_key_down;
        case kVK_UpArrow:
            return  macos_key_up;
    }
    return macos_key_unimplemented;
}

void *macos_internal_create_window(uint32_t width, uint32_t height, int* number) {
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
    *number = [window windowNumber];
    return view;
}

bool macos_handle_event(struct macos_event* mcevent) {
    @autoreleasepool {
        mcevent->type = macos_event_empty;
        NSEvent* ev;
        ev = [NSApp nextEventMatchingMask: NSAnyEventMask
            untilDate: nil
            inMode: NSDefaultRunLoopMode
            dequeue: YES
        ];
        if (ev) {
            mcevent->window_number = [ev windowNumber];
            [NSApp sendEvent: ev];
            switch ([ev type]) {
                case NSEventTypeLeftMouseDown:
                case NSEventTypeLeftMouseUp:
                case NSEventTypeRightMouseDown:
                case NSEventTypeRightMouseUp:
                case NSEventTypeOtherMouseDown:
                case NSEventTypeOtherMouseUp:
                case NSEventTypeLeftMouseDragged:
                case NSEventTypeRightMouseDragged:
                case NSEventTypeOtherMouseDragged:
                case NSEventTypeMouseMoved:
                    {
                        NSWindow* window = [ev window];
                        if (window) {
                            NSPoint l = [ev locationInWindow];
                            mcevent->mb.x = l.x;
                            mcevent->mb.y = (int)NSHeight([window contentRectForFrameRect: [window frame]])  - l.y;
                            switch([ev type]) {
                                case NSEventTypeLeftMouseDown:
                                case NSEventTypeLeftMouseUp:
                                    mcevent->mb.button = macos_mouse_button_l;
                                    break;
                                case NSEventTypeRightMouseDown:
                                case NSEventTypeRightMouseUp:
                                    mcevent->mb.button = macos_mouse_button_r;
                                    break;
                                case NSEventTypeOtherMouseDown:
                                case NSEventTypeOtherMouseUp:
                                    mcevent->mb.button = macos_mouse_button_m;
                                    break;
                                default:
                                    break;
                            }
                            switch([ev type]) {
                                case NSEventTypeLeftMouseDown:
                                case NSEventTypeRightMouseDown:
                                case NSEventTypeOtherMouseDown:
                                    mcevent->type = macos_event_mouse_down;
                                    break;
                                case NSEventTypeLeftMouseUp:
                                case NSEventTypeRightMouseUp:
                                case NSEventTypeOtherMouseUp:
                                    mcevent->type = macos_event_mouse_up;
                                    break;
                                default:
                                    mcevent->type = macos_event_mouse_move;
                                    break;
                            }
                        }
                    }
                    break;
                case NSEventTypeScrollWheel: {
                     mcevent->type = macos_event_mouse_wheel;
                     CGFloat fl = [ev scrollingDeltaY];
                     mcevent->amount = fl;
                    break;
                }
                case NSEventTypeKeyDown: {
                    mcevent->kb.characters[0] = 0;
                    NSString *s = ev.characters;
                    int i = s.length;
                    (void)i;
                    if (s) {
                        unichar     const   code        =   [s characterAtIndex:0];
                        if (code != NSUpArrowFunctionKey &&
                            code != NSDownArrowFunctionKey &&
                            code != NSLeftArrowFunctionKey &&
                            code != NSRightArrowFunctionKey) {
                            if (![s getCString:mcevent->kb.characters maxLength:256 encoding:NSUTF8StringEncoding]) {
                                mcevent->kb.characters[0] = 0;
                            }
                        }
                    }
                    mcevent->kb.code = keycode_to_macos_key_code([ev keyCode]);
                    mcevent->type = macos_event_key_down;
                    break;
                }
                case NSEventTypeKeyUp:  {
                    mcevent->kb.code = keycode_to_macos_key_code([ev keyCode]);
                    mcevent->type = macos_event_key_up;
                    break;
                }
                case NSEventTypeFlagsChanged:
                default:
                    return false;
            }
            return true;
        }
        return false;
    }
}

void macos_internal_close_window(void* _view) {
    NSView* view = (NSView*)(_view);
    [[view window] close];
}
