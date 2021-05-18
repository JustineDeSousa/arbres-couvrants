// Compilation:
//   icc -std=c99 main.c citiesReader.c
// Execution:
//   ./a.out

#include "citiesReader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>


int main() {

//-----------------------------------------------------------------
//--- READING cities
//-----------------------------------------------------------------

  int popMin;
  popMin = 1000;
  //printf("Minimal population? ");
  //scanf("%i", &popMin);

  ListOfCities* bigCities;                                  
  bigCities = bigcitiesReader();  //read the largest cities of every provinces
  
//-----------------------------------------------------------------
//--- COMPUTING graph
//-----------------------------------------------------------------

  //---------------------------------------------------------------
  //--- Graph of bigCities
  //---------------------------------------------------------------
  
  // allocation des variables
  bool* dansS = malloc(bigCities->number*sizeof(bool));
  int* voisin = malloc(bigCities->number*sizeof(int));
  float* dist = malloc(bigCities->number*sizeof(float));

  //Initialisation
  dansS[0] = true;  //On démarre du sommet 0
  dist[0] = 0;      //dist(0,0) = 0
  for(int i = 0; i < bigCities->number; i++){
    dansS[i] = false;
    dist[i] = poids(bigCities,0,i);
    voisin[i] = 0;
  }

  //Itérations
  int k = 0;
  while(k < bigCities->number - 2){
    //Trouver i tels que (dansS[i]=false) et (dist[i] est minimal)
    float minDist = FLT_MAX;
    int i = 0;
    for(int j = 1; j < bigCities->number; j++){
      if( dansS[j] == false){
        if(dist[j] < minDist){
          minDist = dist[j];
          i = j;
        }
      }
    }//minDist,min_i OK
    dansS[i] = true;
    
    for(int j = 0; j < bigCities->number; j++){
      if( dansS[j] == false ){
        if( dist[j] > poids(bigCities,i,j) ){
          dist[j] = poids(bigCities,i,j);
          voisin[j] = i;
        }
      }
    }
    k ++;
  }

  // Écriture du graphe (chaque ligne correspond à une arête)
  FILE* fileOut = fopen("resuGraphBigCities.dat", "w");
  for(int i = 0; i<bigCities->number; i++){
    fprintf(fileOut, "%i %i\n", i, voisin[i]);
  }
  fclose(fileOut);

  
  //Calcul de la taille du réseau bigCities
  float taille_reseau_big_cities = 0;
  for(int i = 0; i < bigCities->number; i++){ 
    taille_reseau_big_cities += poids(bigCities,i,voisin[i]);
  }

  // ... just to check! This line can be removed.
  printf("\n %i cities: \n\n", bigCities->number);
  printf("Taille du reseau: %1.0f km\n\n",taille_reseau_big_cities);

    /*
  printf("       %25s %2s %6s  %9s %9s\n","VILLE","dpt","pop","longitude","latitude");
  printf("----------------------------------------------------------------\n");
  for(int i=0; i<bigCities->number; i++){
    printf("%3i/%3i: %25s %2i %7i %9f %9f\n",i+1, bigCities->number, bigCities->name[i], bigCities->dpt[i], bigCities->pop[i], bigCities->lon[i], bigCities->lat[i]);
  }
  */


//-----------------------------------------------------------------
//--- READING citie6
//-----------------------------------------------------------------
  ListOfCities* citie6;
  citie6 = citiesReader(popMin,6);  //read the cities whith pop >= popMin
                                      //write in resuCities.dat

//-----------------------------------------------------------------
//--- COMPUTING graph
//-----------------------------------------------------------------

  //---------------------------------------------------------------
  //--- Graph of cities with dpt = 6
  //---------------------------------------------------------------

  // allocation des variables
  dansS = malloc(citie6->number*sizeof(bool));
  voisin = malloc(citie6->number*sizeof(int));
  dist = malloc(citie6->number*sizeof(float));

  //Initialisation
  dansS[0] = true;  //On démarre du sommet 0
  dist[0] = 0;      //dist(0,0) = 0
  for(int i = 0; i < citie6->number; i++){
    dansS[i] = false;
    dist[i] = poids(citie6,0,i);
    voisin[i] = 0;
  }

  //Itérations
  k = 0;
  while(k < citie6->number - 2){
    //Trouver i tels que (dansS[i]=false) et (dist[i] est minimal)
    float minDist = FLT_MAX;
    int i = 0;
    for(int j = 1; j < citie6->number; j++){
      if( dansS[j] == false){
        if(dist[j] < minDist){
          minDist = dist[j];
          i = j;
        }
      }
    }//minDist,min_i OK
    dansS[i] = true;
    
    for(int j = 0; j < citie6->number; j++){
      if( dansS[j] == false ){
        if( dist[j] > poids(citie6,i,j) ){
          dist[j] = poids(citie6,i,j);
          voisin[j] = i;
        }
      }
    }
    k ++;
  }

  // Écriture du graphe (chaque ligne correspond à une arête)
  // On écrit les arêtes de ce département à la suite des bigCities
  fileOut = fopen("resuGraph6.dat", "w");
  for(int i=0; i<citie6->number; i++){
    fprintf(fileOut, "%i %i\n", i, voisin[i]);
  }
  fclose(fileOut);


  //Calcul de la taille du réseau citie6
  float taille_reseau_citie6 = 0;
  for(int i = 0; i < citie6->number; i++){ 
    taille_reseau_citie6 += poids(citie6,i,voisin[i]);
  }

  // ... just to check! This line can be removed.
  printf("\n %i cities: \n\n", citie6->number);
  printf("Taille du reseau: %1.0f km\n",taille_reseau_citie6);


  
  printf("       %25s %2s %6s  %9s %9s\n","VILLE","dpt","pop","longitude","latitude");
  printf("----------------------------------------------------------------\n");
  for(int i=0; i<citie6->number; i++){
    printf("%3i/%3i: %25s %2i %7i %9f %9f\n",i+1, citie6->number, citie6->name[i], citie6->dpt[i], citie6->pop[i], citie6->lon[i], citie6->lat[i]);
  }
  

  //---------------------------------------------------------------
  //---------------------------------------------------------------

  //Calcul de la taille du réseau bigCities
  float taille_reseau = taille_reseau_big_cities + taille_reseau_citie6;

  // ... just to check! This line can be removed.
  printf("\n  cities: \n\n");
  printf("Taille du reseau: %1.0f km\n",taille_reseau);

  
  


  //-----------------------------------------------------------------
  //--- DEALLOCATE arrays
  //-----------------------------------------------------------------
  /*
  free(cities->dpt);
  free(cities->name);
  free(cities->pop);
  free(cities->lon);
  free(cities->lat);
  free(cities);
*/
  free(bigCities->dpt);
  free(bigCities->name);
  free(bigCities->pop);
  free(bigCities->lon);
  free(bigCities->lat);
  free(bigCities);

  free(citie6->dpt);
  free(citie6->name);
  free(citie6->pop);
  free(citie6->lon);
  free(citie6->lat);
  free(citie6);


  return 0;
}
