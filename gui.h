#ifndef gui_h
#define gui_h

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "parser.h"
#include "scanner.h"

enum { 
  // todo: delete unnecessary ones. add event handlers
  MY_SPINCNTRL_ID = wxID_HIGHEST + 1,
  MY_TEXTCTRL_LOG,              // for logging text box
  MY_BUTTON_LOAD,               // for loading connection definitation file
  MY_BUTTON_BROWSE,             // for browsing .gf2 files
  MY_TEXTCTRL_FILEPATH,         // storing filepath to def file

  MY_CHECKBOX_0,                // check box for switch signal 0
  MY_CHECKBOX_1,                // check box for switch signal 1
  MY_CHOICE_LIST_SWITCHES,      // choice list for switches

  MY_CHOICE_MONITOR_SET,        // choice list for signals available for monitoring
  MY_CHOICE_MONITOR_ZAP,        // choice list for existing monitors
  MY_BUTTON_SET,                // for setting monitor at certain output
  MY_BUTTON_ZAP,                // for removing one monitor

  MY_BUTTON_RUN,                // for running the simulation
  MY_BUTTON_CONTINUE,           // for continuing simulation
  MY_BUTTON_STOP,               // for stopping the simulation
  MY_BUTTON_QUIT,               // for exiting the software
}; // widget identifiers

// gui-recognised objects, used for interfacing with backend classess
class MyChoiceObj
{
 public:
  MyChoiceObj(std::string name, bool val);
  MyChoiceObj();
  bool operator < (const MyChoiceObj& obj) const
  {
      return (objName < obj.objName);
  }
  bool objVal;
  std::string objName;
  name dev;
  name output;
  devicekind kind;
};

class MyGLCanvas;

class MyFrame: public wxFrame
{
 public:
  MyFrame(wxWindow *parent, 
  const wxString& title, 
  const wxPoint& pos, 
  const wxSize& size, 
	names *names_mod = NULL, 
  devices *devices_mod = NULL, 
  monitor *monitor_mod = NULL, 
  network *network_mod = NULL,
	long style = wxDEFAULT_FRAME_STYLE); // constructor
  wxString filePath;                    // store location of path
  void loadFile(wxString s);

 private:
  MyGLCanvas *canvas;   // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;     // control widget to select the number of cycles
  names *nmz;           // pointer to names class
  devices *dmz;         // pointer to devices class
  monitor *mmz;         // pointer to monitor class
  network *netz;        // pointer to network class
  scanner *smz;         // pointer to scanner class
  parser *pmz;          // pointer to parser class

  wxScrolledWindow* scrolledWindow;     // pointer to the scrolled window, used for displaying signals

  wxTextCtrl *logMessagePanel;          // pointer to the text box for displaying logging message
  wxTextCtrl *filePathBox;              // pointer to the text box for entering logic definition file path
  std::vector<MyChoiceObj> switchVec;   // store a vector of switches
  int currentSwitchIndex;               // the index of the selected switch in the switch choice list
  wxChoice *switchChoice;               // pointer to the switch choice list
  wxCheckBox *switchState0;             // pointer to the switch state check box 0
  wxCheckBox *switchState1;             // pointer to the switch state check box 0

  wxChoice *monitorSet;                 // pointer to monitor set choice list
  wxChoice *monitorZap;                 // pointer to monitor zap choice list
  int currentSetIndex;                  // the index of the selected signal in the choice list
  int currentZapIndex;                  // the index of the selected monitor in the choice list
  std::vector<MyChoiceObj> setVec;      // store a vector of signals that can be monitored
  std::vector<MyChoiceObj> zapVec;      // store a vector of existing monitors
  bool IsStarted;                       // true if the simulation has started
  bool FileLoaded;                      // true if a .gf2 file is loaded

  std::stringstream buffer;             // buffer to store command line error messages for display

  int cyclescompleted;                      // how many simulation cycles have been completed
  void runnetwork(int ncycles);             // function to run the logic network
  void OnAbout(wxCommandEvent& event);      // event handler for about menu item
  void OnOpen(wxCommandEvent &event);       // event handler for open menu item
  void OnHelp(wxCommandEvent &event);       // event handler for help menu item
  // void OnSave(wxCommandEvent &event);
  void OnButtonRUN(wxCommandEvent& event);      // event handler for push button
  void OnButtonQUIT(wxCommandEvent& event);     // event handler for reset button
  void OnButtonCONTINUE(wxCommandEvent& event); // event handler for continue button
  void OnButtonSTOP(wxCommandEvent& event);

  // functions related to loading description file
  void OnButtonLOAD(wxCommandEvent& event);     // event handler for load button
  void OnPathEnter(wxCommandEvent& event);      // event handler for for enter key hit
  void OnPathChange(wxCommandEvent& event);     // event handler for file path changes in the text box

  // functions related to switch checkboxes
  void OnCheck0(wxCommandEvent& event);         // event handler for switch status check box 0
  void OnCheck1(wxCommandEvent& event);         // event handler for switch status check box 1
  void OnChoiceSwitch(wxCommandEvent& event);   // event handler for switch is selected in the list

  // functions related to monitor set/zap
  void OnButtonSET(wxCommandEvent& event);      // event handler for set button
  void OnButtonZAP(wxCommandEvent& event);      // event handler for zap button

  void OnSpin(wxSpinEvent& event);        // event handler for spin control
  void OnText(wxCommandEvent& event);     // event handler for text entry field
  
  int updateCurrentChoice(std::string choiceName, std::vector<MyChoiceObj>* vec);
  DECLARE_EVENT_TABLE()
};
    
class MyGLCanvas: public wxGLCanvas
{
 public:
  MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = "MyGLCanvas", const wxPalette &palette=wxNullPalette); // constructor
  void Render(wxString example_text = "", int cycles = -1); // function to draw canvas contents
  void SetDefault(monitor *monitor_mod, names *names_mod);

  int period;       // period of each cycle
  int height;       // difference between signal 0/1
  float colourBox[10][3] = {
    {0.0, 0.0, 0.0},  // black
    {0.0, 0.0, 0.7},  // Dark Blue
    {0.3, 0.3, 1.0},  // Light Blue
    {0.0, 1.0, 1.0},  // Cyan
    {0.0, 0.7, 0.0},  // Dark Green
    {0.3, 1.0, 0.3},  // Light Green
    {1.0, 0.0, 1.0},  // Magneta
    {1.0, 0.0, 0.0},  // Red
    {1.0, 0.5, 0.0},  // Orange
    {0.5, 0.35, 0.05} // brown
  };

  // lighter version of colours above
  float colourBoxLight[10][3] = {
    {0.7, 0.7, 0.7},  // black
    {0.5, 0.5, 1.0},  // Dark Blue
    {0.7, 0.7, 1.0},  // Light Blue
    {0.7, 1.0, 1.0},  // Cyan
    {0.5, 1.0, 0.5},  // Dark Green
    {0.7, 1.0, 0.7},  // Light Green
    {1.0, 0.7, 1.0},  // Magneta
    {1.0, 0.5, 0.5},  // Red
    {1.0, 0.7, 0.7},  // Orange
    {0.7, 0.55, 0.3}   // Yellow
  };

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
// void sortObjVec();
#endif /* gui_h */
