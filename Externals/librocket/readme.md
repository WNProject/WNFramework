#libRocket - The HTML / CSS User Interface Library

This is a verison of librocket that has been stripped of everything not needed
for WNFramework. The original librocket information is below.

We expect to make heavy modifications and make backwards incompatible changes
to more easily integrate with the rest of WNFramework. These changes will start
small but grow as our needs evolve.

After initial bring-up of librocket inside of WNFramework, we will be removing
all static global state from librocket, with a goal of eventually removing all
non-smart-ptr memory allocations.

http://librocket.com

libRocket is the C++ user interface package based on the HTML and CSS standards. It is 
designed as a complete solution for any project's interface needs.

libRocket uses the time-tested open standards XHTML1.0 and CSS2.0 (while borrowing features from 
HTML5 and CSS3), and extends them with features suited towards real-time applications. Because of 
this, you don't have to learn a whole new proprietary technology like other libraries in this space.

## Features

- Cross platform architecture: Windows, Mac, Linux, iPhone, etc.
- Dynamic layout system.
- Efficient application-wide styling, with a custom-built templating engine.
- Fully featured control set: buttons, sliders, drop-downs, etc.
- Runtime visual debugging suite.
- Easily integrated and extensible with Python or Lua scripting.

## Extensible
- Abstracted interfaces for plugging in to any game engine.
- Decorator engine allowing custom application-specific effects that can be applied to any element.
- Generic event system that binds seamlessly into existing projects.

## License (MIT)
 
 Copyright (c) 2008-2014 CodePoint Ltd, Shift Technology Ltd, and contributors
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
  
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
