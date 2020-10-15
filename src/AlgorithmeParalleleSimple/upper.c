/* TP algorithmique parallele
 * maitrise
 * LL
 * 13/10/97
 */

/*
 * upper.c
 *
 * programme principal en sequentiel
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "point.h"
#include "pvm3.h"


/*
 * calcul recursif d'enveloppe
 * convexe par bissection
 */

/*
 * upper <nb points>
 * exemple :
 * % upper 200 > courbe
 * % jgraph courbe > courbe.ps
 * % ghostview courbe.ps
 */

 void Copier_X(pts,tab)
      point *pts;
      int tab[point_nb(pts)];
   {
		point *pt1;
                int i=0;

	for (pt1=pts; pt1!=NULL; pt1=pt1->next) {
                 tab[i]=pt1->x;
                 i++;
	}
   }

   void Copier_Y(pts,tab)
     point *pts;
     int tab[point_nb(pts)];
     {
		point *pt1;
                int i=0;

	for (pt1=pts; pt1!=NULL; pt1=pt1->next) {
                 tab[i]=pt1->y;
                 i++;
	}
     }


  static int compareX(i,j)
    point **i, **j;
    { return (*i)->x - (*j)->x; }

    point *Copier_Tab(nbPts,tabX,tabY)
       int nbPts;
       int tabX[nbPts];
       int tabY[nbPts];
     {
	int i,j=0;
	point **pts;

	pts = (point **)malloc(nbPts*sizeof(point *));
	for(i=0; i < nbPts; i++) {
		pts[i] = point_alloc();
		pts[i]->x = tabX[j];;
		pts[i]->y = tabY[j];
	         j++;		
         }

	qsort(pts, nbPts, sizeof(point *), compareX);
	for (i=0; i<nbPts-1; i++)
		pts[i]->next = pts[i+1];

	return (point *)*pts;
     }




main(int argc, char **argv)
{
        
	int mytid;		/* tid tache */
	int parent;		/* tid du pere */
	int nbPoints;	        /* nombre d'elements de l'ensembles */
	int tids[2];		/* tids fils */
        point *pts, *pts2 ;        /*liste de points */
       


	
	parent = pvm_parent();

	if (parent == PvmNoParent) {  //initialisation 

              // initialisation dun ensemble de point 
                 pts = point_random(atoi(argv[1]));
               //affichage l'ensemble des points 
               point_print_gnuplot(pts, 0); 
                 //point_print(pts, 0); 
               // nombre d'elements de l'ensemble  
                 nbPoints = point_nb(pts);
               
             
	}
	else { // reception des donnees
                
                pvm_recv(parent, MSG_DATA);  
		pvm_upkint(&nbPoints, 1, 1); // Recevoir  la taille de l'ensemble 
                int tabX [nbPoints];
                int tabY [nbPoints];
		pvm_upkint(tabX, nbPoints, 1); // Recevoir l'ensemble de point
                pvm_upkint(tabY, nbPoints, 1);
                pts = Copier_Tab(nbPoints,tabX,tabY);
       }

	
	// phase 2: calcul parallele

	if (nbPoints > 4) {
		// créer 2 fils
                pvm_spawn(BPWD "/upper", (char**)0, 0, "", 2, tids);
		//  partitionner l'ensemble 
                 pts2 = point_part(pts);
                        int tabPointsX1 [point_nb(pts)] ;
                        int tabPointsY2 [point_nb(pts)] ;
 			int tabPointsX3 [point_nb(pts2)] ;
 			int tabPointsY4 [point_nb(pts2)] ;
                 // stocker les x et y dans des tableaux d entiers, 
                        // ensemble gauche de points
                        Copier_X(pts , tabPointsX1);
                        Copier_Y(pts , tabPointsY2);                          
                        // ensemble droite de points
                        Copier_X(pts2 , tabPointsX3);
                        Copier_Y(pts2 , tabPointsY4);

                        int taille1=point_nb(pts);
                        int taille2=point_nb(pts2);

		// envoie de l'ensemble de points

		// Fils 1 
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&taille1, 1, 1); // envoyer la taille de sous ensemble
		pvm_pkint(tabPointsX1,taille1, 1); // envoyer les x de sous ensemble
                pvm_pkint(tabPointsY2,taille1, 1); // envoyer les y de sous ensemble
		pvm_send(tids[0], MSG_DATA);

		// Fils 2
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&taille2, 1, 1); // envoyer la taille de sous ensemble
		pvm_pkint(tabPointsX3,taille2, 1); // envoyer les x de sous ensemble
                pvm_pkint(tabPointsY4,taille2, 1); // envoyer les y de sous ensemble
		pvm_send(tids[1], MSG_DATA);

		// reception deux enveloppe           
 		 
                // Fils 1
                pvm_recv(tids[0], MSG_SORT);
		pvm_upkint(&nbPoints, 1, 1); // recevoir la taille de sous ensemble
                int tabX1 [nbPoints];
                int tabY1 [nbPoints];
                pvm_upkint(tabX1, nbPoints, 1); // recevoir les x de sous ensemble
                pvm_upkint(tabY1, nbPoints, 1); // recevoir les y de sous ensemble
                pts = Copier_Tab(nbPoints,tabX1,tabY1); // construire l'ensemble de points a partir les X et les Y
                
                // Fils 2
		pvm_recv(tids[1], MSG_SORT);
		pvm_upkint(&nbPoints, 1, 1); // recevoir la taille de sous ensemble
                int tabX2 [nbPoints];
                int tabY2 [nbPoints];
                pvm_upkint(tabX2, nbPoints, 1); // recevoir les x de sous ensemble
                pvm_upkint(tabY2, nbPoints, 1);// recevoir les y de sous ensemble
                pts2 = Copier_Tab(nbPoints,tabX2,tabY2);// construire l'ensemble de points a partir les X et les Y

		// fusion les deux resultats
		point_merge_UH(pts,pts2); 
             
	}
          else // Calculer l'enveloppe haute directement 
            	 pts = point_UH(pts);

	// phase 3: retour

	if (parent == PvmNoParent) { // Affichage Resultat
                 
                point_print_gnuplot(pts, 1);
                //point_print(pts, 1); 
	}
	else { // renvoi les enveloppe au parents
		pvm_initsend(PvmDataDefault);
                int tabPointsX1 [point_nb(pts)] ;
                int tabPointsY2 [point_nb(pts)] ;
                int taille3=point_nb(pts);
                Copier_X(pts , tabPointsX1);
                Copier_Y(pts , tabPointsY2);
                pvm_pkint(&taille3, 1, 1); // envoyer la taille de l'ensemble qui construit l'enveloppe haute
                pvm_pkint(tabPointsX1,taille3, 1);  // envoyer les X de l'ensemble qui construit l'enveloppe haute
                pvm_pkint(tabPointsY2,taille3, 1);// envoyer les Y de l'ensemble qui construit l'enveloppe haute
		pvm_send(parent, MSG_SORT);

	}

		

	/* codage d'un tri-fusion recursif */
	//sort_rec(data, nb_data);
	//show_sort(data);




	pvm_exit();
	exit(0);
}
