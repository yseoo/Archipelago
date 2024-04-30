/*graphic.cc
* Rémi Oddon */
/* Figure 11 b1 */

#include <cmath>
#include "graphic_complete.h"

static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr)
{
	static bool init(false);
	if(!init)
	{
		ptcr = &cr;
		init = true;
	}
}

void graphic_changerCouleur(Couleur couleur)
{
	switch(couleur)
	{
		case BLANC:
			(*ptcr)->set_source_rgb(1,1,1);
			break;
		case NOIR:
			(*ptcr)->set_source_rgb(0,0,0);
			break;
		case ROUGE:
			(*ptcr)->set_source_rgb(1,0,0);
			break;
		case VERT:
			(*ptcr)->set_source_rgb(0,1,0);
			break;
	}
}

void dessinerCercle(int const lesser, double const dim, double xm, double ym, 
					double rayonm)
{
	double xd(lesser*(xm+dim)/(2*dim));
	double yd(lesser*(dim-ym)/(2*dim));
	double rayond((rayonm*lesser)/(2*dim));
	
	(*ptcr)->arc(xd,yd,rayond,0,2*M_PI);
	(*ptcr)->save();
	graphic_changerCouleur(BLANC);
	(*ptcr)->fill_preserve();
	(*ptcr)->restore();
	(*ptcr)->stroke();
}

void dessinerSegment(int const lesser, double const dim, double xm1, double ym1, 
					 double xm2,double ym2)
{
	double xd1(lesser*(xm1+dim)/(2*dim));
	double yd1(lesser*(dim-ym1)/(2*dim));
	
	double xd2(lesser*(xm2+dim)/(2*dim));
	double yd2(lesser*(dim-ym2)/(2*dim));
	
	(*ptcr)->move_to(xd1, yd1);
	(*ptcr)->line_to(xd2, yd2);
	(*ptcr)->stroke();
}
