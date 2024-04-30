/*graphic_partial.h
* Rémi Oddon */
/* Figure 11 b1 */

#ifndef GRAPHIC_PARTIAL_H
#define GRAPHIC_PARTIAL_H

enum Couleur{BLANC, NOIR, ROUGE, VERT};

void graphic_changerCouleur(Couleur c);

void dessinerCercle(int const lesser, double const dim, double xm, double ym, 
					double rayonm);
void dessinerSegment(int const lesser, double const dim, double xm1, double ym1, 
					 double xm2,double ym2);
					 // trace un segment entre le point de départ (xm1, ym1) et 
					 // le point d'arrivée (xm2, ym2)

#endif 

