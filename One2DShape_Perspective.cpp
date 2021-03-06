// This program is a demo for perspective projection using a 2D object (triangle)

#include <iostream> // Anticipated to use C++ later (not used in this program)
#include <stdio.h> // for printf()
#include <stdlib.h> // for exit()
#include <memory.h> // for memory()

#include <X11/Xlib.h> // for the native windowing
#include <X11/Xutil.h> // for the visual and related api
#include <X11/XKBlib.h> // for keyboard api - XkbKeycodeToKeysym()

#include <GL/gl.h>  // for OpenGL
#include <GL/glx.h> // for bridging-api
#include <GL/glu.h> // for OpenGL

// namespaces
using namespace std; // Anticipated to use C++ later (not used in this progra)

// global variable declarations
bool gbFullscreen = false;
int giWindowWidth = 800;
int giWindowHeight = 600;
Display *gpDisplay=NULL; // 'Display' is a struct (or connection) used by the X client to communicate with the X server (O.S)
XVisualInfo *gpXVisualInfo=NULL; // 'XVisualInfo' is a struct that stores information about the visual (More details written in notes in createWindow() where this variable is used).
Colormap gColormap; // color palette
Window gWindow;
GLXContext gGLXcontext; // GLXContext is a data structure which acts like a state machine that stores all the states associated with this instance of OpenGL.

// entry point function
int main(void){

	//function prototypes
	void createWindow(void);
	void toggleFullscreen(void);
	void initialize(void);
	void display(void);
	void resize(int, int);
	void uninitialize(void);

	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;

	bool bDone=false; // this flag will be used to 'close' the window
	
	// code

	// 1. Create the window
	createWindow();

	// 2. Initialize OpenGL
	initialize();

	// 3. Handle events on this window
	XEvent event;
	KeySym keysym;

	while(bDone==false){
	
		while(XPending(gpDisplay)){ // XPending() func. returns the number of events that have been received from the X server but have not been removed from the event queue. 'gpDisplay' - connection to the X server

			XNextEvent(gpDisplay, &event); // XNextEvent() copies the first event from the event queue into the specified 'event' obj and then removes it from the event queue.
			
			switch(event.type){
				case MapNotify: // Similar to WM_CREATE. This event is generated when the window changes the state from unmapped to mapped.
					break;
				case KeyPress: // handles keyboard button press
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0); // 3rd param - locale (0 is default), 4th param - 'Shift' key status 0 means shift key not used.
				
					switch(keysym){

						case XK_Escape: // XK - X Keycode
							bDone = true;							
							// OR	
							// uninitialize();
							// exit(0);
						case XK_F:
						case XK_f:
							if(gbFullscreen==false){
								toggleFullscreen();
								gbFullscreen = true;
							}
							else{
								toggleFullscreen();
								gbFullscreen = false;
							}
							break;
						default:
							break;
				
					}// END: switch(keysym)
					break; // END: case KeyPress:

				case ButtonPress: // Mouse button events
					switch(event.xbutton.button){
						case 1: // Similar to WM_LBUTTONDOWN
							break;
						case 2: // Similar to WM_MBUTTONDOWN
							break;
						case 3: // Similar to WM_RBUTTONDOWN
							break;
						case 4: // Similar to MOUSE_WHEELUP
							break;
						case 5: // Similar to MOUSE_WHEELDOWN
							break;

					} // END: switch(event.xbutton.button)
					break;
				case MotionNotify: // Similar to WM_MOUSEMOVE
					break;
				case ConfigureNotify: // Similar to WM_RESIZE
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					resize(winWidth, winHeight);
					break;
				case Expose: // Similar to WM_PAINT
					break;
				case DestroyNotify: // Similar to WM_DESTROY
					break;
				case 33: // handles click on the 'Close' box and also a click on sys menu 'Close'. (Note- preparation for this case has been done in createWindow(...) by creating an atom and setting the WM protocols.
					uninitialize();
					exit(0);
				default:
					break;
				
			} // END: switch(event.type)


		} // END: while(XPending(gpDisplay))	

		display(); // Rendering is done here

	} // END: while(bDone==false)

	uninitialize();
	return (0); // Program never reaches till this statement. Program exits from case 33.
}

void display(void){

	// code
	
	// Step 1: Clear all the pixels
	glClear(GL_COLOR_BUFFER_BIT);

	// ##### TRIANGLE #####
	glLoadIdentity();// Optional call
	glTranslatef(0.0f, 0.0f, -3.0f); 
	glBegin(GL_TRIANGLES);
		// White: Color of the triangle
		glColor3f(1.0f, 1.0f, 1.0f);
		// co-ordinates of the triangle
		glVertex3f(0.0f,1.0f,0.0f); // apex of the triangle
		glVertex3f(-1.0f,-1.0f,0.0f); // left-bottom tip of the triangle
		glVertex3f(1.0f,-1.0f,0.0f); // right-bottom tip of the triangle
	glEnd(); // End paint

	// Step 3: Process buffered OpenGL routines
	glFlush(); // Empty the buffer
}

void toggleFullscreen(void){

	// variable declaration
	Atom wm_state; // Atom to store the current state of the window
	Atom fullscreen; // Atom for fullscreen
	XEvent xev = {0};

	// code
	// Step 1: Get the current state of the window and save it in an atom.
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False); // XInternAtom will return the atom associated with the "_NET_WM_STATE" message where '_NET' stands for Network compliant and 'WM' for Window Message. False - don't create a seperate atom, if it already exists.

	// Step 2: Create a custom event (also called as the 'client' event)
	// Step 2.1: Allocate 0 memory to all the members of the 'event' obj (before it's use)
	memset(&xev,0,sizeof(xev));

	// Step 2.2: Set the values to this custom event
	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32; // 32-bit
	// Note:- "data" is a union in XEvent and "l" is the array in it.
	xev.xclient.data.l[0] = gbFullscreen ? 0 : 1; // Check the value present in 'l[0]' and perform the corresponding action in 'l[1]'.
	
	// Step 3: Create an atom for fullscreen
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False); // XInternAtom returns an atom related to fullscreen. False - don't create a new atom if it already exists.

	xev.xclient.data.l[1] = fullscreen; // add fullscreen atom to 'l[1]'.

	// Send the custom event to the queue
	XSendEvent(gpDisplay,
		   RootWindow(gpDisplay, gpXVisualInfo->screen), // propogate the message to this window
		   False, // Do not propogate the message to the child window
		   StructureNotifyMask, // similar to WM_SIZE
		   &xev); // custom event created above.
}

void initialize(void){

	// function prototype
	void resize(int, int);
	
	// code
	// Step 1: Create an OpenGL context
	/* Func:   glXCreateContext() creates a GLX rendering context and returns it's handle.
	/ Params:  Display - Specifies the connection to the X server
		   XVisualInfo - Specifies the visual that defines the frame buffer resources available to the rendering context
		   GLXContext - Specifies the context with which to share display list. (NULL indicates that the context is not shared)
		   Bool - Specifies the direct rendering. (True - Hardware rendering using the graphics card. False- Software rendering using the O.S)
	*/	   
	gGLXcontext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);

	// Step 2: Make the 'gGLXContext' as the current context
	glXMakeCurrent(gpDisplay, gWindow, gGLXcontext);
	
	// Set clear values [0-1] to the color buffers
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	resize(giWindowWidth, giWindowHeight);	
}


void createWindow(void){ // Note:- All the programs which create their own window are XClients of the XServer (i.e. O.S)

	// function prototypes
	void uninitialize(void);

	// variable declarations
	XSetWindowAttributes winAttribs;

	int defaultScreen;
	int styleMask;

	static int frameBufferList[] = {
		GLX_RGBA, // consider only 'TrueColor' and 'DirectColor' visuals, otherwise PseudoColor and StaticColor visuals are considered
		GLX_RED_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of red color.
		GLX_GREEN_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of green color.
		GLX_BLUE_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of blue color.
		GLX_ALPHA_SIZE, 1, // The returned visual should support a buffer (1-bit atleast) of alpha.
		None // This frameBufferList array must be terminated by 0 and hence 'None' is used.
	};

	// code
	// Step 1: Get the connection of the local display.
	gpDisplay = XOpenDisplay(NULL); // NULL - gives default local connection.
	if(gpDisplay==NULL){
		printf("ERROR: Unable to open X-Display. \n Exiting now...");
		uninitialize();
		exit(1); // Abortive exit and hence a positive number used.
	}

	// Step 2: Get the default monitor/screen (from 'Display' struct) to which the graphic card is connected.
	defaultScreen = XDefaultScreen(gpDisplay);

	// Step 3: Get the XVisualInfo struct obj that meets the minimum requirements.
	// Note:- A single display can support multiple screens. Each screen can have several different visual types supported at different depths.
	gpXVisualInfo=glXChooseVisual(gpDisplay,defaultScreen,frameBufferList); // glXChooseVisual() returns a pointer to a XVisualInfo struct that best meets the specified requirements.

	// Step 4: Set the window attributes
	winAttribs.border_pixel=0; // 0 - default border color
	winAttribs.background_pixmap=0; // background pixmap - images like cursor, icon, etc (0 - default)

	// Get the colormap ID.
	// Note:- The XCreateColormap() func creates a colormap of the specified visual type for the screen on which the specified window resides and returns the colormap ID associated with it.
	winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone); // AllocNone - Dont allocate fixed memory.

	gColormap = winAttribs.colormap;
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen); // set the background color

	// Specify the events which should be sent to this window
	winAttribs.event_mask = ExposureMask | // similar to WM_PAINT
				VisibilityChangeMask | // similar to WM_CREATE
				ButtonPressMask | // handles mouse button events
				KeyPressMask | // handles keyboard key press events
				PointerMotionMask | // handles mouse motion events
				StructureNotifyMask; // case 'ConfigureNotify': handles window resize, similar to WM_SIZE

	// Specify window style
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap; // CW - Create Window or Change Window

	// Step 5: Create the window
	gWindow = XCreateWindow(gpDisplay, // Specifies the connection with the X-server
				RootWindow(gpDisplay, gpXVisualInfo->screen), // Specifies the parent window
				0, // x-coordinate
				0, // y-coordinate
				giWindowWidth, // width
				giWindowHeight, // height
				0, // border-width (0-default)
				gpXVisualInfo->depth, // depth of the window. Color-depth
				InputOutput, // type of the window
				gpXVisualInfo->visual, // type of visual
				styleMask, // style
				&winAttribs); // attributes of the window
	if(!gWindow){
		printf("ERROR: Failed to create the window. \nExiting now...");
		uninitialize();
		exit(1); // Abortive exit and hence a positive integer is used.
	}

	// Step 6: Name in the caption bar
	XStoreName(gpDisplay, gWindow, "Demo - One 2D shape perspective projection");
	
	// Step 7: Process the window 'close' event
	// Step 7.1: Create an atom to handle the close event
	// Note:- 'Atom' is a unique string and it stays in the memory until the app ends
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW",True); // XInternAtom returns the atom id associated with "WM_DELETE_WINDOW" message/event (WM stands for Window Message), True - create an atom even if it exists.

	// Step 7.2: Ask the window manager to add the atom created above i.e. 'windowManagerDelete' to the set of protocols.
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1); // 3rd param is an array of atoms, 4th param - num of protocols to set
	// Step 8: Map this window to the screen
	XMapWindow(gpDisplay, gWindow);

} // END: createWindow(void)

void resize(int width, int height){
	
	// code
	if(height==0)
		height = 1;
	if(width==0)
		width = 1;

	// set the viewport according to the newer size of the window
	glViewport(0,0,(GLsizei) width, (GLsizei) height); // 1st param - x, 2nd param - y, 3rd param - width, 4th param - height

	// Specify which matrix is the current matrix
	glMatrixMode(GL_PROJECTION);

	// Convert the above projection matrix to the identity matrix
	glLoadIdentity();

	// Set up a perspective projection matrix
	// Func:-   gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
	/* Params:- fovy - Specifies the field of view angle, in degrees, in y direction
		    aspect - Specifies the aspect ratio of x (width) to y (height)
		    zNear - Specifies the distance from the viewer to the near clipping plane (always positive).
		    zFar -  Specifies the distance from the viewer to the far clipping plane (always positive). 	

	 */
	gluPerspective(45.0, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);

	// Set the current matrix to the 'Model View Matrix'. (Note:- Model view matrix is in the middle of the screen and all the drawing is done here).
	/* Func:-    glMatrixMode(GLenum mode) - Specify which matrix is the current matrix
	   Params:-  mode - Specifies which matrix stack is the target for subsequent matrix operations. Three values are accepted: GL_MODELVIEW, GL_PROJECTION and GL_TEXTURE.
	*/ 
	glMatrixMode(GL_MODELVIEW);  
	
	// Convert the above Model View Matrix to the identity matrix.
	glLoadIdentity();
}

void uninitialize(void){ // works like a destructor i.e. destroy in the reverse order of creation

	GLXContext currentGLXContext;
	currentGLXContext = glXGetCurrentContext();

	if((currentGLXContext!=NULL) && (currentGLXContext==gGLXcontext)){
		glXMakeCurrent(gpDisplay,0,0);
	}

	if(gGLXcontext){
		glXDestroyContext(gpDisplay, gGLXcontext);
	}
		
	if(gWindow){
		XDestroyWindow(gpDisplay, gWindow);
	}

	if(gColormap){
		XFreeColormap(gpDisplay, gColormap);
	}
	
	if(gpXVisualInfo){
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}

	if(gpDisplay){
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
}

