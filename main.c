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
  popMin = 100000;
  //printf("Minimal population? ");
  //scanf("%i", &popMin);

  ListOfCities* cities;
  cities = citiesReader(popMin,0);
  //cities = bigcitiesReader();  //read the largest cities of every provinces
  
//-----------------------------------------------------------------
//--- COMPUTING graph
//-----------------------------------------------------------------

  
  // allocation des variables
  bool* dansS = malloc(cities->number*sizeof(bool));
  int* voisin = malloc(cities->number*sizeof(int));
  float* dist = malloc(cities->number*sizeof(float));

  //Initialisation
  dansS[0] = true;  //On démarre du sommet 0
  dist[0] = 0;      //dist(0,0) = 0
  for(int i = 0; i < cities->number; i++){
    dansS[i] = false;
    dist[i] = poids(cities,0,i);
    voisin[i] = 0;
  }

  //Itérations
  int k = 0;
  while(k < cities->number - 2){
    //Trouver i tels que (dansS[i]=false) et (dist[i] est minimal)
    float minDist = FLT_MAX;
    int i = 0;
    for(int j = 1; j < cities->number; j++){
      if( dansS[j] == false){
        if(dist[j] < minDist){
          minDist = dist[j];
          i = j;
        }
      }
    }//minDist,min_i OK
    dansS[i] = true;
    
    for(int j = 0; j < cities->number; j++){
      if( dansS[j] == false ){
        if( dist[j] > poids(cities,i,j) ){
          dist[j] = poids(cities,i,j);
          voisin[j] = i;
        }
      }
    }
    k ++;
  }


  //Calcul de la taille du réseau
  float taille_reseau;
  for(int i = 1; i < cities->number; i++){
    taille_reseau += poids(cities,i,voisin[i]);
  }
  


  // ... just to check! This line can be removed.
  printf("\n %i cities: \n\n", cities->number);
  printf("Taille du reseau: %1.0f km\n",taille_reseau);
  
  printf("       %25s %2s %6s  %9s %9s\n","VILLE","dpt","pop","longitude","latitude");
  printf("----------------------------------------------------------------\n");
  for(int i=0; i<cities->number; i++){
    printf("%3i/%3i: %25s %2i %7i %9f %9f\n",i+1, cities->number, cities->name[i], cities->dpt[i], cities->pop[i], cities->lon[i], cities->lat[i]);
  }
  
  

  // Écriture du graphe (chaque ligne correspond à une arête)
  FILE* fileOut = fopen("resuGraph.dat", "w");
  for(int i=0; i<cities->number; i++){
    fprintf(fileOut, "%i %i\n", i, voisin[i]);
    printf("%i %i\n", i, voisin[i]);
  }
  fclose(fileOut);


  //-----------------------------------------------------------------
  //--- DEALLOCATE arrays
  //-----------------------------------------------------------------
  free(cities->dpt);
  free(cities->name);
  free(cities->pop);
  free(cities->lon);
  free(cities->lat);
  free(cities);


  return 0;
}
