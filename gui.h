#ifndef gui_h
#define gui_h

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include "names.h"
#include "devices.h"
#include "monitor.h"

enum { 
  // todo: delete unnecessary ones. add event handlers
  MY_SPINCNTRL_ID = wxID_HIGHEST + 1,
  MY_TEXTCTRL_ID,         // todo: delete
  MY_TEXTCTRL_LOG,        // for logging text box
  
  MY_BUTTON_ID1,          // todo: delete
  MY_BUTTON_ID2,          // todo: delete
  
  MY_BUTTON_LOAD,         // for loading connection definitation file
  MY_TEXTCTRL_FILEPATH,   // storing filepath to def file

  MY_CHECKBOX_0,            // todo: switch state 0
  MY_CHECKBOX_1,            // todo: switch state 1
  MY_CHOICE_LIST_SWITCHES,  // todo: switch choice

  MY_CHOICE_MONITOR_SET,    // todo: 
  MY_CHOICE_MONITOR_ZAP,    // todo: 
  MY_BUTTON_SET,            // todo: 
  MY_BUTTON_ZAP,            // todo: 

  MY_BUTTON_RUN,            // todo: implement actual running
  MY_BUTTON_PAUSE,          // todo: 
  MY_BUTTON_STOP,           // todo: 
  MY_BUTTON_RESET,          // todo: implement actual reset
}; // widget identifiers

class MyGLCanvas;

class MyFrame: public wxFrame
{
 public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, 
	  names *names_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL, 
	  long style = wxDEFAULT_FRAME_STYLE); // constructor
 private:
  MyGLCanvas *canvas;                     // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;                       // control widget to select the number of cycles
  names *nmz;                             // pointer to names class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  
  wxTextCtrl *logMessagePanel;
  wxChoice *switchChoice;
  wxCheckBox *switchState0;
  wxCheckBox *switchState1;

  wxChoice *monitorSet;
  wxChoice *monitorZap;

  wxString filePath;

  int cyclescompleted;                    // how many simulation cycles have been completed
  void runnetwork(int ncycles);           // function to run the logic network
  void OnExit(wxCommandEvent& event);     // event handler for exit menu item
  void OnAbout(wxCommandEvent& event);    // event handler for about menu item
  void OnButtonRUN(wxCommandEvent& event);    // event handler for push button
  void OnButtonRESET(wxCommandEvent& event);  // event handler for reset button

  // functions related to loading description file
  void OnButtonLOAD(wxCommandEvent& event);
  void OnPathEnter(wxCommandEvent& event);
  void OnPathChange(wxCommandEvent& event);
  void loadFile(wxString s);

  
  void OnSpin(wxSpinEvent& event);        // event handler for spin control
  void OnText(wxCommandEvent& event);     // event handler for text entry field
  
  DECLARE_EVENT_TABLE()
};
    
class MyGLCanvas: public wxGLCanvas
{
 public:
  MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = "MyGLCanvas", const wxPalette &palette=wxNullPalette); // constructor
  void Render(wxString example_text = "", int cycles = -1); // function to draw canvas contents
  void SetDefault();
 private:
  wxGLContext *context;              // OpenGL rendering context
  bool init;                         // has the OpenGL context been initialised?
  int pan_x;                         // the current x pan
  int pan_y;                         // the current y pan
  double zoom;                       // the current zoom
  int cyclesdisplayed;               // how many simulation cycles have been displayed
  monitor *mmz;                      // pointer to monitor class, used to extract signal traces
  names *nmz;                        // pointer to names class, used to extract signal names
  void InitGL();                     // function to initialise OpenGL context
  void OnSize(wxSizeEvent& event);   // event handler for when canvas is resized
  void OnPaint(wxPaintEvent& event); // event handler for when canvas is exposed
  void OnMouse(wxMouseEvent& event); // event handler for mouse events inside canvas
  DECLARE_EVENT_TABLE()
};

std::string getCurrentTime();
    
#endif /* gui_h */
