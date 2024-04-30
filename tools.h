/*tools.h
* Killian Hinard et Rémi Oddon*/
/* Figure 11 b1 */

#ifndef TOOLS_H
#define TOOLS_H

#include "graphic_partial.h"

struct S2d; //vecteur et points

//permettent de rendre le code plus lisible
typedef S2d Point;
typedef S2d Vecteur;

struct S2d //vecteur et points
{
    double x;
    double y;
};

struct Cercle
{
    Point centre;
    double rayon;
};

struct Segment
{
    Point p;
    Vecteur v;
};

//si on ne précise pas de deuxieme point on calcule la norme d'un vect de coordonée p1
double calcDist(const Point& p1, const Point& p2 = {0,0});
//calcule la variation de distance dans le sens du rayon d'un cercle entre deux points
double calcVarRayon(Point p1, Point p2, Point centre);
Vecteur calcVecteur(const Point& p1, const Point& p2);
Segment creerSeg(const Point& p1, const Point& p2);
double produitScalaire(const Vecteur& v1, const Vecteur& v2);
Vecteur projete(const Vecteur& v1, const Vecteur& v2);//projete v1 sur v2

bool pointDansCercle(const Cercle c, const Point p);
bool interCercle(const Cercle& c1, const Cercle& c2, const double dist_min = 0);
bool interCrlSeg(const Cercle& c,const Segment& seg, const double dist_min = 0);


void tools_changerCouleur(Couleur couleur);
void afficherCercle(int const lesser, double const dim, Cercle cercle);
void afficherSeg(int const lesser, double const dim, Point p1, Point p2);
void afficherRectangle(int const lesser, double const dim, Cercle cercle);
void afficherCroix(int const lesser, double const dim, Cercle cercle);

#endif
