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
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
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
  period = 20;
  height = 20;
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

  // std::cout << xx << "    " << yy << std::endl;

  if ((cyclesdisplayed >= 0) && (mmz->moncount() > 0)) 
  { // draw the first monitor signal, get trace from monitor class
    for(int j = 0; j < mmz->moncount(); ++j)
    {
      int yLow = 500 - 50*j;
      int yHigh = yLow + height;

      // doted 0 and 1 lines
      glEnable(GL_LINE_STIPPLE); 
      glLineStipple(2,0xAAAA); // dashed lines, factor 2
	    glColor3f(0.7,0.7,0.7);   
      glBegin(GL_LINE_STRIP);			
      glVertex2f(100, yLow);
      glVertex2f(cyclesdisplayed*20+50, yLow);
      glEnd();

      glBegin(GL_LINE_STRIP);			
      glVertex2f(100, yHigh);
      glVertex2f(cyclesdisplayed*20+100, yHigh);
      glEnd();
      glDisable(GL_LINE_STIPPLE);
      
      // fill areas below high signals
      for(i = 0; i < cyclesdisplayed; ++i)
      {
        if(mmz->getsignaltrace(j, i, s))
        {
          if(s==high)
          {
            glColor3f(colourBoxLight[j][0], colourBoxLight[j][1], colourBoxLight[j][2]);
            glBegin(GL_QUADS);
                glVertex2f(period*i+100.0, yHigh);
                glVertex2f(period*i+100.0, yLow);
                glVertex2f(period*i+120.0, yLow);
                glVertex2f(period*i+120.0, yHigh);
            glEnd();            
          }
        }
      }
        
      // draw signal
      glColor3f(colourBox[j][0], colourBox[j][1], colourBox[j][2]);
      glLineWidth(2);
      glBegin(GL_LINE_STRIP);
      for (i=0; i<cyclesdisplayed; i++) 
      {
        if (mmz->getsignaltrace(j, i, s)) 
        {
          if (s==low) y = yLow;
          if (s==high) y = yHigh;
          glVertex2f(period*i+100.0, y); 
          glVertex2f(period*i+120.0, y);
        }
      }
      glEnd();
      glLineWidth(1);
        
      // set cycle mark
      for (i=0; i<cyclesdisplayed; i++) 
      {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.7, 0.7, 0.7); // set line colour to grey
				glVertex2f(20*i+100.0, yLow); 
				glVertex2f(20*i+100.0, yLow-5);
				glEnd();
			}

			for (i=0; i <= cyclesdisplayed; i+=5) 
      {	
				stringstream ss;
				ss << (i);
				wxString numstr = ss.str();
				
				glColor3f(0.7, 0.7, 0.7); // set font colour to black
				glRasterPos2f(period*i+100, yLow-10);
				for (int ch = 0; ch < numstr.Len(); ch++) {
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, numstr[ch]);
				}
			}
    }
    
    // white background for displaying signal labels
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glVertex2f(100+0.0, 550.0);
        glVertex2f(100+0.0, 50.0);
        glVertex2f(100+99.0, 50.0);
        glVertex2f(100+99.0, 550.0);
    glEnd();
      
    glEnable(GL_LINE_STIPPLE); 
    glLineStipple(2,0xAAAA);    
	glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
        glVertex2f(100+100.0, 550.0);
        glVertex2f(100+100.0, 550-50*(mmz->moncount()));
    glEnd(); 
    glDisable(GL_LINE_STIPPLE);
    
    // signal labels
	for (int j = 0; j < mmz->moncount(); j++) 
    {
      int yLow = 500 - 50*j;
      int yHigh = yLow + height;
      name dev, outp;
			mmz->getmonname(j, dev, outp); 		// dev is now the id of the device
			namestring sigName = nmz->getname(dev);
      if (outp != -1)
      {
        sigName = sigName + "." + nmz->getname(outp);
      }
			glColor3f(0.0, 0.0, 0.0); 			// set text colour to black
			glRasterPos2f(100+20, yLow+10); 	// position is in (columns, rows) from bottom left corner
			for (i = 0; i < sigName.length(); i++) 
      {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, sigName[i]);
			}
		glEnable(GL_LINE_STIPPLE); 
		glLineStipple(2,0xAAAA);  
		glBegin(GL_LINE_STRIP);
			glVertex2f(100+40, yLow);
			glVertex2f(100+100.0, yLow);
		glEnd(); 
		glDisable(GL_LINE_STIPPLE);
		
		glRasterPos2f(100+90, yHigh+4);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, '1');
		
		glRasterPos2f(100+90, yLow+4);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, '0');
		
    }

    // mark the cycle limit
    glBegin(GL_LINE_STRIP);	
    glColor3f(0.7,0.7,0.7);		
    glVertex2f(maxcycles*20+100, 50);
    glVertex2f(maxcycles*20+100, 550);
    glEnd();
  } 
  else 
  { // draw an artificial trace

    // glColor3f(0.0, 1.0, 0.0);
    // glBegin(GL_LINE_STRIP);
    // for (i=0; i<15; i++) {
    //   if (i%3) y = 10.0;
    //   else y = 30.0;
    //   glVertex2f(20*i+10.0, y); 
    //   glVertex2f(20*i+30.0, y);
    // }
    // glEnd();
  }

  // Example of how to use GLUT to draw text on the canvas
  // glColor3f(0.0, 0.0, 1.0);
  // glRasterPos2f(20*xx, 300);
  // for (i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);

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
  init = false; // this will force the viewport and projection matrices to be reconfigured on the next paint
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
  EVT_MENU(wxID_EXIT, MyFrame::OnButtonQUIT)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_HELP, MyFrame::OnHelp)
  EVT_BUTTON(MY_BUTTON_RUN, MyFrame::OnButtonRUN)
  EVT_BUTTON(MY_BUTTON_QUIT, MyFrame::OnButtonQUIT)
  EVT_BUTTON(MY_BUTTON_STOP, MyFrame::OnButtonSTOP)
  // EVT_MENU(wxID_SAVE, MyFrame::OnSave)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  EVT_BUTTON(MY_BUTTON_CONTINUE, MyFrame::OnButtonCONTINUE)
  EVT_BUTTON(MY_BUTTON_LOAD, MyFrame::OnButtonLOAD)
  EVT_BUTTON(MY_BUTTON_BROWSE, MyFrame::OnOpen)
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
  IsStarted = 0;
  FileLoaded = 0;
  cyclescompleted = 0;
  nmz = names_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  netz = network_mod;

  // put wxLANGUAGE_CHINESE_SIMPLIFIED as an argument in Init() to set manually
  locale.Init();
  wxLocale::AddCatalogLookupPathPrefix(wxT("."));
  locale.AddCatalog(wxT("gui"));

  if (nmz == NULL || dmz == NULL || mmz == NULL|| netz == NULL) {
    cout << "Cannot operate GUI without names, devices, monitor, or network classes" << endl;
    exit(1);
  }
  // Menu bar
  wxMenu *fileMenu = new wxMenu;
  // About is under guitest menu on mac
  // exit is under guitest menu by default
  fileMenu->Append(wxID_OPEN, _("&Open"));
  fileMenu->Append(wxID_HELP, _("&Help"));
  fileMenu->Append(wxID_ABOUT, _("&About"));
  fileMenu->Append(wxID_EXIT, _("&Quit"));

  // The following behave normally
  // refer to wxStandardID
  // fileMenu->Append(wxID_NEW, "&New");
  // fileMenu->Append(wxID_SAVE, "&Save");

  // wxMenu *editMenu = new wxMenu;
  // editMenu->Append(wxID_UNDO, "&Undo");
  // editMenu->Append(wxID_REDO, "&Redo");
  
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, _("&File"));
  // menuBar->Append(editMenu, "&Edit");
  SetMenuBar(menuBar);

  // main window
  wxBoxSizer *overallSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *filePathSizer = new wxBoxSizer(wxHORIZONTAL);
  filePathSizer->Add(
    new wxButton(this, MY_BUTTON_LOAD, _("Load")), 
    0, 
    wxALL, 
    10);

  filePathSizer->Add(
    new wxButton(this, MY_BUTTON_BROWSE, _("Browse")), 
    0, 
    wxALL, 
    10);

	filePathBox = new wxTextCtrl(this, MY_TEXTCTRL_FILEPATH, "", wxDefaultPosition, wxSize(10000, -1), wxTE_PROCESS_ENTER);
  filePathSizer->Add(
    filePathBox, 
    0, 
    wxEXPAND | wxALL, 
    10);
    
  int* x,y;
  wxSize s;
  wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);

  wxBoxSizer *displaySizer = new wxBoxSizer(wxVERTICAL);

	//~ scrolledWindow = new wxScrolledWindow(this, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL);
  //~ displaySizer->Add(scrolledWindow, 1, wxEXPAND|wxALL, 10);
  //~ wxBoxSizer *swinSizer = new wxBoxSizer(wxVERTICAL);
	//~ scrolledWindow->SetAutoLayout(true);
  //~ scrolledWindow->SetMinSize(wxSize(2000, 500));
	//~ scrolledWindow->SetScrollbars(30, 30, 120, 60);
  canvas = new MyGLCanvas(
    this, 
    wxID_ANY, 
    monitor_mod, 
    names_mod, 
    wxDefaultPosition, 
    wxDefaultSize);
  // displaySizer->Add(canvas, 1, wxEXPAND | wxALL, 10);
    //~ swinSizer->Add(canvas, 1, wxRIGHT|wxBOTTOM|wxEXPAND, 20);
	//~ scrolledWindow->SetSizer(swinSizer);
  // scrolledWindow->GetVirtualSize(&x, &y);
  // std::cout << x << "   "<< y << "\n";

  displaySizer->Add(canvas, 1, wxEXPAND|wxALL, 10);
  displaySizer->Add(
    new wxStaticText(this, wxID_ANY, _("Log Activity")), 
    0, 
    wxTOP|wxLEFT|wxRIGHT, 
    10);

  logMessagePanel = new wxTextCtrl(this, 
      MY_TEXTCTRL_LOG, 
      "", 
      wxDefaultPosition, 
      wxSize(2000, 100), 
      wxTE_MULTILINE|wxTE_READONLY|wxALL);

  wxBoxSizer *textSizer = new wxBoxSizer(wxHORIZONTAL);
  textSizer->Add(logMessagePanel, 1, wxEXPAND);

  displaySizer->Add(
    textSizer,
    0,
    wxEXPAND|wxALL,
    10);

  bottomSizer->Add(displaySizer, 1, wxEXPAND);
  
  //bottom-right config-op panel
  wxBoxSizer *bottomRightSizer = new wxBoxSizer(wxVERTICAL);

  // configuration sizer
  wxStaticBoxSizer *configSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Configuration"));
  // switch sizers
  wxStaticBoxSizer *switchSizer = new wxStaticBoxSizer(wxHORIZONTAL, configSizer->GetStaticBox(), _("Switches"));
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
  wxStaticBoxSizer *switchStateSizer = new wxStaticBoxSizer(wxHORIZONTAL, switchSizer->GetStaticBox(), _("Switch State"));
  switchState0 = new wxCheckBox(switchStateSizer->GetStaticBox(), MY_CHECKBOX_0, "0");
  switchState1 = new wxCheckBox(switchStateSizer->GetStaticBox(), MY_CHECKBOX_1, "1");
  switchStateSizer->Add(switchState0, 0, wxALL, 5);
  switchStateSizer->Add(switchState1, 0, wxALL, 5);
  switchSizer->Add(switchStateSizer, 0, wxALIGN_TOP);

  // monitor sizers
  wxStaticBoxSizer *monitorCtrlSizer = new wxStaticBoxSizer(wxVERTICAL, configSizer->GetStaticBox(), _("Monitors"));
  wxBoxSizer *monitorSetSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *monitorZapSizer = new wxBoxSizer(wxHORIZONTAL);
  monitorSet = new wxChoice(
    monitorCtrlSizer->GetStaticBox(), 
    MY_CHOICE_MONITOR_SET,
    wxDefaultPosition,
    wxSize(100, -1),
    0,
    NULL,
    wxCB_SORT,
    wxDefaultValidator,
    "Set Monitors");
  monitorSetSizer->Add(monitorSet, 0, wxALL, 5);
  monitorSetSizer->Add(new wxButton(monitorCtrlSizer->GetStaticBox(), MY_BUTTON_SET, _("Set")), 0, wxALL, 10);
  monitorZap = new wxChoice(
    monitorCtrlSizer->GetStaticBox(), 
    MY_CHOICE_MONITOR_ZAP,
    wxDefaultPosition,
    wxSize(100, -1),
    0,
    NULL,
    wxCB_SORT,
    wxDefaultValidator,
    "Zap Monitors");
  monitorZapSizer->Add(monitorZap, 0, wxALL, 5);
  monitorZapSizer->Add(new wxButton(monitorCtrlSizer->GetStaticBox(), MY_BUTTON_ZAP, _("Zap")), 0, wxALL, 10);
  monitorCtrlSizer->Add(monitorSetSizer, 0, wxALIGN_LEFT);
  monitorCtrlSizer->Add(monitorZapSizer, 0, wxALIGN_LEFT);

  configSizer->Add(switchSizer, 0, wxALIGN_TOP|wxALIGN_LEFT);
  configSizer->Add(monitorCtrlSizer, 0, wxALIGN_TOP|wxALIGN_LEFT);
  // end of configuration sizer

  // opration sizer
  wxStaticBoxSizer *opSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Operations"));
  wxBoxSizer *buttonSizer1 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *buttonSizer2 = new wxBoxSizer(wxHORIZONTAL);
  opSizer->Add(new wxStaticText(opSizer->GetStaticBox(), wxID_ANY, _("No. of Clock Cycles")), 0, wxTOP|wxLEFT|wxRIGHT, 10);
  spin = new wxSpinCtrl(opSizer->GetStaticBox(), MY_SPINCNTRL_ID, wxString("10"));
  spin->SetRange(0, 50);
  opSizer->Add(spin, 0 , wxALL, 10);
  buttonSizer1->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_RUN, _("Run")), 0, wxALL, 10);
  buttonSizer1->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_CONTINUE, _("Continue")), 0, wxALL, 10);
  buttonSizer2->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_STOP, _("Stop")), 0, wxALL, 10);
  buttonSizer2->Add(new wxButton(opSizer->GetStaticBox(), MY_BUTTON_QUIT, _("Quit")), 0, wxALL, 10);
  opSizer->Add(buttonSizer1, 0 , wxALL, 10);
  opSizer->Add(buttonSizer2, 0 , wxALL, 10);
  // end of operation sizer

  bottomRightSizer->Add(configSizer, 0, wxALL, 10);
  bottomRightSizer->Add(opSizer, 0, wxALL, 10);
  bottomSizer->Add(bottomRightSizer, 0, wxALIGN_TOP);
  overallSizer->Add(filePathSizer, 0, wxALIGN_LEFT);
  overallSizer->Add(bottomSizer, 1, wxALIGN_LEFT);

  // the top level window should not shrink below this size
  SetSizeHints(1000, 1000);
  //~ SetMaxSize(wxSize(800, 700));
  SetSizer(overallSizer);
}

 void MyFrame::OnOpen(wxCommandEvent &event)
   // Event handler for the about menu item
 {
    wxFileDialog openFileDialog(
      NULL, 
      _("Open a circuit definition file"), 
      "", 
      "",
      _("GF2 files (*.gf2)|*.gf2"), 
      wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if(openFileDialog.ShowModal() == wxID_CANCEL)
		return;			// user changed idea..
    // proceed loading the file chosen by the user
  filePath = openFileDialog.GetPath();
  filePathBox->SetValue(filePath);
 }
 /* --------------------- */

// void MyFrame::OnSave(wxCommandEvent &event)
// // Event Handler for 'Save'
// {
// 	//@potsawee I still have no idea what to implement here haha
// }


void MyFrame::OnAbout(wxCommandEvent &event)
  // Event handler for the about menu item
{
  wxMessageDialog about(
    this, 
    _("Part IIA Project GF2\nLogic Simulator\nNaitian Zheng (nz253)\nPotsawee Manakul (pm574)\nShutong Feng (sf548)\nMay 2018"), 
    _("About Logsim"), 
    wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void MyFrame::OnHelp(wxCommandEvent &event)
  // Event handler for the about menu item
{
  wxMessageDialog about(
    this, 
    _("Please refer to the user manual."), 
    _("Help"), 
    wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void MyFrame::OnButtonRUN(wxCommandEvent &event)
  // Event handler for the push button
{
  if(FileLoaded)
  {
    IsStarted = 1;
    int n, ncycles;
    //~ canvas->SetSize(wxSize(500, 600));
    cyclescompleted = 0;
    dmz->initdevices ();
    mmz->resetmonitor ();
    ncycles = spin->GetValue();
    // todo: change 20 to signal size, scale according to the number of monitors accordingly
    //~ if((ncycles+cyclescompleted)*20+250>500)
    //~ {
    //~ int x, y;
    //~ scrolledWindow->GetViewStart(&x, &y);
      //~ canvas->SetSize((ncycles+cyclescompleted)*20+250, 600);
      //~ scrolledWindow->SetScrollbars(20, 20, (ncycles+cyclescompleted)+15, 20);
      //~ scrolledWindow->Scroll(x, y);
    //~ }
    runnetwork(ncycles);
    canvas->Render("Run button pressed", cyclescompleted);
    logMessagePanel->AppendText(getCurrentTime()+_("Start running.\n"));
  }
  else
  {
    wxMessageDialog opwarning(this, 
      _("Please load a file before running."), 
      _("Warning"), wxOK|wxICON_INFORMATION);    
    opwarning.ShowModal();
  }
}

void MyFrame::OnButtonQUIT(wxCommandEvent &event)
  // Event handler for the push button
{
  wxMessageDialog monitorwarning(this, 
    _("Are you sure you want to quit the programme?"), 
    _("Warning"), wxOK|wxCANCEL);
  if(monitorwarning.ShowModal() == wxID_OK)
  {
    Close(true);
  }
}

void MyFrame::OnButtonSTOP(wxCommandEvent &event)
{
  mmz->resetmonitor();
  canvas->SetDefault(mmz, nmz);
  //~ canvas->SetSize(wxSize(1200, 600));
  //~ scrolledWindow->Scroll(0, 0);
  //~ scrolledWindow->SetScrollbars(20, 20, 60, 30);
  IsStarted = 0;
  // FileLoaded = 0;
}

void MyFrame::OnButtonCONTINUE(wxCommandEvent &event)
{
  if(FileLoaded)
  {
    if(IsStarted)
    {
      int ncycles = spin->GetValue();
      if(cyclescompleted>0)
      {
        if(ncycles+cyclescompleted > maxcycles)
        {
          ncycles = maxcycles-cyclescompleted;
          wxMessageDialog runwarning(this, 
          _("Maximum number of cycles per run (50) is reached."), 
          _("Warning"), wxOK|wxICON_INFORMATION);    
          runwarning.ShowModal();  			
        }
        // todo: change 20 to signal size, scale according to the number of monitors accordingly
        //~ if((ncycles+cyclescompleted)*20+250>500)
        //~ {
        //~ int x, y;
        //~ scrolledWindow->GetViewStart(&x, &y);
          //~ canvas->SetSize((ncycles+cyclescompleted)*20+300, 600);
          //~ scrolledWindow->SetScrollbars(20, 20, (ncycles+cyclescompleted)+15, 30);
          //~ scrolledWindow->Scroll(x, y);
        //~ }
        runnetwork(ncycles);
        canvas->Render("Continue button pressed", cyclescompleted);
        logMessagePanel->AppendText(getCurrentTime()+_("Continue running.\n"));
      }
      else
      {
        logMessagePanel->AppendText(getCurrentTime()+_("Please run before continuing.\n"));
      }
    }
    else
    {
      wxMessageDialog contwarning(this, 
        _("Please run before continuing."), 
        _("Warning"), wxOK|wxICON_INFORMATION);    
      contwarning.ShowModal();        
    }
  }
  else
  {
    wxMessageDialog opwarning(this, 
      _("Please load a file before continuing."), 
      _("Warning"), wxOK|wxICON_INFORMATION);    
    opwarning.ShowModal();    
  }
}


void MyFrame::OnSpin(wxSpinEvent &event)
  // Event handler for the spin control
{
  if(spin->GetValue()>maxcycles)
  {
    spin->SetValue(50);
    wxMessageDialog spinwarning(this, 
      _("The maximum number of cycles per run is 50"), 
      _("Warning"), wxOK|wxICON_INFORMATION);    
    spinwarning.ShowModal(); 
  }
  
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
  if(switchChoice->IsEmpty())
  {
    wxMessageDialog switchwarning(this, 
      _("No switch is available.\nPlease load a file first."), 
      _("Warning"), wxICON_INFORMATION | wxOK);
    switchwarning.ShowModal();   
  }
  else
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
        _(" is set to 0.\n"));
      }
      else
      {
        logMessagePanel->AppendText(
        getCurrentTime()+
        switchVec[currentSwitchIndex].objName +
        _(" is not set to 0.\n"));
      }
    }
  }
}

void MyFrame::OnCheck1(wxCommandEvent &event)
{
  if(switchChoice->IsEmpty())
  {
    wxMessageDialog switchwarning(this, 
      _("No switch is available.\nPlease load a file first."), 
      _("Warning"), wxICON_INFORMATION | wxOK);
    switchwarning.ShowModal();   
  }
  else
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
        _(" is set to 1.\n"));
      }
      else
      {
        logMessagePanel->AppendText(
        getCurrentTime()+
        switchVec[currentSwitchIndex].objName +
        _(" is not set to 1.\n"));     
      }
    }  
  }
}

void MyFrame::OnButtonSET(wxCommandEvent& event)
{
  if(IsStarted)
  {
    wxMessageDialog monitorwarning(this, 
    _("Please do not alter monitor settings while running.\nPlease press stop button first."), 
    _("Warning"), wxICON_INFORMATION | wxOK);
    monitorwarning.ShowModal();
  }
  else
  {
    if(monitorSet->IsEmpty())
    {
      wxMessageDialog monitorwarning(this, 
        _("No monitor is available to be set.\nPlease load a file first."), 
        _("Warning"), wxICON_INFORMATION | wxOK);
        monitorwarning.ShowModal();
      logMessagePanel->AppendText(
      getCurrentTime()+
      _("No monitor is available to be set.\n"));
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
          _("Monitor ")+
          monitorSet->GetStringSelection() + 
          _(" is set.\n"));
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
        monitorSet->SetSelection(0);
        monitorZap->SetSelection(0);
      }
      else
      {
        logMessagePanel->AppendText(
          getCurrentTime()+
          _("Monitor ")+
          monitorSet->GetStringSelection() + 
          _(" is not set successfully.\n"));
          if(mmz->moncount()==10)
          {
            wxMessageDialog monitorwarning(this, 
              _("Maximun number of monitors reached.\nPlease zap monitor point(s) before setting."), 
              _("Warning"), wxICON_INFORMATION | wxOK);
              monitorwarning.ShowModal();
          }
      }
    }
  }
}

void MyFrame::OnButtonZAP(wxCommandEvent& event)
{
  if(IsStarted)
  {
    wxMessageDialog monitorwarning(this, 
    _("Please do not alter monitor settings while running.\nPlease press stop button first."), 
    _("Warning"), wxICON_INFORMATION | wxOK);
    monitorwarning.ShowModal();
  }
  else
  {
    if(monitorZap->IsEmpty())
    {
      wxMessageDialog monitorwarning(this, 
        _("No monitor is available to be zapped.\nPlease load a file first."), 
        _("Warning"), wxICON_INFORMATION | wxOK);
        monitorwarning.ShowModal();
      logMessagePanel->AppendText(
      getCurrentTime()+
      _("No monitor is available to be zapped.\n"));
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
          _("Monitor ")+
          monitorZap->GetStringSelection() + 
          _(" is zapped.\n"));
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
		  monitorZap->SetSelection(0);
		  monitorSet->SetSelection(0);
      }
      else
      {
        logMessagePanel->AppendText(
          getCurrentTime()+
          _("Monitor ")+
          monitorZap->GetStringSelection() + 
          _(" is not zapped successfully.\n"));
      }
    }
  }
}


void MyFrame::loadFile(wxString s)
{
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
  IsStarted = 0;
  logMessagePanel->Clear();
  filePathBox->Clear();
  filePathBox->AppendText(s);
  ifstream f((filePath.ToStdString()).c_str());
  if(f.good())
  {
    string extString = (filePath.ToStdString()).substr((filePath.ToStdString()).length() - 4, (filePath.ToStdString()).length());
    if(extString == ".gf2")
    {
    // nmz, netz, dmz, mmz reinitialised here to allow load/reset
    // todo: maybe not necessary to pass them as arguments?
      smz = new scanner(nmz, filePath);
      pmz = new parser(netz, dmz, mmz, smz, nmz);
      std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());
      if(!(pmz->readin()))
      {// check if the definition file is valid
        logMessagePanel->AppendText(
          getCurrentTime()+
          _("Logic Definition File Error.\n"));
        
        std::string text = buffer.str(); // text will now contain "Bla\n"
        wxMessageDialog errorwarning(this, 
          _(text), 
          _("Definition File Error"), wxICON_INFORMATION | wxOK);

        errorwarning.ShowModal();
          
          // /* --- Open gedit to edit to file --- */
          // string str = "gedit " + filePath.ToStdString();
          // const char *command = str.c_str();
          // system(command);
          // /* ---------------------------------- */
      }  
      else
      {
        logMessagePanel->AppendText(
          getCurrentTime()+
          _("File loaded from ")+
          s + _(" successfully.\n"));
          FileLoaded = 1;
        std::vector<MyChoiceObj> signalList;

        devlink currentDevice = netz->devicelist();
        name currentID;
        int maxClockCycle;
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
            tempObj.kind = currentDevice->kind;
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
              tempObj.kind = currentDevice->kind;
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
                tempObj.kind = currentDevice->kind;
                signalList.push_back(tempObj);
                currentOutput = currentOutput->next;
              }
            }
          }
          currentDevice = currentDevice->next;
        }

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
        switchChoice->SetSelection(0);
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
        monitorSet->SetSelection(0);
        monitorZap->SetSelection(0);
      }
    }
    else
    {
      wxMessageDialog filewarning(this, 
        _("Please load a valid file with .gf2 extension."), 
        _("Warning"), wxICON_INFORMATION | wxOK);
      filewarning.ShowModal();
    }
  }
  else
  {
    wxMessageDialog filewarning(this, 
      _("The file does not exist.\nPlease load a valid file."), 
      _("Warning"), wxICON_INFORMATION | wxOK);
      filewarning.ShowModal();
    logMessagePanel->AppendText(
      getCurrentTime()+
      _("File does not exist.\n"));
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
    if (ok) 
    {
      n--;
      mmz->recordsignals ();
    } 
    else
    {
      wxMessageDialog filewarning(this, 
        _("Network is oscillating" ), 
        _("Warning"), wxICON_INFORMATION | wxOK);
        filewarning.ShowModal();
      logMessagePanel->AppendText(
        getCurrentTime()+
        _("Warning: network is oscillating.\n"));
    }
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
