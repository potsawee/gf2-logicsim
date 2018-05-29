#include "logsim.h"
#include "userint.h"
#include "gui.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define USE_GUI

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
  // This function is automatically called when the application starts
{

  // Construct the six classes required by the innards of the logic simulator
  nmz = new names();
  netz = new network(nmz);
  dmz = new devices(nmz, netz);
  mmz = new monitor(nmz, netz);

#ifdef USE_GUI
    // glutInit cannot cope with Unicode command line arguments, so we pass
    // it some fake ASCII ones instead
    char **tmp1; int tmp2 = 0; glutInit(&tmp2, tmp1);
    // Construct the GUI
    MyFrame *frame = new MyFrame(
      NULL, 
      "Logic simulator", 
      wxDefaultPosition,  
      wxSize(800, 600), 
      nmz, dmz, mmz, netz);
    frame->Show(true);
    return(true); // enter the GUI event loop
#else
  if (argc != 2) { // check we have one command line argument
    wcout << "Usage:      " << argv[0] << " [filename]" << endl;
    exit(1);
  }
  smz = new scanner(nmz, wxString(argv[1]).mb_str());
  pmz = new parser(netz, dmz, mmz, smz, nmz);
  if (pmz->readin ()) { // check the logic file parsed correctly
    // Construct the text-based interface
    userint umz(nmz, dmz, mmz);
    umz.userinterface();
  }
#endif /* USE_GUI */

  return(false); // exit the application
}
