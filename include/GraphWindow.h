#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include "OptNet.h"
#include "SRGGraph.h"

namespace mikeNets{

	class GraphWindow : public Fl_Gl_Window {
		OptNet * o;
		SRGGraph * g;
		GLfloat * vertices, *keyVertices;
		GLfloat * colours;
		GLuint *edges, *nodes, *opedges;
		GLuint m,n,opm;
		std::vector<bool> * relays;
		bool renderSRGs, renderRelays;
		size_t selectedNode, srg;
		void draw();
		void resize(int X,int Y,int W,int H);
		virtual int handle(int event);
	public:
		GraphWindow(int X,int Y,int W,int H,const char*L=0);
		void setGraph(SRGGraph& g);
		void setGraph(OptNet& op);
		void setRelays(std::vector<bool> * relays){this->relays = relays; renderRelays = true;}
		void resetRelays(){renderRelays = false;}
		void SRGrendering(bool b){renderSRGs = b;}
		~GraphWindow();
	};

}

#endif // GRAPHWINDOW_H
