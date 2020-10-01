/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

static const char* beacon_rcss =
    "body\n"
    "{\n"
    "	position: absolute;\n"
    "	top: 4pt;\n"
    "	right: 25pt;\n"
    "	z-index: top;\n"
    "	width: 15pt;\n"
    "	font-family: Fira Code;\n"
    "	font-size: 10pt;\n"
    "	color: black;\n"
    "	visibility: hidden;\n"
    "}\n"
    "button\n"
    "{\n"
    "	display: block;\n"
    "	width: 16pt;\n"
    "	height: 16pt;\n"
    "	text-align: center;\n"
    "	border-width: 1px;\n"
    "	font-weight: bold;\n"
    "}\n";

static const char* beacon_rml = "<button class=\"error\">!</button>\n";
