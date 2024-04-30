/*gui.h
 * Killian Hinard et Rémi Oddon*/
/* Figure 11 b1 */

#ifndef GUI_H
#define GUI_H


#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/button.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/box.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>

class Dessin:public Gtk::DrawingArea
{
public:
	Dessin();
	void actualisationDessin();
	double set_zoom(double deltaZoom);
	double reinit_zoom();
	double get_zoom() const;
protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
private:
	double zoom;
};

class Fenetre:public Gtk::Window
{
public:
	Fenetre();
	~Fenetre();
	
private:

	//attribut servant à savoir si le clic gauche dans la fenetre à déjà été traité ou 
	//si il fait partie d'un action en deux temps
	bool dejaTraite;
	
	char btnRadio;
	
	// Signal handler
	void signalExit();
	void signalNew();
	void signalOpen();
	void signalSave();
	
	void signalShortestPath();
	void signalZoomIn();
	void signalZoomOut();
	void signalZoomReset();
	
	void signalEditLink();
	void signalHousing();
	void signalTransport();
	void signalProduction();
	
	// Events
	bool on_button_press_event(GdkEventButton * event);
	bool on_button_release_event(GdkEventButton * event);
	bool on_key_press_event(GdkEventKey * key_event);
	
	void conversionDessinModele(GdkEventButton const* event,double const& lesser, 
								double const& dim, double& xd, double& yd, double& xm,
								double& ym);
	
	void setPressPoint(double const& x, double const& y);
	double xm_press, ym_press;
	
	void majCriteres();
	void actualisationZoom(double newZoom);
	void resetTgglBtn();
	
	
	Gtk::HBox boiteFenetre;
	Gtk::VBox boiteMenu, boiteGeneral, boiteDisplay, boiteEditor, boiteInformations;
	Gtk::Frame frameGeneral, frameDisplay, frameEditor, frameInformations;
	Gtk::Button boutonExit, boutonNew, boutonOpen, boutonSave, boutonZoomIn, 
				boutonZoomOut, boutonZoomReset;
	Gtk::ToggleButton toggleShortestPath, toggleEditLink;
	Gtk::RadioButtonGroup groupe;
	Gtk::RadioButton boutonRadioHousing, boutonRadioTransport, boutonRadioProduction;
	Gtk::Label labelZoom, labelENJ, labelCI, labelMTA;
	
	Dessin dessin;
};

#endif
