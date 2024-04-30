/*noeud.h
 * Killian Hinard*/
/* Figure 11 b1 */

#ifndef NOEUD_H
#define NOEUD_H

#include <string>
#include <vector>
#include "tools.h"
#include "constantes.h"

enum Type{LOGEMENT, TRANSPORT, PRODUCTION, LIEN};

class Noeud;

struct Djikstra
{
	double access;
	bool in;
	Noeud* parent;
};

class Noeud
{
protected :
	unsigned int uid;
	Cercle quartier;
	unsigned int nbp;//on se sert de cette variable pour stocker nbp plutot que le rayon de 
			//quartier pour avoir une plus grande précision
	std::vector<Noeud*> link;
	Djikstra attributsDjikstra;
	bool DSP;//DSP = dans shortest path

public :
	
	virtual ~Noeud();
	
	unsigned int getUid() const;
	Point getPosition() const;
	int getNbp() const;
	std::vector<Noeud*> getLink() const;
	bool getDjikstraIn() const;
	double getDjikstraAccess() const;
	Noeud* getDjikstraParent() const;
	bool getDSP() const;
	virtual Type getType() const = 0;
	
	bool setUid(unsigned int newUid);
	bool setPosition(Point pos, double distMin);
	void varNbp(double variation);
	bool setQuartier(Point centre, unsigned int nbp_, double distMin);
	bool addLink(Noeud* ptr, double distMin = 0);
	void supprLink(Noeud* nSuppr);
	void majLink(unsigned int uid);
	void setDjikstraIn(bool in_);
	void setDjikstraAccess(double access_);
	void setDjikstraParent(Noeud* parent_);
	void setShortestPath(std::vector<std::vector<int>>& liensTraites,int lesser, 
						 double dim);
	void reinitDSP();
	
	bool dansLien(Noeud* noeud);
	bool pointDansNoeud(const Point p) const;
	bool intersecNodeNode(const Noeud& n, double distMin) const;
	bool intersecLienNode(const Segment& lien, double distMin) const;
	bool verifieInterLien(Noeud* nLien, double distMin) const;
	bool verifieCollision(double distMin) const;
	
	virtual void dessiner(int const lesser, double const dim) const = 0;
};

class Logement : public Noeud
{
public :
	Type getType() const override;
	void dessiner(int const lesser, double const dim) const override;
};

class Production : public Noeud
{
public :
	Type getType() const override;
	void dessiner(int const lesser, double const dim) const override;
};

class Transport : public Noeud
{
public :
	Type getType() const override;
	void dessiner(int const lesser, double const dim) const override;
};

bool lectureNoeud(Type type, std::string info);
bool lectureLien(std::string info);
std::vector<std::string> sauvegardeNoeud(Type type);
std::vector<std::vector<int>> sauvegardeLien();
void clearDatabase();
bool noeudSelectione();//renvoie true si il y a un noeud sélectionné
size_t getDatabaseSize();
bool analyse1Temps(double x, double y, Type type);
void analyse2Temps(double xd, double yd, double xf, double yf);
void deplacerNoeud(double x, double y);
bool editionLien(double x, double y);

double calculENJ();
double calculCI();
double calculMTA();

void dessinerNoeuds(int const lesser, double const dim, bool shortestPath);
void dessinerLiens(int const lesser, double const dim, bool shortestPath);

#endif
