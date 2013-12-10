#include "../include/MainWindow.h"
#include "../include/Construction.h"

void btnCreate_cb(Fl_Widget* w,void* data)
{
    MainWindow * win = (MainWindow*) data;
    win->makeGraph();
}

void btnSRGs_cb(Fl_Widget* w,void* data)
{
    MainWindow * win = (MainWindow*) data;
    win->buildSRG();
}

void btnRelays_cb(Fl_Widget* w,void* data)
{
    MainWindow * win = (MainWindow*) data;
    win->displayRelays();
}

void btnRand_cb(Fl_Widget * w, void * data)
{
    MainWindow * win = (MainWindow*) data;
    win->randomise();
}

void btnDrop_cb(Fl_Widget * w, void * data)
{
    MainWindow * win = (MainWindow*) data;
    win->drop();
}

MainWindow::MainWindow(int w, int h,const char * title):Fl_Window(w,h,title),o(0),g(0)
{
    begin();
    btnCreate = new Fl_Button(160, 5, 50, 20, "Create");
    btnCreate->callback(btnCreate_cb, this);
    btnSRGs = new Fl_Button(160, 30, 80, 20, "Lightpaths");
    btnSRGs->callback(btnSRGs_cb, this);
    txtNodes = new Fl_Int_Input(55, 5, 30, 20, "Nodes");
    txtNodes->value("200");
    txtMTD = new Fl_Float_Input(125, 5, 30, 20, "MTD");
    txtMTD->value("0.25");
    txtProb = new Fl_Float_Input(55, 30, 30, 20, "Prob");
    txtProb->value("0.8");
    txtMED = new Fl_Float_Input(125, 30, 30, 20, "MED");
    txtMED->value("0.13");
    btnRelays = new Fl_Light_Button(300, 5, 95, 20, "Relays only" );
    btnRelays->callback(btnRelays_cb, this);
    btnRelays->type(FL_TOGGLE_BUTTON);
    btnRelays->clear();
    btnRand = new Fl_Button(300,30,80,20,"Randomize");
    btnRand->callback(btnRand_cb,this);
    btnDrop = new Fl_Button(400,30,50,20, "Drop");
    btnDrop->callback(btnDrop_cb,this);
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

    relays.clear();
    relays.resize(o->size(),true);

    for(int i=0; i<o->size(); ++i)
        for(int j=i+1; j<o->size(); ++j)
            if(float(std::rand())/RAND_MAX < atof(txtProb->value()) && o->checkDistance(i,j, (double) atof(txtMED->value())))
                o->AddEdge(i,j,o->getDistance(i,j));
    GWindow->setGraph(*o);
    GWindow->SRGrendering(false);
    GWindow->redraw();
}

void MainWindow::buildSRG()
{
    if(g)
        delete g;
    g = new SRGGraph(o->size());
    o->GetSRGs(*g);
    GWindow->setGraph(*g);
    GWindow->SRGrendering(true);SRGGraph *problem;
    GWindow->redraw();
}

void MainWindow::displayRelays()
{
    if(btnRelays->value()==1)
        GWindow->setRelays(&relays);
    else
        GWindow->resetRelays();//if connected, --numRels - update relDegree
    GWindow->redraw();
}

void MainWindow::randomise()
{
    for(int i=0; i<relays.size(); ++i)
        if(rand()%2 == 0)
            relays[i] = false;
        else
            relays[i] = true;
}

void MainWindow::drop()
{
    dropAlgorithm(*g,relays);
    GWindow->redraw();
}

MainWindow::~MainWindow()
{
    if(o)
        delete o;
    if(g)
        delete g;
}
