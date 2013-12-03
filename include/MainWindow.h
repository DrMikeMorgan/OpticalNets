#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Window.H>
#include <FL/Fl.H>

#include "OptNet.h"
#include "SRGGraph.h"
#include "GraphWindow.h"


class MainWindow : public Fl_Window
{
    public:
        MainWindow(int w, int h,const char * title = 0 );
        virtual ~MainWindow();
        //virtual int handle(int event);
        Fl_Button * btnCreate, *btnSRGs;
        Fl_Int_Input * txtNodes;
        Fl_Float_Input * txtMTD, * txtMED, * txtProb;
        void makeGraph();
        void buildSRG();
    protected:
    GraphWindow * GWindow;
    OptNet * o;
    SRGGraph * g;
    size_t nodes;
    size_t MTD;
    private:

};

#endif // MAINWINDOW_H
