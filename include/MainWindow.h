#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Window.H>
#include <FL/Fl.H>

#include "OptNet.h"
#include "SRGGraph.h"
#include "GraphWindow.h"
#include "PavanGenerator.h"

namespace mikeNets
{

	class MainWindow : public Fl_Window
	{
		public:
		    MainWindow(int w, int h,const char * title = 0 );
		    virtual ~MainWindow();
		    //virtual int handle(int event);
		    Fl_Button * btnCreate, *btnSRGs, *btnRand, *btnDrop, *btnSA, *btnACO;
		    Fl_Output * lbRelays;
		    Fl_Int_Input * txtNodes;
		    Fl_Float_Input * txtMTD, * txtMED, * txtProb;
		    Fl_Light_Button * btnRelays;
		    void makeGraph();
		    void buildSRG();
		    void displayRelays();
		    void randomise();
		    void drop();
		    void SA();
		    void ACO();
		protected:
		GraphWindow * GWindow;
		OptNet * o;
		SRGGraph * g;
		mikeNets::PavanGenerator * p;
		size_t nodes;
		size_t MTD;
		std::vector<bool> relays;
		private:

	};
}

#endif // MAINWINDOW_H
