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
  MY_TEXTCTRL_LOG,        // for logging text box
  
  MY_BUTTON_LOAD,         // for loading connection definitation file
  MY_TEXTCTRL_FILEPATH,   // storing filepath to def file

  MY_CHECKBOX_0,            
  MY_CHECKBOX_1,            
  MY_CHOICE_LIST_SWITCHES,  

  MY_CHOICE_MONITOR_SET,     
  MY_CHOICE_MONITOR_ZAP,    
  MY_BUTTON_SET,            
  MY_BUTTON_ZAP,            

  MY_BUTTON_RUN,            // todo: implement actual running
  MY_BUTTON_CONTINUE,          // todo: 
  MY_BUTTON_STOP,           // todo: 
  MY_BUTTON_RESET,          // todo: implement actual reset
}; // widget identifiers

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
 private:
  MyGLCanvas *canvas;                     // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;                       // control widget to select the number of cycles
  names *nmz;                             // pointer to names class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  network *netz;
  scanner *smz;
  parser *pmz;

  wxScrolledWindow* scrolledWindow;

  wxTextCtrl *logMessagePanel;
  wxTextCtrl *filePathBox;
  std::vector<MyChoiceObj> switchVec;
  int currentSwitchIndex;
  wxChoice *switchChoice;
  wxCheckBox *switchState0;
  wxCheckBox *switchState1;

  wxChoice *monitorSet;
  wxChoice *monitorZap;
  int currentSetIndex;
  int currentZapIndex;
  std::vector<MyChoiceObj> setVec;  
  std::vector<MyChoiceObj> zapVec;

  wxString filePath; //store location of path

  int cyclescompleted;                    // how many simulation cycles have been completed
  void runnetwork(int ncycles);           // function to run the logic network
  void OnExit(wxCommandEvent& event);     // event handler for exit menu item
  void OnAbout(wxCommandEvent& event);    // event handler for about menu item
  void OnOpen(wxCommandEvent &event);
  void OnSave(wxCommandEvent &event);
  void OnButtonRUN(wxCommandEvent& event);    // event handler for push button
  void OnButtonRESET(wxCommandEvent& event);  // event handler for reset button
  void OnButtonCONTINUE(wxCommandEvent& event);   // event handler for continue button

  // functions related to loading description file
  void OnButtonLOAD(wxCommandEvent& event);
  void OnPathEnter(wxCommandEvent& event);
  void OnPathChange(wxCommandEvent& event);
  void loadFile(wxString s);

  // functions related to switch checkboxes
  void OnCheck0(wxCommandEvent& event);
  void OnCheck1(wxCommandEvent& event); 
  void OnChoiceSwitch(wxCommandEvent& event);

  // functions related to monitor set/zap
  void OnButtonSET(wxCommandEvent& event);
  void OnButtonZAP(wxCommandEvent& event);

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

  int period;
  int height;
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
    {0.5, 0.35, 0.05}   // Yellow
  };

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
