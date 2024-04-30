/*tools.cc
* Killian Hinard et Rémi Oddon*/
/* Figure 11 b1 */

#include <cmath>
#include "tools.h"

double calcDist(const Point& p1, const Point& p2)
{
    return sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2));
}

double calcVarRayon(Point p1, Point p2, Point centre)
{
	Vecteur vCtrP1(calcVecteur(centre,p1));
	Vecteur vCtrP2(calcVecteur(centre,p2));
	
	return calcDist(vCtrP2)-calcDist(vCtrP1);
}

Vecteur calcVecteur(const Point& p1, const Point& p2)
{
    Vecteur v;
    v.x=p2.x-p1.x;
    v.y=p2.y-p1.y;

    return v;
}

Segment creerSeg(const Point& p1, const Point& p2)
{
	Segment seg;
	
	seg.p = p1;
	seg.v = calcVecteur(p1,p2);
	
	return seg;
}

double produitScalaire(const Vecteur& v1, const Vecteur& v2)
{
    return v1.x*v2.x+v1.y*v2.y;
}

Vecteur projete(const Vecteur& v1, const Vecteur& v2)
{
    Vecteur proj;

    proj.x=(produitScalaire(v1,v2)/produitScalaire(v2,v2))*v2.x;
    proj.y=(produitScalaire(v1,v2)/produitScalaire(v2,v2))*v2.y;

    return proj;
}

bool pointDansCercle(const Cercle c, const Point p)
{
	if(calcDist(p,c.centre)<=c.rayon)
		return true;
	
	return false;
}

bool interCercle(const Cercle& c1, const Cercle& c2, const double dist_min)
{
    if(calcDist(c1.centre,c2.centre)<=c1.rayon+c2.rayon+dist_min)
        return true;

    return false;
}

bool interCrlSeg(const Cercle& c, const Segment& seg, const double dist_min)
{
	
    Vecteur vPointCentre(calcVecteur(seg.p,c.centre));
    
    if(calcDist(vPointCentre)>=calcDist(seg.v)||produitScalaire(seg.v,vPointCentre)<=0)
		return false;
    
    Vecteur vProj(projete(vPointCentre,seg.v));
    Vecteur vOrtho;

    vOrtho.x=vPointCentre.x-vProj.x;
    vOrtho.y=vPointCentre.y-vProj.y;

    if(calcDist(vOrtho)<=c.rayon+dist_min)
        return true;

    return false;
}

// Fonctions de dessin

void tools_changerCouleur(Couleur couleur)
{
	graphic_changerCouleur(couleur);
}

void afficherCercle(int const lesser, double const dim, Cercle cercle)
{
	dessinerCercle(lesser, dim, cercle.centre.x, cercle.centre.y, cercle.rayon);
}

void afficherSeg(int const lesser, double const dim, Point p1, Point p2)
{
	dessinerSegment(lesser, dim, p1.x, p1.y, p2.x, p2.y);
}

void afficherRectangle(int const lesser, double const dim, Cercle cercle)
{
	double xc(cercle.centre.x), yc(cercle.centre.y), rayon(cercle.rayon);
	double x1(xc-(3*rayon)/4), x2(xc+(3*rayon)/4);
	double y1(yc-rayon/8), y2(yc+rayon/8);
	
	dessinerSegment(lesser, dim, x1, y1, x1, y2);
	dessinerSegment(lesser, dim, x1, y2, x2, y2);
	dessinerSegment(lesser, dim, x2, y2, x2, y1);
	dessinerSegment(lesser, dim, x2, y1, x1, y1);
}

void afficherCroix(int const lesser, double const dim, Cercle cercle)
{
	double xc(cercle.centre.x), yc(cercle.centre.y), rayon(cercle.rayon);
	double x1(xc-rayon), x2(xc-(sqrt(2)*rayon)/2),
		x3(xc+(sqrt(2)*rayon)/2), x4(xc+rayon);
	double y1(yc-rayon), y2(yc-(sqrt(2)*rayon)/2),
		y3(yc+(sqrt(2)*rayon)/2), y4(yc+rayon);
		
		
	dessinerSegment(lesser, dim, xc, y1, xc, y4);
	dessinerSegment(lesser, dim, x2, y2, x3, y3);
	dessinerSegment(lesser, dim, x1, yc, x4, yc);
	dessinerSegment(lesser, dim, x2, y3, x3, y2);
}
