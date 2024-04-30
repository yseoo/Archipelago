/*gui.cc
 * Killian Hinard et Rémi Oddon*/
/* Figure 11 b1 */

#include <gtkmm/filechooserdialog.h>
#include <iostream>
#include <sstream>
#include "gui.h"
#include "ville.h"
#include "constantes.h"
#include "graphic_complete.h"

using namespace std;

enum BoutonSouris{BS_GAUCHE=1, BS_DROIT=3};

constexpr double default_zoom(1.0);

Dessin::Dessin()
:zoom(default_zoom) {}

void Dessin::actualisationDessin()
{
  auto win = get_window();
  if (win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), 
						   get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

double Dessin::set_zoom(double deltaZoom)
{
	if((deltaZoom<0 and min_zoom+.1<zoom) or (deltaZoom>0 and zoom<max_zoom))
		zoom+=deltaZoom;
	return zoom;
}

double Dessin::reinit_zoom()
{
	zoom=default_zoom;
	return zoom;
}

double Dessin::get_zoom() const
{
	return zoom;
}

bool Dessin::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	const int lesser((width<height)? width:height);
	
	cr->set_line_width(3);
	// joint les extrémités de segment en un carré
	cr->set_line_cap(Cairo::LINE_CAP_SQUARE);
	
	graphic_set_context(cr);
	
	graphic_changerCouleur(BLANC);
	cr->paint();
	
	graphic_changerCouleur(NOIR);
	dessinerVille(lesser,dim_max/zoom);
	cr->stroke();
	return true;
}

Fenetre::Fenetre()
:btnRadio('L'),
 boiteFenetre(false, 10),
 boiteMenu(false, 5),
 boiteGeneral(Gtk::BUTTONBOX_CENTER, 2), boiteDisplay(Gtk::BUTTONBOX_CENTER, 2), 
 boiteEditor(Gtk::BUTTONBOX_CENTER, 2), boiteInformations(Gtk::BUTTONBOX_CENTER, 2),
 frameGeneral("General"), frameDisplay("Display"), frameEditor("Editor"),
 frameInformations("Informations"),
 boutonExit("exit"), boutonNew("new"), boutonOpen("open"), boutonSave("save"),
 boutonZoomIn("zoom in"), boutonZoomOut("zoom out"), boutonZoomReset("zoom reset"),
 toggleShortestPath("shortest path"), toggleEditLink("edit link"),
 boutonRadioHousing(groupe, "housing"), boutonRadioTransport(groupe, "transport"),
 boutonRadioProduction(groupe, "production"),labelZoom("zoom:")
{
	majCriteres();
	set_title("Archipellago");
	signalZoomReset(); // met zoom : x1 dans le menu
	
	frameGeneral.add(boiteGeneral);
    boiteGeneral.pack_start(boutonExit);
    boiteGeneral.pack_start(boutonNew);
    boiteGeneral.pack_start(boutonOpen);
    boiteGeneral.pack_start(boutonSave);
    
    /////
    frameDisplay.add(boiteDisplay);
    boiteDisplay.pack_start(toggleShortestPath);
    boiteDisplay.pack_start(boutonZoomIn);
    boiteDisplay.pack_start(boutonZoomOut);
    boiteDisplay.pack_start(boutonZoomReset);
    boiteDisplay.pack_start(labelZoom);
    
    /////
    frameEditor.add(boiteEditor);
    boiteEditor.pack_start(toggleEditLink);
    boiteEditor.pack_start(boutonRadioHousing);
    boiteEditor.pack_start(boutonRadioTransport);
    boiteEditor.pack_start(boutonRadioProduction);
    
    /////
    frameInformations.add(boiteInformations);
    boiteInformations.pack_start(labelENJ);
    boiteInformations.pack_start(labelCI);
    boiteInformations.pack_start(labelMTA);
    
    /////
    boiteMenu.pack_start(frameGeneral, Gtk::PACK_SHRINK);
    boiteMenu.pack_start(frameDisplay, Gtk::PACK_SHRINK);
    boiteMenu.pack_start(frameEditor, Gtk::PACK_SHRINK);
    boiteMenu.pack_start(frameInformations, Gtk::PACK_SHRINK);
    
    /////
    dessin.set_size_request(default_drawing_size, default_drawing_size);
    
    /////
    boiteFenetre.pack_start(boiteMenu, Gtk::PACK_SHRINK);
    boiteFenetre.pack_start(dessin);
    add(boiteFenetre);
    show_all_children();
    
    ///// Signal
    boutonExit.signal_clicked().connect([this]() {signalExit();});
    boutonOpen.signal_clicked().connect([this]() {signalOpen();});
    boutonSave.signal_clicked().connect([this]() {signalSave();});
    boutonNew.signal_clicked().connect([this]() {signalNew();});
    
    toggleShortestPath.signal_clicked().connect([this]() {signalShortestPath();});
    boutonZoomIn.signal_clicked().connect([this]() {signalZoomIn();});
    boutonZoomOut.signal_clicked().connect([this]() {signalZoomOut();});
    boutonZoomReset.signal_clicked().connect([this]() {signalZoomReset();});
    
    toggleEditLink.signal_clicked().connect([this]() {signalEditLink();});
    boutonRadioHousing.signal_clicked().connect([this]() {signalHousing();});
    boutonRadioTransport.signal_clicked().connect([this]() {signalTransport();});
    boutonRadioProduction.signal_clicked().connect([this]() {signalProduction();});
    
    boutonRadioHousing.set_active(true);
}

Fenetre::~Fenetre()
{
}

void Fenetre::signalExit()
{
	hide();
}

void Fenetre::signalNew()
{
	reinitialisation();
	majCriteres();
	dessin.actualisationDessin();
	resetTgglBtn();
}

void Fenetre::signalOpen()
{
	Gtk::FileChooserDialog fenetreOpen(*this,"Please choose a file",
		Gtk::FILE_CHOOSER_ACTION_OPEN);

	fenetreOpen.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	fenetreOpen.add_button("_Open", Gtk::RESPONSE_OK);

	int result(fenetreOpen.run());
  
    if(result==Gtk::RESPONSE_OK)
    {
		reinitialisation();
		string filename = fenetreOpen.get_filename();
		if(!lecture_fichier(filename))
			reinitialisation();
		majCriteres();
		signalZoomReset();
		resetTgglBtn();
	}
}

void Fenetre::signalSave()
{
	Gtk::FileChooserDialog fenetreSave(*this,"Please choose a file",
		Gtk::FILE_CHOOSER_ACTION_SAVE);
	
	fenetreSave.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	fenetreSave.add_button("_Save", Gtk::RESPONSE_OK);
	
	int result(fenetreSave.run());
	if(result==Gtk::RESPONSE_OK)
	{
		string fileName(fenetreSave.get_filename());
		sauvegarde(fileName);
	}
}

void Fenetre::signalShortestPath()
{
	changeShortestPath(toggleShortestPath.get_active());
	dessin.actualisationDessin();
}

void Fenetre::signalZoomIn()
{
	actualisationZoom(dessin.set_zoom(delta_zoom));
}

void Fenetre::signalZoomOut()
{
	actualisationZoom(dessin.set_zoom(-delta_zoom));
}

void Fenetre::signalZoomReset()
{
	actualisationZoom(dessin.reinit_zoom());
}

void Fenetre::signalEditLink()
{
	if(!toggleEditLink.get_active() || !checkEditLink())
		toggleEditLink.set_active(false);
}

void Fenetre::signalHousing()
{
	btnRadio = 'L';
}

void Fenetre::signalTransport()
{
	btnRadio = 'T';
}

void Fenetre::signalProduction()
{
	btnRadio = 'P';
}

// Signal handler Souris
bool Fenetre::on_button_press_event(GdkEventButton * event)
{
	if(event->type == GDK_BUTTON_PRESS)
	{
		double xd(0), yd(0), xm(0), ym(0);
		double width = dessin.get_allocation().get_width();
		double height= dessin.get_allocation().get_height();
		double lesser((width<height)? width:height);
		
		double dim(dim_max/dessin.get_zoom());
	
		conversionDessinModele(event,lesser, dim, xd, yd, xm, ym);
		
		if(xd>=0 and xd<=width and yd>=0 and yd<=height)
		{
			if(event->button==BS_GAUCHE)
			{
				setPressPoint(xm, ym);
				dejaTraite=action1temps(xm,ym, toggleEditLink.get_active(), btnRadio);
			}
			else if(event->button==BS_DROIT)
				deplacer(xm, ym);
			
			dessin.actualisationDessin();
			majCriteres();
		}
	}
	return true;
}

bool Fenetre::on_button_release_event(GdkEventButton * event)
{
	if(event->type == GDK_BUTTON_RELEASE)
	{
		double xd(0), yd(0), xm(0), ym(0);
		double width = dessin.get_allocation().get_width();
		double height= dessin.get_allocation().get_height();
		double lesser((width<height)? width:height);
		
		double dim(dim_max/dessin.get_zoom());
	
		conversionDessinModele(event,lesser, dim, xd, yd, xm, ym);
		
		if(xd>=0 and xd<=width and yd>=0 and yd<=height)
		{
			if(event->button==BS_GAUCHE && !dejaTraite && !toggleEditLink.get_active())
			{
				action2temps(xm_press, ym_press, xm, ym);
				dessin.actualisationDessin();
				majCriteres();
			}
		}
	}
	return true;
}

// Signal handler clavier
bool Fenetre::on_key_press_event(GdkEventKey * key_event)
{
	if(key_event->type == GDK_KEY_PRESS)
	{
		switch(gdk_keyval_to_unicode(key_event->keyval))
		{
			case 'i':
				signalZoomIn();
				break;
			case 'o':
				signalZoomOut();
				break;
			case 'r':
				signalZoomReset();
				break;
		}
	}
	return Gtk::Window::on_key_press_event(key_event);
}

void Fenetre::conversionDessinModele(GdkEventButton const* event,double const& lesser,
									 double const& dim, double& xd, double& yd, 
									 double& xm, double& ym)
{
	double xf = event->x ;
	double yf = event->y ;
	
	double origine_x = dessin.get_allocation().get_x();
	double origine_y = dessin.get_allocation().get_y();
	
	xd=xf-origine_x;
	yd=yf-origine_y;
	xm=(xd/lesser)*2*dim-dim;
	ym=dim-(yd/lesser)*2*dim;
}

void Fenetre::setPressPoint(double const& xm, double const& ym)
{
	xm_press=xm;
	ym_press=ym;
}

void Fenetre::majCriteres()
{
	double enj,ci,mta;
	ostringstream flowFormatage;
	
	calculCritere(enj,ci,mta);
	
	labelENJ.set_text("ENJ : " + to_string(enj));
	
	flowFormatage<<ci;
	labelCI.set_text("CI : " + flowFormatage.str());
	flowFormatage.str("");//on vide le flow ou plutot on lui met le contenu ""
	
	flowFormatage<<mta;
	labelMTA.set_text("MTA : " + flowFormatage.str());
}

void Fenetre::actualisationZoom(double newZoom)
{
	ostringstream flowFormatage;
	flowFormatage<<newZoom;
	labelZoom.set_text("zoom: x"+flowFormatage.str());
	dessin.actualisationDessin();
}

void Fenetre::resetTgglBtn()
{
	toggleShortestPath.set_active(false);
	toggleEditLink.set_active(false);
}
