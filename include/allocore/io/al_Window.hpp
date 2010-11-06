#ifndef INCLUDE_AL_WINDOWGL_HPP
#define INCLUDE_AL_WINDOWGL_HPP

/*
 *	OpenGL window
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

/*
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
*/

#include <algorithm>
#include <vector>
#include <string>

namespace al{

class Window;


// can redefine, but should be at least 4
#ifndef AL_MOUSE_MAX_BUTTONS
#define AL_MOUSE_MAX_BUTTONS 4
#endif


/// Constants of keyboard keys.
namespace Key{
	enum t{
		
		// Standard ASCII non-printable characters
		Enter		=3,		/**< */
		BackSpace	=8,		/**< */
		Tab			=9,		/**< */
		Return		=13,	/**< */
		Escape		=27,	/**< */
		Delete		=127,	/**< */
			
		// Non-standard, but common keys.
		F1=256, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
		Insert, Left, Up, Right, Down, PageDown, PageUp, End, Home,
	};
}

/// Window display mode bit flags
namespace DisplayMode{
	enum t{
		SingleBuf	= 1<<0,		/**< Single-buffered */
		DoubleBuf	= 1<<1,		/**< Double-buffered */
		StereoBuf	= 1<<2,		/**< Do left-right stereo buffering */
		AccumBuf	= 1<<3,		/**< Use accumulation buffer */
		AlphaBuf	= 1<<4,		/**< Use alpha buffer */
		DepthBuf	= 1<<5,		/**< Use depth buffer */
		StencilBuf	= 1<<6,		/**< Use stencil buffer */
		Multisample = 1<<7,		/**< Multisampling support */
		DefaultBuf	= DoubleBuf|AlphaBuf|DepthBuf /**< Default display mode */
	};
	inline t operator| (const t& a, const t& b){ return t(int(a) | int(b)); }
	inline t operator& (const t& a, const t& b){ return t(int(a) & int(b)); }
}

/// Cursor icon types
namespace Cursor{
	enum t{
		None		= 0,		/**< */
		Pointer		= 1,		/**< */
		CrossHair,				/**< */
	};
}


using namespace DisplayMode;

/// Keyboard state
class Keyboard{
public:

	/// Constructor.
	Keyboard();
	
	int key() const;			///< Returns key code (non-shifted character) of last key event.
	int keyAsNumber() const;	///< Returns decimal number correlating to key code
	bool alt() const;			///< Whether an alt key is down.
	bool caps() const;			///< Whether capslock is down.
	bool ctrl() const;			///< Whether a ctrl key is down.
	bool meta() const;			///< Whether a meta (e.g. windows, apple) key is down.
	bool shift() const;			///< Whether a shift key is down.
	bool down() const;			///< Whether last event was button down.
	bool isNumber() const;		///< Whether key is a number key
	bool key(int k) const;		///< Whether the last key was 'k'.

	void alt  (bool state);		///< Set alt key state.
	void caps (bool state);		///< Set alt key state.
	void ctrl (bool state);		///< Set ctrl key state.
	void meta (bool state);		///< Set meta key state.
	void shift(bool state);		///< Set shift key state.

	void print();				///< Print keyboard state to stdout.

private:
	friend class WindowImpl;

	int	mKeycode;		// last key event key number
	bool mDown;			// last key event state (pressed or released)
	bool mModifiers[5];	// Modifier key state array (shift, alt, ctrl, caps, meta)
	
	void setKey(int k, bool v);
};


/// Mouse state
class Mouse{
public:
	enum{
		Left	= 0,				/**< Left button */
		Middle	= 1,				/**< Middle button */
		Right	= 2,				/**< Right button */
		Extra	= 3					/**< Start of any extra buttons */
	};
	
	Mouse();
	
	int x() const;					///< Get x position in screen pixels
	int y() const;					///< Get y position in screen pixels
	int dx() const;					///< Get change in x position in screen pixels
	int dy() const;					///< Get change in y position in screen pixels

	int button() const;				///< Get last clicked button
	bool down() const;				///< Get state of last clicked button
	bool down(int button) const;	///< Get state of a button
	bool left() const;				///< Get whether left button is down
	bool middle() const;			///< Get whether middle button is down
	bool right() const;				///< Get whether right button is down
	
private:
	friend class WindowImpl;

	int mX, mY;						// x,y positions
	int mDX, mDY;					// change in x,y positions
	int mButton;					// most recent button changed
	int mBX[AL_MOUSE_MAX_BUTTONS];	// button down xs
	int mBY[AL_MOUSE_MAX_BUTTONS];	// button down ys
	bool mB[AL_MOUSE_MAX_BUTTONS];	// button states
	
	void button(int b, bool v);
	void position(int x, int y);
};



/// Controller for handling input events

/// The return value of the event handlers determines whether or not
/// the event should be propagated to other handlers.
struct InputEventHandler{
	InputEventHandler() : mWindow(NULL) {}
	virtual ~InputEventHandler();

	virtual bool onKeyDown(const Keyboard& k){return true;}	///< Called when a keyboard key is pressed
	virtual bool onKeyUp(const Keyboard& k){return true;}	///< Called when a keyboard key is released

	virtual bool onMouseDown(const Mouse& m){return true;}	///< Called when a mouse button is pressed
	virtual bool onMouseDrag(const Mouse& m){return true;}	///< Called when the mouse moves while a button is down
	virtual bool onMouseMove(const Mouse& m){return true;}	///< Called when the mouse moves
	virtual bool onMouseUp(const Mouse& m){return true;}	///< Called when a mouse button is released

	Window& window(){ return *mWindow; }
	const Window& window() const { return *mWindow; }

private:
	friend class Window;
	Window * mWindow;
	InputEventHandler& window(Window * v){ mWindow=v; return *this; }
};


/// Subscriber for general Window events

/// The return value of the event handlers determines whether or not
/// the event should be propagated to other handlers.
struct WindowEventHandler {
	WindowEventHandler() : mWindow(NULL) {}
	virtual ~WindowEventHandler();

	virtual bool onCreate(){ return true; }					///< Called after window is created with valid OpenGL context
	virtual bool onDestroy(){ return true; }				///< Called before the window and its OpenGL context are destroyed
	virtual bool onFrame(){ return true; }					///< Called every frame
	virtual bool onResize(int dw, int dh){ return true; }	///< Called whenever window dimensions change
	virtual bool onVisibility(bool v){ return true; }		///< Called when window changes from hidden to shown and vice versa
	
	Window& window(){ return *mWindow; }
	const Window& window() const { return *mWindow; }

private:
	friend class Window;
	Window * mWindow;
	WindowEventHandler& window(Window * v){ mWindow=v; return *this; }
};



/// Window with OpenGL context
class Window : public InputEventHandler, public WindowEventHandler {
public:

	/// Window pixel dimensions
	struct Dim{
		int l,t,w,h;
		Dim(int v=0): l(0), t(0), w(v), h(v){}
		Dim(int w_, int h_): l(0), t(0), w(w_), h(h_){}
		Dim(int l_, int t_, int w_, int h_): l(l_), t(t_), w(w_), h(h_){}
		void set(int l_, int t_, int w_, int h_){l=l_;t=t_;w=w_;h=h_;}
	};

	Window();
	virtual ~Window();
	
	/// Create a new window
	
	/// @param[in] dim		Window dimensions in pixels
	/// @param[in] title	Title of window
	/// @param[in] fps		Desired frames/second
	/// @param[in] mode		Display mode bit flags
	void create(
		const Dim& dim,
		const std::string title,
		double fps=40,
		DisplayMode::t mode = DisplayMode::DefaultBuf
	);
	
	/// Destroys current window and its associated OpenGL context
	void destroy();

	const Keyboard& keyboard() const { return mKeyboard; }	///< Get current keyboard state
	const Mouse& mouse() const { return mMouse; }			///< Get current mouse state

	bool cursorHide() const;					///< Whether the cursor is hidden
	Dim dimensions() const;						///< Get current dimensions of window
	bool enabled(DisplayMode::t v) const;		///< Get whether display mode flag is set
	bool fullScreen() const;					///< Get whether window is in fullscreen
	double fps() const;							///< Returns frames/second (requested)
	double avgFps() const;						///< Returns frames/second (running average)
	double spf() const { return 1./fps(); }		///< Returns seconds/frame
	const std::string& title() const;			///< Get title of window
	bool visible() const;						///< Get whether window is visible

	Window& cursor(Cursor::t v);				///< Set cursor type
	Window& cursorHide(bool v);					///< Set cursor hiding
	Window& cursorHideToggle();					///< Toggle cursor hiding
	Window& dimensions(const Dim& v);			///< Set dimensions
	Window& fps(double v);						///< Set frames/second
	
	/// Set fullscreen mode
	
	/// This will make the window go fullscreen without borders and,
	/// if posssible, without changing the display resolution.
	Window& fullScreen(bool on);

	Window& fullScreenToggle();					///< Toggle fullscreen
	Window& hide();								///< Hide window (if showing)
	Window& iconify();							///< Iconify window
	Window& makeActive();						///< Bring window to front
	Window& show();								///< Show window (if hidden)
	Window& title(const std::string& v);		///< Set title

	/// Add input event handler
	
	/// The order by which handlers are added matches their calling order.
	/// The window's event handlers are called before any attached handlers.
	Window& add(InputEventHandler * v){
		mInputEventHandlers.push_back(&(v->window(this)));
		return *this;
	}

	/// Add window event handler
	
	/// The order by which handlers are added matches their calling order.
	/// The window's event handlers are called before any attached handlers.
	Window& add(WindowEventHandler * v){
		mWindowEventHandlers.push_back(&(v->window(this)));
		return *this;
	}

	/// Remove all input event handlers matching argument
	Window& remove(InputEventHandler * v){
		std::remove(mInputEventHandlers.begin(), mInputEventHandlers.end(), v);
		v->mWindow = NULL;
		return *this;
	}

	/// Remove all window event handlers matching argument
	Window& remove(WindowEventHandler * v){
		std::remove(mWindowEventHandlers.begin(), mWindowEventHandlers.end(), v);
		v->mWindow = NULL;
		return *this;
	}

	/// Destroy all created windows
	static void destroyAll();

	static void startLoop();
	static void stopLoop();

private:
	// note: maybe mEventHandlers should be a std::list instead?
	// ljp: A list has faster insertion/removal but is slower to iterate through
	// versus an array. We will probably be iterating more often than inserting
	// removing, so probably best to leave as an array.
	typedef std::vector<InputEventHandler *> InputEventHandlers;
	typedef std::vector<WindowEventHandler *> WindowEventHandlers;
	friend class WindowImpl;

	void doFrameImpl();							///< Calls onFrame() and swaps buffers

	class WindowImpl * mImpl;
	Keyboard mKeyboard;
	Mouse mMouse;
	InputEventHandlers mInputEventHandlers;
	WindowEventHandlers mWindowEventHandlers;

	#define CALL(e)\
	if(e){\
		InputEventHandlers::iterator iter = mInputEventHandlers.begin(); \
		while(iter != mInputEventHandlers.end()){\
			if(!(*iter)->e) break;\
			iter++; \
		}\
	}
	void doMouseDown(const Mouse& m){ CALL(onMouseDown(m)); }
	void doMouseDrag(const Mouse& m){ CALL(onMouseDrag(m)); }
	void doMouseMove(const Mouse& m){ CALL(onMouseMove(m)); }
	void doMouseUp(const Mouse& m){ CALL(onMouseUp(m)); }
	void doKeyDown(const Keyboard& k){ CALL(onKeyDown(k)); }
	void doKeyUp(const Keyboard& k){ CALL(onKeyUp(k)); }
	#undef CALL
	
	#define CALL(e)\
	if(e){\
		WindowEventHandlers::iterator iter = mWindowEventHandlers.begin(); \
		while(iter != mWindowEventHandlers.end()){\
			if(!(*iter)->e) break;\
			iter++; \
		}\
	}
	void doFrame() { CALL(onFrame()); }
	void doCreate(){ CALL(onCreate()); }				
	void doDestroy(){ CALL(onDestroy()); }				
	void doResize(int w, int h){ CALL(onResize(w, h)); }	
	void doVisibility(bool v){ CALL(onVisibility(v)); }
	#undef CALL

	void init();
};



struct StandardWindowKeyControls : InputEventHandler {
	bool onKeyDown(const Keyboard& k){
		if(k.ctrl()){
			switch(k.key()){
				case 'q': Window::stopLoop(); return false;
				//case 'w': window().destroy(); return false;
				case 'h': window().hide(); return false;
				case 'm': window().iconify(); return false;
				case 'c': window().cursorHideToggle(); return false;
				default:;
			}
		}
		else{
			switch(k.key()){
				case Key::Escape: window().fullScreenToggle(); return false;
				default:;
			}
		}
		return true;
	}
};


} // al::

#endif
