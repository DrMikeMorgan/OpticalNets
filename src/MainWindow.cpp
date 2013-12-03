#include "../include/MainWindow.h"
static inline void btnCreate_cb(Fl_Widget* w,void* data)
{
    MainWindow * win = (MainWindow*) data;
    win->makeGraph();
}

static inline void btnSRGs_cb(Fl_Widget* w,void* data)
{
    MainWindow * win = (MainWindow*) data;
    win->buildSRG();
}

MainWindow::MainWindow(int w, int h,const char * title):Fl_Window(w,h,title),o(0),g(0)
{
    begin();
    btnCreate = new Fl_Button(160, 5, 50, 20, "Create");
    btnCreate->callback(btnCreate_cb, this);
    btnSRGs = new Fl_Button(160, 30, 80, 20, "Lightpaths");
    btnSRGs->callback(btnSRGs_cb, this);
    txtNodes = new Fl_Int_Input(55, 5, 30, 20, "Nodes");
    txtNodes->value("50");
    txtMTD = new Fl_Float_Input(125, 5, 30, 20, "MTD");
    txtMTD->value("0.4");
    txtProb = new Fl_Float_Input(55, 30, 30, 20, "Prob");
    txtProb->value("0.8");
    txtMED = new Fl_Float_Input(125, 30, 30, 20, "MED");
    txtMED->value("0.2");
    GWindow = new GraphWindow(5,60,w-10,h-65);
    end();
    resizable(this);
    show();
}

void MainWindow::makeGraph()
{
    if(o)
        delete o;
    o = new OptNet(atoi(txtNodes->value()),(double) atof(txtMTD->value()));


    for(int i=0; i<o->size(); ++i)
        for(int j=i+1; j<o->size(); ++j)
            if(float(std::rand())/RAND_MAX < atof(txtProb->value()) && o->checkDistance(i,j, (double) atof(txtMED->value())))
                o->AddEdge(i,j,o->getDistance(i,j));
    GWindow->SetGraph(*o);
    GWindow->SRGrendering(false);
    GWindow->redraw();
}

void MainWindow::buildSRG()
{
    if(g)
        delete g;
    g = new SRGGraph(o->size());
    o->GetSRGs(*g);
    GWindow->SetGraph(*g);
    GWindow->SRGrendering(true);
    GWindow->redraw();
}

MainWindow::~MainWindow()
{
    if(o)
        delete o;
    if(g)
        delete g;
}
