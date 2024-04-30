/*projet.cc
 * Rémi Oddon */
/* Figure 11 b1 */
 
#include <iostream>
#include "ville.h"
#include "gui.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc==2)
    {
		reinitialisation();
        if(!lecture_fichier(static_cast<string>(argv[1])))
			reinitialisation();
	}
    
    int argc1(1);
    auto app = Gtk::Application::create(argc1, argv);
    Fenetre fenetre;
	fenetre.set_resizable(true);
	fenetre.set_border_width(7);
    app->run(fenetre);
    
    return EXIT_SUCCESS;
}
