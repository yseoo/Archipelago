/*ville.cc 
 * Killian Hinard et Rémi Oddon*/
/* Figure 11 b1 */

#include <iostream>
#include <sstream>
#include <fstream>
#include "ville.h"
#include "noeud.h"

using namespace std;

enum EtatLecture{NB, LIGNE};

class Ville
{
public:
    Ville() {}
	bool ligneNoeud(string ligne, int total, int& compteur, Type& type, 
				EtatLecture& etat);
	bool ligneLien(string ligne, int total, int& compteur);
	void changeType(Type& type);
};

namespace
{
	bool shortestPathActif;
	Ville ville;
}

bool lecture_fichier(string nom_fichier)
{
	ifstream fichier(nom_fichier);
	if(fichier.fail())
	{
		cout<<"error opening file"<<endl;
		return false;
	}
		
    string ligne;
    
    Type type(LOGEMENT);
    EtatLecture etat(NB);
    
    int compteur(0), total(0);

    while(getline(fichier>>ws, ligne))
    {
        if(ligne[0]=='#')
            continue;
        
        switch(etat)
        {
            case NB:
            {
                istringstream data(ligne);
                data>>total;
                if(total>0)
					etat=LIGNE;
				if(total==0)
					ville.changeType(type);
                compteur=0;
                break;
            }
            case LIGNE:
                switch(type)
                {
                    case LOGEMENT:
						if(!ville.ligneNoeud(ligne,total,compteur,type,etat))
							return false;
						break;
                    case TRANSPORT:
						if(!ville.ligneNoeud(ligne,total,compteur,type,etat))
							return false;
						break;
                    case PRODUCTION:
						if(!ville.ligneNoeud(ligne,total,compteur,type,etat))
							return false;
                        break;
                    case LIEN:
                        if(!ville.ligneLien(ligne, total, compteur))
							return false;
                        break;
                }
                break;
        }
    }
    
    return true;
}

void dessinerVille(int const lesser, double const dim)
{		
	dessinerLiens(lesser, dim, shortestPathActif);
	dessinerNoeuds(lesser, dim, shortestPathActif);
}

void reinitialisation()
{
	clearDatabase();
}

void calculCritere(double& enj, double& ci, double& mta)
{
	enj=calculENJ();
	ci=calculCI();
	mta=calculMTA();
}

void sauvegarde(string fichier)
{
	vector<string> noeuds;
	vector<vector<int>> donneesLiens;
	ofstream flowFichier(fichier);
	Type etatSauvegarde(LOGEMENT);
	
	while(etatSauvegarde!=LIEN)
	{
		noeuds=sauvegardeNoeud(etatSauvegarde);
		flowFichier<<noeuds.size()<<endl;
		
		for(auto donnees:noeuds)
			flowFichier<<"\t"<<donnees<<endl;
		
		flowFichier<<endl;
		
		ville.changeType(etatSauvegarde);
	}
	
	donneesLiens = sauvegardeLien();
	flowFichier<<donneesLiens.size()<<endl;
	
	for(auto lien: donneesLiens)
	{
		flowFichier<<"\t"<<lien[0]<<" "<<lien[1]<<endl;
	}
}


bool action1temps(double x, double y, bool editLink, char type)
{
	if(editLink)
		return editionLien(x,y);
	
	Type t;
	switch(type)
	{
		case 'L':
			t=LOGEMENT;
			break;
		case 'T':
			t=TRANSPORT;
			break;
		case 'P':
			t=PRODUCTION;
			break;
	}
	
	return analyse1Temps(x,y,t);
}

void action2temps(double xdebut, double ydebut, double xfin, double yfin)
{
	analyse2Temps(xdebut,ydebut,xfin,yfin);
}

void deplacer(double x, double y)
{
	if(noeudSelectione())
		deplacerNoeud(x,y);
}

bool checkEditLink()
{
	if(getDatabaseSize() < 2)
	{
		cout<<"Erreur il n'y a pas assez de noeud pour creer des liens"<<endl;
		return false;
	}
	else if(!noeudSelectione())
	{
		cout<<"Erreur veuillez d'abord selectionner un noeud"<<endl;
		return false;
	}
	
	return true;
}

void changeShortestPath(bool shortestPath)
{
	shortestPathActif=shortestPath;
}

bool Ville::ligneNoeud(string ligne, int total, int& compteur, Type& type,
					   EtatLecture& etat)
{
	//appel de la fonction créant un noeud de type type
	if(!lectureNoeud(type,ligne))
		return false;
		
    ++compteur;

    if(compteur==total)
    {
        etat=NB;
        changeType(type);
    }
    
    return true;
}

bool Ville::ligneLien(string ligne, int total, int& compteur)
{
    if(compteur<total)
    {
		//appel de la fonction créant un lien.
        if(!lectureLien(ligne))
			return false;
			
        ++compteur;
    }
    
    return true;
}

void Ville::changeType(Type& type)
{
	switch(type)
        {
            case LOGEMENT:
                type=TRANSPORT;
                break;
            case TRANSPORT:
                type=PRODUCTION;
                break;
            case PRODUCTION:
                type=LIEN;
                break;
            case LIEN:
                break;
        }
}
