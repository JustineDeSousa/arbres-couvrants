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

#define max(a,b) (((a) > (b)) ? (a) : (b))


int main() {

  int popMin;
  printf("Minimal population? ");
  scanf("%i", &popMin);


  ListOfCities* cities;

  for(int dpt = 1; dpt<95; dpt++){
//-----------------------------------------------------------------
//--- READING cities from departement = dpt
//-----------------------------------------------------------------
    char outputFile[50];
    sprintf(outputFile,"resuCities_%d.dat",dpt);
    cities = citiesReader(popMin,dpt,outputFile);
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
//-----------------------------------------------------------------
//--- WRITING GRAPH for cities from departement = dpt
//----------------------------------------------------------------- 
    char filename[50];
    sprintf(filename,"resuGraph_%d.dat",dpt);
    FILE* fileOut = fopen(filename, "w");
    for(int i=0; i<cities->number; i++){
      fprintf(fileOut, "%i %i\n", i, voisin[i]);
    }
    fclose(fileOut);


  }


//-----------------------------------------------------------------
//--- READING bigger city from each departement
//-----------------------------------------------------------------                             
  cities = bigcitiesReader();
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
//-----------------------------------------------------------------
//--- WRITING GRAPH for big cities
//-----------------------------------------------------------------
  FILE* fileOut = fopen("resuGraph_bigcities.dat", "w");
  for(int i = 0; i<cities->number; i++){
    fprintf(fileOut, "%i %i\n", i, voisin[i]);
  }
  fclose(fileOut);
  

//---------------------------------------------------------------
//--- Réunion de tous les fichiers
//---------------------------------------------------------------
  FILE* outputFile = NULL;
  outputFile = fopen("resuCities.dat","w");
  
  if(outputFile != NULL){
    for(int dpt = 1; dpt < 95; dpt ++){
      char filename[50];
      sprintf(filename,"resuCities_%d.dat",dpt);
      printf("== Merging cities file with population >= %i and department = %i from %s ==\n", popMin, dpt, filename);

      FILE* inputFile = NULL;
      inputFile = fopen(filename, "r");
      
      if(inputFile != NULL){
        char line[512];
        const char s[2] = " ";
        char *token;
      
        while(fgets(line, 512, inputFile) != NULL){
          token = strtok(line, s);
          int myPop = atoi(token);
          token = strtok(NULL, s);
          float myLon = atof(token);
          token = strtok(NULL, s);
          float myLat = atof(token);

          fprintf(outputFile, "%i %f %f\n", myPop, myLon, myLat);
        }
      }
    }
  }
  
  outputFile = NULL;
  outputFile = fopen("resuGraph.dat","w");
  
  if(outputFile != NULL){
    int n = 0;
    int villeMax = 0;
    for(int dpt = 1; dpt < 95; dpt ++){
      char filename[50];
      sprintf(filename,"resuGraph_%d.dat",dpt);
      printf("== Merging graph file with population >= %i and department = %i from %s ==\n", popMin, dpt, filename);

      FILE* inputFile = NULL;
      inputFile = fopen(filename, "r");
      
      if(inputFile != NULL){
        char line[512];
        const char s[2] = " ";
        char *token;
        villeMax = 0;
        while(fgets(line, 512, inputFile) != NULL){
          token = strtok(line, s);
          int villeA = atoi(token);
          token = strtok(NULL, s);
          int villeB = atoi(token);
          fprintf(outputFile, "%i %i\n", villeA+n, villeB+n);

          int villeMax = max(villeMax,max(villeA,villeB));
        }
      }
      n += villeMax + 1;
    }
  }

  




  



  
  




/*
  // Écriture du graphe (chaque ligne correspond à une arête)


  
  //Calcul de la taille du réseau cities
  float taille_reseau_big_cities = 0;
  for(int i = 0; i < cities->number; i++){ 
    taille_reseau_big_cities += poids(cities,i,voisin[i]);
  }

  // ... just to check! This line can be removed.
  printf("\n %i cities: \n\n", cities->number);
  printf("Taille du reseau: %1.0f km\n\n",taille_reseau_big_cities);

  */

    /*
 
  */

/*
  //---------------------------------------------------------------
  //---------------------------------------------------------------

  //Calcul de la taille du réseau cities
  float taille_reseau = taille_reseau_big_cities + taille_reseau_citie6;

  // ... just to check! This line can be removed.
  printf("\n  cities: \n\n");
  printf("Taille du reseau: %1.0f km\n",taille_reseau);

  */
  


  //-----------------------------------------------------------------
  //--- DEALLOCATE arrays
  //-----------------------------------------------------------------
  
  free(cities->dpt);
  free(cities->name);
  free(cities->pop);
  free(cities->lon);
  free(cities->lat);
  free(cities);
/*
  free(cities->dpt);
  free(cities->name);
  free(cities->pop);
  free(cities->lon);
  free(cities->lat);
  free(cities);
*/


  return 0;
}
