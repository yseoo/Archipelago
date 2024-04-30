/*ville.h
 * Killian Hinard et Rémi Oddon*/
/* Figure 11 b1 */

#ifndef VILLE_H
#define VILLE_H

bool lecture_fichier(std::string nom_fichier);
void dessinerVille(int const lesser, double const dim);
void reinitialisation();
void calculCritere(double& enj, double& ci, double& mta);
void sauvegarde(std::string fichier);
bool action1temps(double x, double y, bool editLink, char type);
void action2temps(double xdebut, double ydebut, double xfin, double yfin);
void deplacer(double x, double y);
bool checkEditLink();
void changeShortestPath(bool shortestPath);

#endif 
