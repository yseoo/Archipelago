/*noeud.cc
 * Killian Hinard*/
/* Figure 11 b1 */

#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "noeud.h"
#include "error.h"

using namespace std;

namespace
{
	enum EtatNoeud{UID, X, Y, NBP};
	enum EtatLien {L1, L2};
	
	bool getNoeud(Noeud* &ptrNoeud, unsigned int uid);
	bool ajouterNoeud(Type type, int uid, int nbp, Point centre, double distMin = 0);
	void deleteNoeud(Noeud* nDeleted);
	Noeud* plusCourtAccess(Noeud* nDepart, Type typeNoeudCherche);
	double calculAccess(Noeud* noeud1, Noeud* noeud2);
	bool sortTA(Noeud* ptrNoeud1, Noeud* ptrNoeud2);
	void initDjikstra(Noeud* nDepart);
	Noeud* getNoeudPointe(const Point p);
	unsigned int getUidLibre();
	void initShortestPath(vector<vector<int>>&liensTraites,int lesser,double dim);
	void clearShortestPath();
	
	vector<Noeud*> database;
	Noeud* noeudCourant;
}

Noeud::~Noeud()
{

}

unsigned int Noeud::getUid() const
{
	return uid;
}

Point Noeud::getPosition() const
{
	return quartier.centre;
}

int Noeud::getNbp() const
{
	return nbp;
}

std::vector<Noeud*> Noeud::getLink() const
{
	return link;
}

bool Noeud::getDjikstraIn() const
{
	return attributsDjikstra.in;
}

double Noeud::getDjikstraAccess() const
{
	return attributsDjikstra.access;
}

Noeud* Noeud::getDjikstraParent() const
{
	return attributsDjikstra.parent;
}

bool Noeud::getDSP() const
{
	return DSP;
}

bool Noeud::setUid(unsigned int newUid)
{
	if(newUid == no_link)
	{
		cout<<error::reserved_uid();
		return false;
	}

	for(auto ptrNoeud : database)
		if(newUid==ptrNoeud->uid)
		{	
			cout<<error::identical_uid(newUid);
			return false;
		}
					
	uid=newUid;
	return true;
}

bool Noeud::setPosition(Point nouvellePos, double distMin)
{
	//si le noeud n'avait pas de position enregistrée anciennePos sera incohérente
	//mais cela ne pose pas de problème dans le déroulement du programme
	Point anciennePos = quartier.centre;
	
	quartier.centre = nouvellePos;
	
	if(!verifieCollision(distMin))
	{
		quartier.centre = anciennePos;
		return false;
	}
	
	return true;
}

void Noeud::varNbp(double variation)
{
	double newRayon(quartier.rayon+variation);
	double oldRayon(quartier.rayon);
	
	if(newRayon < sqrt(min_capacity))
	{
		quartier.rayon = sqrt(min_capacity);
		nbp = min_capacity;
	}
	else if(newRayon > sqrt(max_capacity))
	{
		quartier.rayon=sqrt(max_capacity);
		
		if(!verifieCollision(dist_min))
			quartier.rayon=oldRayon;
		else
			nbp=max_capacity;
	}
	else
	{
		quartier.rayon=newRayon;
		
		if(!verifieCollision(dist_min))
			quartier.rayon=oldRayon;
		else
			nbp=newRayon*newRayon;
	}
}

bool Noeud::setQuartier(Point centre, unsigned int nbp_, double distMin)
{
	if(nbp_<min_capacity)
	{
		cout<<error::too_little_capacity(nbp_);
		return false;
	}
		
	else if(nbp_>max_capacity)
	{
		cout<<error::too_much_capacity(nbp_);
		return false;
	}
	
	nbp=nbp_;
	quartier.rayon = sqrt(nbp_);
	
	if(!setPosition(centre, distMin))
		return false;
			
	return true;
}

bool Noeud::addLink(Noeud* ptr, double distMin)
{
	if(ptr->uid==uid)
	{
		cout<<error::self_link_node(uid);
		return false;
	}
	else if(link.size()==max_link && getType() == LOGEMENT)
	{
		cout<<error::max_link(uid);
		return false;
	}
	else if(ptr->link.size()==max_link && ptr->getType() == LOGEMENT)
	{
		cout<<error::max_link(ptr->uid);
		return false;
	}
	for(auto voisin : link)
		if(voisin->uid==ptr->uid)
		{
			cout<<error::multiple_same_link(uid,ptr->uid);
			return false;
		}
		
	if(!verifieInterLien(ptr,distMin))
		return false;
	
	link.push_back(ptr);
	ptr->link.push_back(this);
	return true;
}

void Noeud::supprLink(Noeud* nSuppr)
{
	size_t pos;
	
	for(size_t i(0); i<link.size(); i++)
		if(nSuppr==link[i])
			pos = i;
			
	swap(link[pos],link[link.size()-1]);
	link.pop_back();
}

void Noeud::majLink(unsigned int uid)
{
	size_t pos;
	for(size_t i(0); i<link.size(); i++)
		if(link[i]->getUid()==uid)
			pos=i;
		
	swap(link[pos],link[link.size()-1]);
	link.pop_back();
}

void Noeud::setDjikstraIn(bool in_)
{
	attributsDjikstra.in=in_;
}

void Noeud::setDjikstraAccess(double access_)
{
	attributsDjikstra.access=access_;
}

void Noeud::setDjikstraParent(Noeud* parent_)
{
	attributsDjikstra.parent=parent_;
}

void Noeud::setShortestPath(vector<vector<int>>& liensTraites, int lesser, double dim)
{
	afficherSeg(lesser,dim,quartier.centre,attributsDjikstra.parent->quartier.centre);
	DSP=true;
	int uid1(uid), uid2(attributsDjikstra.parent->uid);
	
	liensTraites.push_back({uid1,uid2});
	
	if(attributsDjikstra.parent!=noeudCourant)
		attributsDjikstra.parent->setShortestPath(liensTraites,lesser,dim);
}

void Noeud::reinitDSP()
{
	DSP=false;
}

bool Noeud::dansLien(Noeud* noeud)
{
	for(auto nLien: link)
	 if(noeud == nLien)
		return true;
	
	return false;
}

bool Noeud::pointDansNoeud(const Point p) const
{
	if(pointDansCercle(quartier,p))
		return true;
		
	return false;
}

bool Noeud::intersecNodeNode(const Noeud& n, double distMin) const
{
	if(interCercle(quartier, n.quartier, distMin))
		return true;
	
	return false;
}

bool Noeud::intersecLienNode(const Segment& lien, double distMin) const
{
	if(interCrlSeg(quartier,lien))
		return true;
		
	return false;
}

bool Noeud::verifieInterLien(Noeud* nLien, double distMin) const
{
	Segment lien(creerSeg(quartier.centre,nLien->quartier.centre));
	
	for(auto noeud : database)
	{
		if(noeud->uid==uid || noeud->uid==nLien->uid)
			continue;	
		else if(noeud->intersecLienNode(lien,0))
		{
			cout<<error::node_link_superposition(noeud->uid);
			return false;
		}
	}
	return true;
}

bool Noeud::verifieCollision(double distMin) const
{
	vector<vector<int>> liensTraites;
	bool traite;
	int uid1,uid2;
	Segment lien;
	
	//vérifie les collisions avec les autres noeuds
	for(auto noeud: database)
		if(noeud->uid!=uid && intersecNodeNode(*noeud, distMin))
		{
			cout<<error::node_node_superposition(uid,noeud->uid);
			return false;
		}
	
	//vérifie les collisions avec des liens
	for(auto noeud:database)
	{
		uid1 = noeud->getUid();
		for(auto noeudLien: noeud->getLink())
		{
			uid2= noeudLien->getUid();
			traite=false;
			
			for(auto lien : liensTraites)
				if((uid1==lien[0] && uid2==lien[1])||(uid1==lien[1] && uid2==lien[0]))
					traite=true;
			
			if(!traite)
			{
				lien = creerSeg(noeud->getPosition(), noeudLien->getPosition());
				if(intersecLienNode(lien, distMin))
				{
					cout<<error::node_link_superposition(uid);
					return false;
				}
			}
		}
	}
	return true;
}		

Type Logement::getType() const
{
	return LOGEMENT;
}

void Logement::dessiner(int const lesser, double const dim) const
{
	afficherCercle(lesser,dim,quartier);
}

Type Production::getType() const
{
	return PRODUCTION;
}

void Production::dessiner(int const lesser, double const dim) const
{
	afficherCercle(lesser,dim,quartier);
	afficherRectangle(lesser,dim,quartier);
}

Type Transport::getType() const
{
	return TRANSPORT;
}

void Transport::dessiner(int const lesser, double const dim) const
{
	afficherCercle(lesser,dim,quartier);
	afficherCroix(lesser,dim,quartier);
}

bool lectureNoeud(Type type, string info)
{	
	istringstream streamInfo(info);
	string ligne;
	int uid, nbp;
	double x, y;
	bool fini(false);
	EtatNoeud etat(UID);
	
	while(streamInfo >> ligne && !fini)
	{
		switch(etat)
		{
			case UID:
				uid=stoul(ligne);
				etat=X;
				break;

			case X:
				x=stod(ligne);
				etat=Y;
				break;
			
			case Y:
				y=stod(ligne);
				etat=NBP;
				break;
			
			case NBP:
				nbp=stoul(ligne);
				fini=true; 
				break;				
		}
	}
	if(!ajouterNoeud(type,uid,nbp,{x,y}))
		return false;
	
	return true;
}

bool lectureLien(string info)
{
	Noeud* ptrN1(nullptr);
	Noeud* ptrN2(nullptr);
	
	istringstream streamInfo(info);
	string ligne;
	
	EtatLien etat(L1);
	
	bool fini(false);
	
	while(streamInfo>>ligne && !fini)
	{
		switch(etat)
		{
			case L1:
				if(!getNoeud(ptrN1,stoul(ligne)))
					return false;
				etat=L2;
				break;
			
			case L2:
				if(!getNoeud(ptrN2,stoul(ligne)))
					return false;
				fini=true;
				break;
		}
	}
	
	if(!ptrN1->addLink(ptrN2))
		return false;
		
	return true;
}

vector<string> sauvegardeNoeud(Type type)
{
	string donneesNoeud;
	vector<string> donnees;
	Point centre;
	
	for(auto noeud : database)
	{
		if(noeud->getType()==type)
		{
			donneesNoeud="";
			centre=noeud->getPosition();
			
			donneesNoeud+=to_string(noeud->getUid())+" ";
			donneesNoeud+=to_string(centre.x)+" "+to_string(centre.y)+" ";
			donneesNoeud+=to_string(noeud->getNbp());
			
			donnees.push_back(donneesNoeud);
		}
	}
	
	return donnees;
}

vector<vector<int>> sauvegardeLien()
{
	vector<vector<int>> liensTraites;
	bool traite;
	int uid1,uid2;
	
	for(auto noeud:database)
	{
		uid1 = noeud->getUid();
		
		for(auto noeudLien: noeud->getLink())
		{
			uid2= noeudLien->getUid();
			traite=false;
			
			for(auto lien : liensTraites)
				if((uid1==lien[0] && uid2==lien[1])||(uid1==lien[1] && uid2==lien[0]))
					traite=true;
			
			if(!traite)
				liensTraites.push_back({uid1,uid2}); 
		}
	}
	
	return liensTraites;
}

void clearDatabase()
{
	for(auto & noeud : database)
		delete noeud;
	
	database.clear();
	noeudCourant=nullptr;
}

bool noeudSelectione()
{
	if(noeudCourant==nullptr)
		return false;
	
	return true;
}

size_t getDatabaseSize()
{
	return database.size();
}

bool analyse1Temps(double x, double y, Type type)
{
	Point p({x,y});
	Noeud* noeudPointe(getNoeudPointe(p));
	
	if(noeudPointe!=nullptr)
	{
		if(noeudPointe==noeudCourant)
		{
			deleteNoeud(noeudCourant);
			noeudCourant=nullptr;
		}
		
		else
			noeudCourant=noeudPointe;
		
		return true;
	}
	
	else if(noeudCourant==nullptr)
	{
		ajouterNoeud(type, getUidLibre(), min_capacity, p, dist_min);
		return true;
	}
	
	return false;		
}

void analyse2Temps(double xd, double yd, double xf, double yf)
{
	Point pDebut({xd,yd}), pFin({xf,yf});
	
	if(xd==xf && yd==yf)
		noeudCourant=nullptr;
		
	else
		noeudCourant->varNbp(calcVarRayon(pDebut,pFin,noeudCourant->getPosition()));
}

void deplacerNoeud(double x, double y)
{
	Point antPos=noeudCourant->getPosition();
	bool interLien(false);
	
	if(!noeudCourant->setPosition({x,y},dist_min))
		return;
	
	for(auto nLien : noeudCourant->getLink())
		if(!noeudCourant->verifieInterLien(nLien, dist_min))
		{
			interLien = true;
			break;
		}
	
	if(interLien)
		noeudCourant->setPosition(antPos,0);
}

bool editionLien(double x, double y)
{
	Point p({x,y});
	Noeud* noeudPointe(getNoeudPointe(p));
	
	if(noeudPointe!=nullptr)
	{		
		if(noeudCourant->dansLien(noeudPointe))
		{
			noeudCourant->supprLink(noeudPointe);
			noeudPointe->supprLink(noeudCourant);
		}
		else
			noeudCourant->addLink(noeudPointe,dist_min);
	}
	
	return true;
}

double calculENJ()
{
	if(database.empty())
		return 0;
	
	double nbpTotNuit(0), nbpTotJour(0);
	
	for(auto const& noeud : database)
	{
		if(noeud->getType()==LOGEMENT)
			nbpTotNuit+=noeud->getNbp();
		else
			nbpTotJour+=noeud->getNbp();
	}
	
	return (nbpTotNuit-nbpTotJour)/(nbpTotNuit+nbpTotJour);
}

double calculCI()
{
	double ci(0);
	
	vector<vector<int>> liensTraites;
	bool traite;
	int uid1,uid2,capacite;
	Point centre1,centre2;
	
	for(auto const & noeud:database)
	{
		uid1 = noeud->getUid();
		centre1=noeud->getPosition();
		
		for(auto const & noeudLien: noeud->getLink())
		{
			uid2= noeudLien->getUid();
			traite=false;
			
			for(auto lien : liensTraites)
				if((uid1==lien[0] && uid2==lien[1])||(uid1==lien[1] && uid2==lien[0]))
					traite=true;
			
			if(!traite)
			{
				capacite=min(noeud->getNbp(),noeudLien->getNbp());
				centre2=noeudLien->getPosition();
				
				if(noeud->getType()==TRANSPORT && noeudLien->getType()==TRANSPORT)
					ci+=calcDist(centre1,centre2)*capacite*fast_speed;
				else
					ci+=calcDist(centre1,centre2)*capacite*default_speed;
					
				liensTraites.push_back({uid1,uid2});
			} 
		}
	}
	
	return ci;
}

double calculMTA()
{	
	double mtaTransport(0), mtaProduction(0);
	int nbNoeudLogement(0);
	Noeud* nPlusCourtAccess;
	
	for(auto noeud : database)
		if(noeud->getType()==LOGEMENT)
		{
			nbNoeudLogement++;
			
			nPlusCourtAccess = plusCourtAccess(noeud,TRANSPORT);
			if(nPlusCourtAccess == nullptr)
				mtaTransport+=infinite_time;
			else
				mtaTransport+=nPlusCourtAccess->getDjikstraAccess();
				
			nPlusCourtAccess = plusCourtAccess(noeud,PRODUCTION);
			if(nPlusCourtAccess == nullptr)
				mtaProduction+=infinite_time;
			else
				mtaProduction+=nPlusCourtAccess->getDjikstraAccess();
		}
	
	if(nbNoeudLogement==0)
		return 0;
	
	return 1./nbNoeudLogement * (mtaTransport+mtaProduction);
}

void dessinerNoeuds(int const lesser, double const dim, bool shortestPath)
{		
	for(auto const & noeud : database)
	{
		if(noeud==noeudCourant)
		{
			tools_changerCouleur(ROUGE);
			noeudCourant->dessiner(lesser,dim);
			tools_changerCouleur(NOIR);
		}
		else if(shortestPath && noeud->getDSP())
		{
			tools_changerCouleur(VERT);
			noeud->dessiner(lesser,dim);
			tools_changerCouleur(NOIR);
		}
		else
			noeud->dessiner(lesser,dim);
	}
}

void dessinerLiens(int const lesser, double const dim, bool shortestPath)
{
	vector<vector<int>> liensTraites;
	bool traite;
	int uid1,uid2;
	Point centre1,centre2;
	
	clearShortestPath();
	if(shortestPath && noeudCourant!=nullptr)
	{
		tools_changerCouleur(VERT);
		initShortestPath(liensTraites,lesser,dim);
		tools_changerCouleur(NOIR);
	}
	
	for(auto const & noeud:database)
	{
		uid1 = noeud->getUid();
		centre1=noeud->getPosition();
		for(auto const & noeudLien: noeud->getLink())
		{
			uid2= noeudLien->getUid();
			traite=false;
			for(auto lien : liensTraites)
				if((uid1==lien[0] && uid2==lien[1])||(uid1==lien[1] && uid2==lien[0]))
					traite=true;
			
			if(!traite)
			{
				centre2=noeudLien->getPosition();
				afficherSeg(lesser,dim,centre1, centre2);	
				liensTraites.push_back({uid1,uid2});
			} 
		}
	}
}

namespace
{
	bool getNoeud(Noeud* &ptrNoeud,unsigned int uid)
	{
		for(auto noeud: database)
		{
			if(noeud->getUid()==uid)
			{
				ptrNoeud=noeud;
				return true;
			}
		}
		cout<<error::link_vacuum(uid);
		return false;
	}
	
	bool ajouterNoeud(Type type, int uid, int nbp, Point centre, double distMin)
	{
		Noeud* ptr(nullptr);
		
		switch(type)
		{
			case LOGEMENT:
				ptr = new Logement;
				break;
			case PRODUCTION:
				ptr = new Production;
				break;
			case TRANSPORT:
				ptr=new Transport;
				break;
			case LIEN:
				break;
		}
		
		if(!ptr->setUid(uid) || !ptr->setQuartier(centre,nbp, distMin))
		{
			delete ptr;
			return false;
		}
		
		database.push_back(ptr);
		return true;
	}
	
	void deleteNoeud(Noeud* nDeleted)
	{
		unsigned int uid(nDeleted->getUid());
		size_t pos;
		
		for(auto nlien : nDeleted->getLink())
			nlien->majLink(uid);
		
		for(size_t i(0); i<database.size(); i++)
			if(database[i]->getUid()==uid)
				pos=i;
		
		swap(database[pos], database[database.size()-1]);
		database.pop_back();
		
		delete nDeleted;
	}

	Noeud* plusCourtAccess(Noeud* nDepart, Type typeNoeudCherche)
	{
		vector<Noeud*>ta = database;
		bool vide(false);
		Noeud* noeudTraite;
		double alt;
		initDjikstra(nDepart);
		
		sort(ta.begin(),ta.end(),sortTA);
		while(!vide)
		{
			noeudTraite=nullptr;
			
			for(size_t i(0); noeudTraite==nullptr; i++)
				if(ta[i]->getDjikstraIn())
					noeudTraite=ta[i];
					
			if(noeudTraite->getType()==typeNoeudCherche)
				return noeudTraite;
			
			noeudTraite->setDjikstraIn(false);
			if(noeudTraite->getType()!=PRODUCTION)
				for(auto voisin: noeudTraite->getLink())
					if(voisin->getDjikstraIn())
					{
						alt=noeudTraite->getDjikstraAccess()+calculAccess(noeudTraite,
																		  voisin);
						if(alt < voisin->getDjikstraAccess())
						{
							voisin->setDjikstraAccess(alt);
							voisin->setDjikstraParent(noeudTraite);
							sort(ta.begin(),ta.end(),sortTA);
						}
					}
			vide = true;
			for(auto noeud: database)
				if(noeud->getDjikstraIn())
					vide=false;
		}
		return nullptr;
	}

	double calculAccess(Noeud* noeud1, Noeud* noeud2)
	{
		Point c1(noeud1->getPosition()),c2(noeud2->getPosition());
		double vitesse(default_speed);
		
		if(noeud1->getType()==TRANSPORT && noeud2->getType()==TRANSPORT)
			vitesse = fast_speed;
		
		return calcDist(c1,c2)/vitesse;
	}

	//fonction pour le tri de TA
	bool sortTA(Noeud* ptrNoeud1, Noeud* ptrNoeud2)
	{
		return ptrNoeud1->getDjikstraAccess() < ptrNoeud2->getDjikstraAccess();
	}
	
	void initDjikstra(Noeud* nDepart)
	{
		for(auto & noeud : database)
		{
			noeud->setDjikstraIn(true);
			noeud->setDjikstraAccess(infinite_time);
			noeud->setDjikstraParent(nullptr);
		}
		nDepart->setDjikstraAccess(0);
	}
	
	Noeud* getNoeudPointe(const Point p)
	{
		for(auto noeud : database)
		{
			if(noeud->pointDansNoeud(p))
				return noeud;
		}
		
		return nullptr;
	}
	
	unsigned int getUidLibre()
	{
		unsigned int uid(0);
		bool diff;
		
		do
		{
			uid++;
			diff=true;
			for(auto noeud : database)
				if(noeud->getUid()==uid)
					diff=false;
					
		}while(!diff);
		
		return uid;
	}
	
	
	void initShortestPath(vector<vector<int>>& liensTraites,int lesser, double dim)
	{		
		Noeud* nPlusProche;
		
		if(noeudCourant->getType()==LOGEMENT)
		{
			nPlusProche = plusCourtAccess(noeudCourant, TRANSPORT);
			if(nPlusProche!=nullptr && nPlusProche->getDjikstraAccess()!=infinite_time)
				nPlusProche->setShortestPath(liensTraites,lesser,dim);	
					
			nPlusProche = plusCourtAccess(noeudCourant, PRODUCTION);
			if(nPlusProche!=nullptr && nPlusProche->getDjikstraAccess()!=infinite_time)
				nPlusProche->setShortestPath(liensTraites,lesser,dim);
		}
	}

	void clearShortestPath()
	{
		for(auto noeud : database)
			noeud->reinitDSP();
	}
}
