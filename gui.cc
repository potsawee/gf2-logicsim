#include "gui.h"

// added macintosh compatibility
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "wx_icon.xpm"
#include <iostream>
#include <algorithm>
using namespace std;

// MyGLCanvas ////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
  EVT_SIZE(MyGLCanvas::OnSize)
  EVT_PAINT(MyGLCanvas::OnPaint)
  EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()

int wxglcanvas_attrib_list[5] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

MyGLCanvas::MyGLCanvas(wxWindow *parent, wxWindowID id, monitor* monitor_mod, names* names_mod, const wxPoint& pos,
		       const wxSize& size, long style, const wxString& name, const wxPalette& palette):
  wxGLCanvas(parent, id, wxglcanvas_attrib_list, pos, size, style, name, palette)
  // Constructor - initialises private variables
{
  context = new wxGLContext(this);
  mmz = monitor_mod;
  nmz = names_mod;
  SetDefault(mmz, nmz);
}

void MyGLCanvas::Render(wxString example_text, int cycles)
  // Draws canvas contents - the following example writes the string "example text" onto the canvas
  // and draws a signal trace. The trace is artificial if the simulator has not yet been run.
  // When the simulator is run, the number of cycles is passed as a parameter and the first monitor
  // trace is displayed.
{
  float y;
  unsigned int i;
  asignal s;

  if (cycles >= 0) cyclesdisplayed = cycles;

  SetCurrent(*context);
  if (!init) {
    InitGL();
    init = true;
  }
  glClear(GL_COLOR_BUFFER_BIT);

  if ((cyclesdisplayed >= 0) && (mmz->moncount() > 0)) { // draw the first monitor signal, get trace from monitor class



			glEnable(GL_LINE_STIPPLE);
			glLineStipple(2,0xAAAA); // dashed lines, factor 2
			glColor3f(0.7,0.7,0.7);

			glBegin(GL_LINE_STRIP);
			glVertex2f(50, 10);
			glVertex2f(500, 10);
			glEnd();

			glBegin(GL_LINE_STRIP);
			glVertex2f(50, 30);
			glVertex2f(500, 30);
			glEnd();

			glDisable(GL_LINE_STIPPLE);

    glColor3f(1., 0.0, 0.0);
    for(int j = 0; j < mmz->moncount(); ++j)
    {
      glBegin(GL_LINE_STRIP);
      for (i=0; i<cyclesdisplayed; i++)
      {
        if (mmz->getsignaltrace(j, i, s))
        {
          if (s==low) y = 30.0*j+10.0;
          if (s==high) y = 30.0*j+30.0;
          glVertex2f(20*i+10.0, y);
          glVertex2f(20*i+30.0, y);
        }
      }
      glEnd();
    }
    // glBegin(GL_LINE_STRIP);
    // for (i=0; i<cyclesdisplayed; i++) {
    //   if (mmz->getsignaltrace(1, i, s)) {
    //     if (s==low) y = 50.0;
    //     if (s==high) y = 70.0;
    //     glVertex2f(20*i+10.0, y);
    //     glVertex2f(20*i+30.0, y);
    //   }
    // }
    // glEnd();

  } else { // draw an artificial trace

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (i=0; i<15; i++) {
      if (i%3) y = 10.0;
      else y = 30.0;
      glVertex2f(20*i+10.0, y);
      glVertex2f(20*i+30.0, y);
    }
    glEnd();

  }

  // Example of how to use GLUT to draw text on the canvas
  glColor3f(0.0, 0.0, 1.0);
  glRasterPos2f(10, 100);
  for (i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);

  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
  glFlush();
  SwapBuffers();
}



void MyGLCanvas::InitGL()
  // Function to initialise the GL context
{
  int w, h;

  GetClientSize(&w, &h);
  SetCurrent(*context);
  glDrawBuffer(GL_BACK);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glViewport(0, 0, (GLint) w, (GLint) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(pan_x, pan_y, 0.0);
  glScaled(zoom, zoom, zoom);
}

void MyGLCanvas::SetDefault(monitor *monitor_mod, names *names_mod)
{
  init = false;
  pan_x = 0;
  pan_y = 0;
  zoom = 1.0;
  cyclesdisplayed = -1;
  mmz = monitor_mod;
  nmz = names_mod;
}

void MyGLCanvas::OnPaint(wxPaintEvent& event)
  // Event handler for when the canvas is exposed
{
  int w, h;
  wxString text;

  wxPaintDC dc(this); // required for correct refreshing under MS windows
  GetClientSize(&w, &h);
  text.Printf("Canvas redrawn by OnPaint event handler, canvas size is %d by %d", w, h);
  Render(text);
}

void MyGLCanvas::OnSize(wxSizeEvent& event)
  // Event handler for when the canvas is resized
{
  init = false;; // this will force the viewport and projection matrices to be reconfigured on the next paint
}

void MyGLCanvas::OnMouse(wxMouseEvent& event)
  // Event handler for mouse events inside the GL canvas
{
  wxString text;
  int w, h;;
  static int last_x, last_y;

  GetClientSize(&w, &h);
  if (event.ButtonDown()) {
    last_x = event.m_x;
    last_y = event.m_y;
    text.Printf("Mouse button %d pressed at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  }
  if (event.ButtonUp()) text.Printf("Mouse button %d released at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  if (event.Dragging()) {
    pan_x += event.m_x - last_x;
    pan_y -= event.m_y - last_y;
    last_x = event.m_x;
    last_y = event.m_y;
    init = false;
    text.Printf("Mouse dragged to %d %d, pan now %d %d", event.m_x, h-event.m_y, pan_x, pan_y);
  }
  if (event.Leaving()) text.Printf("Mouse left window at %d %d", event.m_x, h-event.m_y);
  if (event.GetWheelRotation() < 0) {
    zoom = zoom * (1.0 - (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
    text.Printf("Negative mouse wheel rotation, zoom now %f", zoom);
  }
  if (event.GetWheelRotation() > 0) {
    zoom = zoom / (1.0 + (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
    text.Printf("Positive mouse wheel rotation, zoom now %f", zoom);
  }

  if (event.GetWheelRotation() || event.ButtonDown() || event.ButtonUp() || event.Dragging() || event.Leaving()) Render(text);
}

// MyFrame ///////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  EVT_MENU(wxID_SAVE, MyFrame::OnSave)
  EVT_BUTTON(MY_BUTTON_RUN, MyFrame::OnButtonRUN)
  EVT_BUTTON(MY_BUTTON_RESET, MyFrame::OnButtonRESET)
  EVT_BUTTON(MY_BUTTON_LOAD, MyFrame::OnButtonLOAD)
  EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)

  // load files
  EVT_TEXT(MY_TEXTCTRL_FILEPATH, MyFrame::OnPathChange)
  EVT_TEXT_ENTER(MY_TEXTCTRL_FILEPATH, MyFrame::OnPathEnter)

  EVT_CHOICE(MY_CHOICE_LIST_SWITCHES, MyFrame::OnChoiceSwitch)
  EVT_CHECKBOX(MY_CHECKBOX_0, MyFrame::OnCheck0)
  EVT_CHECKBOX(MY_CHECKBOX_1, MyFrame::OnCheck1)

  EVT_BUTTON(MY_BUTTON_SET, MyFrame::OnButtonSET)
  EVT_BUTTON(MY_BUTTON_ZAP, MyFrame::OnButtonZAP)

END_EVENT_TABLE()

MyFrame::MyFrame(wxWindow *parent,
  const wxString& title,
  const wxPoint& pos,
  const wxSize& size,
	names *names_mod,
  devices *devices_mod,
  monitor *monitor_mod,
  network *network_mod,
  long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers
{
  SetIcon(wxIcon(wx_icon));

  cyclescompleted = 0;
  nmz = names_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  netz = network_mod;

  if (nmz == NULL || dmz == NULL || mmz == NULL|| netz == NULL) {
    cout << "Cannot operate GUI without names, devices, monitor, or network classes" << endl;
    exit(1);
  }
  // Menu bar
  wxMenu *fileMenu = new wxMenu;
  // About is under guitest menu on mac
  // exit is under guitest menu by default
  fileMenu->Append(wxID_ABOUT, "&About");
  fileMenu->Append(wxID_EXIT, "&Quit");

  // The following behave normally
  // refer to wxStandardID
  fileMenu->Append(wxID_OPEN, "&Open");
  fileMenu->Append(wxID_NEW, "&New");
  fileMenu->Append(wxID_SAVE, "&Save");

  wxMenu *editMenu = new wxMenu;
  editMenu->Append(wxID_UNDO, "&Undo");
  editMenu->Append(wxID_REDO, "&Redo");

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  menuBar->Append(editMenu, "&Edit");
  SetMenuBar(menuBar);

  // main window
  wxBoxSizer *overallSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *filePathSizer = new wxBoxSizer(wxHORIZONTAL);
  filePathSizer->Add(
    new wxButton(this, MY_BUTTON_LOAD, "Load"),
    0,
    wxALL,
    10);
	filePathBox = new wxTextCtrl(this, MY_TEXTCTRL_FILEPATH, "", wxDefaultPosition, wxSize(800, -1), wxTE_PROCESS_ENTER);
  filePathSizer->Add(
    filePathBox,
    0,
    wxEXPAND | wxALL,
    10);
    
  int* x,y;
  wxSize s;
  wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

  wxBoxSizer *displaySizer = new wxBoxSizer(wxVERTICAL);

	scrolledWindow = new wxScrolledWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL);
  displaySizer->Add(scrolledWindow, 1, wxEXPAND|wxALL, 10);
  wxBoxSizer *swinSizer = new wxBoxSizer(wxVERTICAL);
	scrolledWindow->SetAutoLayout(true);
  scrolledWindow->SetMinSize(wxSize(500, 400));
	scrolledWindow->SetScrollRate(20,20);
  canvas = new MyGLCanvas(
    scrolledWindow, 
    wxID_ANY, 
    monitor_mod, 
    names_mod, 
    wxDefaultPosition, 
    wxSize(600, 500));
  // displaySizer->Add(canvas, 1, wxEXPAND | wxALL, 10);
  swinSizer->Add(canvas, 1, wxRIGHT|wxBOTTOM|wxEXPAND, 20);
	scrolledWindow->SetSizer(swinSizer);
  // scrolledWindow->GetVirtualSize(&x, &y);
  // std::cout << x << "   "<< y << "\n";

  displaySizer->Add(
    new wxStaticText(this, wxID_ANY, "Log Activity"),
    0,
    wxTOP|wxLEFT|wxRIGHT,
    10);

  logMessagePanel = new wxTextCtrl(this,
      MY_TEXTCTRL_LOG,
      "",
      wxDefaultPosition,
      wxSize(500, 100),
      wxTE_MULTILINE|wxTE_READONLY);

  displaySizer->Add(
    logMessagePanel,
    0,
    wxALL,
    10);

  bottomSizer->Add(displaySizer, 1, wxEXPAND);
  
  //bottom-right config-op panel
  wxBoxSizer *bottomRightSizer = new wxBoxSizer(wxVERTICAL);

  // configuration sizer
  wxStaticBoxSizer *configSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Configuration");
  // switch sizers
  wxStaticBoxSizer *switchSizer = new wxStaticBoxSizer(wxHORIZONTAL, configSizer->GetStaticBox(), "Switches");
  switchChoice = new wxChoice(
    switchSizer->GetStaticBox(),
    MY_CHOICE_LIST_SWITCHES,
    wxDefaultPosition,
    wxDefaultSize,
    0,
    NULL,
    0,
    wxDefaultValidator,
    "Switches");
  switchSizer->Add(switchChoice, 0, wxALL, 5);
  wxStaticBoxSizer *switchStateSizer = new wxStaticBoxSizer(wxHORIZONTAL, switchSizer->GetStaticBox(), "Switch State");
  switchState0 = new wxCheckBox(switchStateSizer->GetStaticBox(), MY_CHECKBOX_0, "0");
  switchState1 = new wxCheckBox(switchStateSizer->GetStaticBox(), MY_CHECKBOX_1, "1");
  switchStateSizer->Add(switchState0, 0, wxALL, 5);
  switchStateSizer->Add(switchState1, 0, wxALL, 5);
  switchSizer->Add(switchStateSizer, 0, wxALIGN_TOP);

  // monitor sizers
  wxStaticBoxSizer *monitorCtrlSizer = new wxStaticBoxSizer(wxVERTICAL, configSizer->GetStaticBox(), "Monitors");
  wxBoxSizer *monitorSetSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *monitorZapSizer = new wxBoxSizer(wxHORIZONTAL);
  monitorSet = new wxChoice(
    monitorCtrlSizer->GetStaticBox(),
    MY_CHOICE_MONITOR_SET,
    wxDefaultPosition,
    wxDefaultSize,
    0,
    NULL,
    wxCB_SORT,
    wxDefaultValidator,
    "Set Monitors");
  monitorSetSizer->Add(monitorSet, 0, wxALL, 5);
  monitorSetSizer->Add(new wxButton(monitorCtrlSizer->GetStaticBox(), MY_BUTTON_SET, "Set"), 0, wxALL, 10);
  monitorZap = new wxChoice(
    monitorCtrlSizer->GetStaticBox(),
    MY_CHOICE_MONITOR_ZAP,
    wxDefaultPosition,
    wxDefaultSize,
    0,
    NULL,
    wxCB_SORT,
    wxDefaultValidator,
    "Zap Monitors");
  monitorZapSizer->Add(monitorZap, 0, wxALL, 5);
  monitorZapSizer->Add(new wxButton(monitorCtrlSizer->GetStaticBox(), MY_BUTTON_ZAP, "Zap"), 0, wxALL, 10);
  monitorCtrlSizer->Add(monitorSetSizer, 0, wxALIGN_LEFT);
  monitorCtrlSizer->Add(monitorZapSizer, 0, wxALIGN_LEFT);

  configSizer->Add(switchSizer, 0, wxALIGN_TOP|wxALIGN_LEFT);
  configSizer->Add(monitorCtrlSizer, 0, wxALIGN_TOP|wxALIGN_LEFT);
  // end of configuration sizer

  // opration sizer
  wxStaticBoxSizer *opSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Operations");
  wxBoxSizer *buttonSizer1 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *buttonSizer2 = new wxBoxSizer(wxHORIZONTAL);
  opSizer->Add(new wxStaticText(opSizer->GetStaticBox(), wxID_ANY, "No. of Clock Cycles"), 0, wxTOP|wxLEFT|wxRIGHT, 10);
  spin = new wxSpinCtrl(opSizer->GetStaticBox(), MY_SPINCNTRL_ID, wxString("10"));
  opSizer->Add(spin, 0 , wxALL, 10);
  buttonSizer1->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_RUN, "Run"), 0, wxALL, 10);
  buttonSizer1->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_PAUSE, "Pause/Resume"), 0, wxALL, 10);
  buttonSizer2->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_STOP, "Stop"), 0, wxALL, 10);
  buttonSizer2->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_RESET, "Reset"), 0, wxALL, 10);
  opSizer->Add(buttonSizer1, 0 , wxALL, 10);
  opSizer->Add(buttonSizer2, 0 , wxALL, 10);
  // end of operation sizer



  bottomRightSizer->Add(configSizer, 0, wxALIGN_LEFT);
  bottomRightSizer->Add(opSizer, 0, wxALIGN_LEFT);
  bottomSizer->Add(bottomRightSizer, 0, wxALIGN_TOP);
  // bottomSizer->Add(switchSizer, 0, wxALIGN_TOP);
  // bottomSizer->Add(monitorCtrlSizer, 0, wxALIGN_TOP);
  // bottomSizer->Add(button_sizer, 0, wxALIGN_TOP);
  overallSizer->Add(filePathSizer, 0, wxALIGN_LEFT);
  overallSizer->Add(bottomSizer, 0, wxALIGN_LEFT);

  // the top level window should not shrink below this size
  SetSizeHints(800, 600);
  SetSizer(overallSizer);
}

void MyFrame::OnExit(wxCommandEvent &event)
  // Event handler for the exit menu item
{
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
  // Event handler for the about menu item
{
  wxMessageDialog about(this, "Example wxWidgets GUI\nAndrew Gee\nJune 2014", "About Logsim", wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

// @Potsawee 29 May 2018 //
void MyFrame::OnOpen(wxCommandEvent &event)
// Event Handler for 'Open'
{
    wxFileDialog openFileDialog(this, _("Open a circuit definition file"), "", "",
               "GF2 files (*.gf2)|*.gf2", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if(openFileDialog.ShowModal() == wxID_CANCEL)
		return;			// user changed idea..
    // proceed loading the file chosen by the user
    filePath = openFileDialog.GetPath();
    filePathBox->SetValue(filePath);
}
void MyFrame::OnSave(wxCommandEvent &event)
// Event Handler for 'Save'
{
	//@potsawee I still have no idea what to implement here haha
}
/* --------------------- */

void MyFrame::OnButtonRUN(wxCommandEvent &event)
  // Event handler for the push button
{
  int n, ncycles;

  cyclescompleted = 0;
  dmz->initdevices ();
  mmz->resetmonitor ();
  runnetwork(spin->GetValue());
  canvas->Render("Run button pressed", cyclescompleted);
  logMessagePanel->AppendText(getCurrentTime()+"Start running.\n");
}

void MyFrame::OnButtonRESET(wxCommandEvent &event)
  // Event handler for the push button
{
  canvas->SetDefault(mmz, nmz);
  switchChoice->Clear();
  switchState0->SetValue(0);
  switchState1->SetValue(0);
  monitorSet->Clear();
  monitorZap->Clear();
  switchVec.clear();
  setVec.clear();
  zapVec.clear();
  canvas->Render("Reset button pressed.\n", 0);
}

void MyFrame::OnSpin(wxSpinEvent &event)
  // Event handler for the spin control
{
  wxString text;

  text.Printf("New spinctrl value %d", event.GetPosition());
  canvas->Render(text);
}

void MyFrame::OnButtonLOAD(wxCommandEvent &event)
{
  loadFile(filePath);
}

void MyFrame::OnPathEnter(wxCommandEvent &event)
  // Event handler for the text entry field
{
  loadFile(filePath);
}

void MyFrame::OnPathChange(wxCommandEvent &event)
  // Event handler for the text entry field
{
  filePath = event.GetString();
}

// display switch state accordingly
void MyFrame::OnChoiceSwitch(wxCommandEvent &event)
{
  // todo: find a better way of doing so
  currentSwitchIndex = updateCurrentChoice((event.GetString()).ToStdString(), &switchVec);

  if(switchVec[currentSwitchIndex].objVal)
  {
    switchState1->SetValue(1);
    switchState0->SetValue(0);
  }
  else
  {
    switchState1->SetValue(0);
    switchState0->SetValue(1);
  }
}

void MyFrame::OnCheck0(wxCommandEvent &event)
{
  if(switchState1->GetValue()==switchState0->GetValue())//&&(switchState0->GetValue()))
  // todo: two unchecked boxes should not be allowed
  {
    bool ok;
    dmz->setswitch(
      switchVec[currentSwitchIndex].dev,
      low,
      ok
    );
    if(ok)
    {
      switchState1->SetValue(!(switchState1->GetValue()));
      switchVec[currentSwitchIndex].objVal = 0;
      logMessagePanel->AppendText(
      getCurrentTime()+
      switchVec[currentSwitchIndex].objName +
      " is set to 0.\n");
    }
    else
    {
      logMessagePanel->AppendText(
      getCurrentTime()+
      switchVec[currentSwitchIndex].objName +
      " is not set to 0.\n");
    }
  }
}

void MyFrame::OnCheck1(wxCommandEvent &event)
{
  if(switchState1->GetValue()==switchState0->GetValue())//&&(switchState0->GetValue()))
  // todo: two unchecked boxes should not be allowed
  {
    bool ok;
    dmz->setswitch(
      switchVec[currentSwitchIndex].dev,
      high,
      ok
    );
    if(ok)
    {
      switchState0->SetValue(!(switchState0->GetValue()));
      switchVec[currentSwitchIndex].objVal = 1;
      logMessagePanel->AppendText(
      getCurrentTime()+
      switchVec[currentSwitchIndex].objName +
      " is set to 1.\n");
    }
    else
    {
      logMessagePanel->AppendText(
      getCurrentTime()+
      switchVec[currentSwitchIndex].objName +
      " is not set to 1.\n");     
    }
  }  
}

void MyFrame::OnButtonSET(wxCommandEvent& event)
{
  if(monitorSet->IsEmpty())
  {
    logMessagePanel->AppendText(
    getCurrentTime()+
    "No monitor is available to be set.\n");
  }
  else
  {
    currentSetIndex = updateCurrentChoice((monitorSet->GetStringSelection()).ToStdString(), &setVec);
    bool ok = true;
    mmz->makemonitor(setVec[currentSetIndex].dev, setVec[currentSetIndex].output, ok);
    // std::cout << setVec[currentSetIndex].objName << "    "
    // << setVec[currentSetIndex].dev << "    "
    // << setVec[currentSetIndex].output << "\n";
    if(ok)
    {
      logMessagePanel->AppendText(
        getCurrentTime()+
        "Monitor "+
        monitorSet->GetStringSelection() +
        " is set.\n");
      monitorSet->Delete(currentSetIndex);
      setVec[currentSetIndex].objVal = !setVec[currentSetIndex].objVal;
      zapVec.push_back(setVec[currentSetIndex]);
      std::sort(zapVec.begin(), zapVec.end());
      setVec.erase(setVec.begin()+currentSetIndex);
      monitorZap->Clear();
      for(std::vector<MyChoiceObj>::iterator it = zapVec.begin() ; it != zapVec.end(); ++it)
      {
        monitorZap->Append(it->objName);
      }
      currentSetIndex = 0;
    }
    else
    {
      logMessagePanel->AppendText(
        getCurrentTime()+
        "Monitor "+
        monitorSet->GetStringSelection() + 
        " is not set successfully.\n");
    }
  }
}

void MyFrame::OnButtonZAP(wxCommandEvent& event)
{
  if(monitorZap->IsEmpty())
  {
    logMessagePanel->AppendText(
    getCurrentTime()+
    "No monitor is available to be zapped.\n");
  }
  else
  {
    currentZapIndex = updateCurrentChoice((monitorZap->GetStringSelection()).ToStdString(), &zapVec);
    bool ok = true;
    mmz->remmonitor(zapVec[currentZapIndex].dev, zapVec[currentZapIndex].output, ok);
    if(ok)
    {
      logMessagePanel->AppendText(
        getCurrentTime()+
        "Monitor "+
        monitorZap->GetStringSelection() +
        " is zapped.\n");
      monitorZap->Delete(currentZapIndex);
      zapVec[currentZapIndex].objVal = !zapVec[currentZapIndex].objVal;
      setVec.push_back(zapVec[currentZapIndex]);
      std::sort(setVec.begin(), setVec.end());
      zapVec.erase(zapVec.begin()+currentZapIndex);
      monitorSet->Clear();
      for(std::vector<MyChoiceObj>::iterator it = setVec.begin() ; it != setVec.end(); ++it)
      {
        monitorSet->Append(it->objName);
      }
      currentZapIndex = 0;
    }
    else
    {
      logMessagePanel->AppendText(
        getCurrentTime()+
        "Monitor "+
        monitorZap->GetStringSelection() +
        " is not zapped successfully.\n");
    }

  }

}


void MyFrame::loadFile(wxString s)
{
  // todo: check file path validity
  // todo: seems duplicated with reset button, delete one maybe
  // (reset button only sets the canvas
  nmz = new names();
  netz = new network(nmz);
  dmz = new devices(nmz, netz);
  mmz = new monitor(nmz, netz);
  canvas->SetDefault(mmz, nmz);
  switchChoice->Clear();
  switchState0->SetValue(0);
  switchState1->SetValue(0);
  monitorSet->Clear();
  monitorZap->Clear();
  switchVec.clear();
  setVec.clear();
  zapVec.clear();
  mmz->resetmonitor();
  // todo: more to be added upon 'reset'

  std::cout << "Reset/Reload\n";

  ifstream f((filePath.ToStdString()).c_str());
  if(f.good())
  {
    // nmz, netz, dmz, mmz reinitialised here to allow load/reset
    // todo: maybe not necessary to pass them as arguments?
    smz = new scanner(nmz, filePath);
    pmz = new parser(netz, dmz, mmz, smz, nmz);
    if(!(pmz->readin()))
    {// check if the definition file is valid
      logMessagePanel->AppendText(
        getCurrentTime()+
        "Logic Definition File Error.\n");
        
        /* --- Open gedit to edit to file --- */
        string str = "gedit " + filePath.ToStdString();
		const char *command = str.c_str();
		system(command);
		/* ---------------------------------- */
        
    }
    else
    {
      logMessagePanel->AppendText(
        getCurrentTime()+
        "File loaded from  "+
        s + "\n");
      std::vector<MyChoiceObj> signalList;

      devlink currentDevice = netz->devicelist();
	    name currentID;
      while(currentDevice!=NULL)
      {
        currentID = currentDevice->id;
        namestring devName = nmz->getname(currentID);
        if(currentDevice->kind == aswitch)
        {
          MyChoiceObj tempObj;
          tempObj.dev = currentID;
          tempObj.objName = devName;
          if(currentDevice->swstate==low)
          {
            tempObj.objVal = 0;
          }
          else if(currentDevice->swstate==high)
          {
            tempObj.objVal = 1;
          }
          switchVec.push_back(tempObj);
          signalList.push_back(tempObj);
        }
        else
        {
          outplink currentOutput = currentDevice->olist;
          if(currentOutput->id==-1)
          {
            MyChoiceObj tempObj;
            tempObj.dev = currentID;
            tempObj.output = -1;
            tempObj.objName = devName;
            tempObj.objVal = 0;
            signalList.push_back(tempObj);
          }
          else
          {
            while(currentOutput != NULL)
            {
              MyChoiceObj tempObj;
              tempObj.dev = currentID;
              tempObj.output = currentOutput->id;
              tempObj.objName = devName+"."+nmz->getname(currentOutput->id);
              tempObj.objVal = 0;
              signalList.push_back(tempObj);
              currentOutput = currentOutput->next;
            }
          }
        }
        currentDevice = currentDevice->next;
      }
      // for(int mm = 0; mm < signalList.size(); ++mm)
      // {
      //   std::cout << signalList[mm].objName << "\t"
      //   << signalList[mm].dev << "\t"
      //   << signalList[mm].output << "\t"
      //   << "\n";
      // }

      std::sort(switchVec.begin(), switchVec.end());
      for(std::vector<MyChoiceObj>::iterator it = switchVec.begin() ; it != switchVec.end(); ++it)
      {
        switchChoice->Append(it->objName);
      }
      // current switch choice by default is
      currentSwitchIndex = 0;
      // set the value for the first switch
      // todo: find a better way to initialise
      if(switchVec.begin()->objVal)
      {
        switchState1->SetValue(1);
        switchState0->SetValue(0);
      }
      else
      {
        switchState1->SetValue(0);
        switchState0->SetValue(1);
      }

      int monitorCount = mmz->moncount();
      name dev, output;
      for(int i = 0; i < monitorCount; ++i)
      {
        std::string monitorName;
        mmz->getmonname(i, dev, output);
        MyChoiceObj tempObj;
        tempObj.dev = dev;
        tempObj.output = output;
        if(output == -1)
        {
          monitorName = nmz->getname(dev);
        }
        else
        {
          monitorName = nmz->getname(dev) + "." + nmz->getname(output);
        }
        for(int m = 0; m < signalList.size(); ++m)
        {
          if(monitorName == signalList[m].objName)
          {
            tempObj.objName = monitorName;
            tempObj.objVal = 1;
            zapVec.push_back(tempObj);
            signalList.erase(signalList.begin()+m);
          }
        }
      }
      std::sort(zapVec.begin(), zapVec.end());
      for(std::vector<MyChoiceObj>::iterator it = zapVec.begin() ; it != zapVec.end(); ++it)
      {
        monitorZap->Append(it->objName);
      }
      currentZapIndex = 0;
      
      for(int n = 0; n < signalList.size(); ++n)
      {
        setVec.push_back(signalList[n]);
      }
      std::sort(setVec.begin(), setVec.end());
      for(std::vector<MyChoiceObj>::iterator it = setVec.begin() ; it != setVec.end(); ++it)
      {
        monitorSet->Append(it->objName);
      }
      currentSetIndex = 0;
      std::cout<< signalList.size() << "\n";

    }
  }
  else
  {
    logMessagePanel->AppendText(
      getCurrentTime()+
      "File does not exist.\n");
  }

}

int MyFrame::updateCurrentChoice(std::string choiceName, std::vector<MyChoiceObj>* vec)
{
  int i = 0;
  for(std::vector<MyChoiceObj>::iterator it = vec->begin() ; it != vec->end(); ++it)
  {
    if(choiceName == it->objName)
    {
      return i;
    }
    i++;
  }
  return 0;
}

void MyFrame::runnetwork(int ncycles)
  // Function to run the network, derived from corresponding function in userint.cc
{
  bool ok = true;
  int n = ncycles;

  while ((n > 0) && ok) {
    dmz->executedevices (ok);
    if (ok) {
      n--;
      mmz->recordsignals ();
    } else
      cout << "Error: network is oscillating" << endl;
  }
  if (ok) cyclescompleted = cyclescompleted + ncycles;
  else cyclescompleted = 0;
}



std::string getCurrentTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
  std::string str(buffer);
  str += ": ";
  return str;
}

MyChoiceObj::MyChoiceObj(std::string name, bool val)
{
  objName = name;
  objVal = val;
}

MyChoiceObj::MyChoiceObj()
{
  objName = "";
  objVal = 0;
  dev = -1;
  output = -1;
}
