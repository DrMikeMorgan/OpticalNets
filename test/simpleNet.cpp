#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include "include/OptNet.h"
#include "include/SRGGraph.h"
#include "include/GraphWindow.h"



// MAIN
int main() {
    //make graph...
    OptNet g(9,20);
    SRGGraph gdash(9);
    g.AddEdge(0,1,18);
    g.AddEdge(0,2,13);
    g.AddEdge(1,3,10);
    g.AddEdge(2,3,7);
    g.AddEdge(2,5,8);
    g.AddEdge(3,4,3);
    g.AddEdge(3,6,15);
    g.AddEdge(4,6,4);
    g.AddEdge(5,6,1);
    g.AddEdge(5,7,19);
    g.AddEdge(6,8,14);
    g.AddEdge(7,8,2);
    g.GetSRGs(gdash);


     Fl_Window win(500, 300, "Graphs R Us");
     GraphWindow mygl(gdash, 30, 30, win.w()-60, win.h()-60);
     win.end();
     win.resizable(mygl);
     win.show();
     return(Fl::run());
}
