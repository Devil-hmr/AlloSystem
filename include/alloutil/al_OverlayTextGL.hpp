#ifndef INC_AL_OVERLAY_TEXT_OPENGL_HPP
#define INC_AL_OVERLAY_TEXT_OPENGL_HPP

/*	Allocore --
	Multimedia / virtual environment application class library
	
	Copyright (C) 2009. AlloSphere Research Group, Media Arts & Technology, UCSB.
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


	File description:
	A printf for OpenGL windows:

	File author(s):
	Graham Wakefield, 2010, grrrwaaa@gmail.com
*/

#include "allocore/graphics/al_Graphics.hpp"
#include "allocore/graphics/al_Font.hpp"

/*!
	A printf for OpenGL windows:
 */

namespace al {

class OverlayText {
public:
	
	OverlayText(std::string fontfile, int maxlines = 20)
	: font(fontfile), mMaxlines(maxlines) {}
	
	void printf(std::string line) {
		lines.push_back(line);
	}
	void printf(const char * fmt, ...) {
		static char line[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(line, 1024, fmt, args);
		va_end(args);
		lines.push_back(line);
	}
	
	void draw() {
		while (lines.size() > mMaxlines) lines.pop_front();
		gl.pushMatrix();
		// because OpenGL/font is upside-down:
		gl.scale(1, -1, 1);
		for (std::list<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
			gl.translate(0, -font.size(), 0);
			//gl.translate(0.5, 0.5, 0);
			font.render(gl, *it);
		}
		gl.popMatrix();
	}
	
	void clear() {
		lines.clear();
	}
	
	std::list<std::string> lines;
	Graphics gl;
	Font font;
	int mMaxlines;
};

} // al::

#endif
