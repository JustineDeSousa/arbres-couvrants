// Compilation:
//   icc -mkl -std=c99 main.c citiesReader.c
// Execution:
//   ./a.out

#include "citiesReader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <mkl.h>

int main() {

  int popMin;
  printf("Minimal population? ");
  scanf("%i", &popMin);


  ListOfCities* cities;
  float taille_reseau = 0;
  int nb_villes = 0;
  double time = 0;

  for(int dpt = 1; dpt<95; dpt++){
//-----------------------------------------------------------------
//--- READING cities from departement = dpt
//-----------------------------------------------------------------
    char outputFile[50];
    sprintf(outputFile,"resuCities_%d.dat",dpt);
    cities = citiesReader(popMin,dpt,outputFile);
    nb_villes += cities->number;

    //TEMPS DE CALCUL: entrée dans l'algo
    unsigned MKL_INT64 t0;
    mkl_get_cpu_clocks(&t0);
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
//--- GRAPH COMPUTED
//-----------------------------------------------------------------
    //TEMPS DE CALCUL: sortie de l'algo
    unsigned MKL_INT64 t1;
    mkl_get_cpu_clocks(&t1);
    time += (double)(t1-t0)/mkl_get_clocks_frequency();

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

    float taille_reseau_dpt = 0;
    for(int i = 0; i < cities->number; i++){ 
      taille_reseau_dpt += poids(cities,i,voisin[i]);
    }
    taille_reseau += taille_reseau_dpt;

//-----------------------------------------------------------------
//--- DEALLOCATE arrays
//-----------------------------------------------------------------
    free(dansS);
    free(voisin);
    free(dist);
  }


//-----------------------------------------------------------------
//--- READING bigger city from each departement
//-----------------------------------------------------------------                             
  cities = bigcitiesReader();

  //TEMPS DE CALCUL: entrée dans l'algo
  unsigned MKL_INT64 t2;
  mkl_get_cpu_clocks(&t2);

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
//--- GRAPH COMPUTED
//-----------------------------------------------------------------
  //TEMPS DE CALCUL: sortie de l'algo
  unsigned MKL_INT64 t3;
  mkl_get_cpu_clocks(&t3);
  //TEMPS DE CALCUL TOTAL
  double duration = (double)(t3 - t2 + time)/mkl_get_clocks_frequency()/1e9;
  printf("time is %f second\n ",duration);

//-----------------------------------------------------------------
//--- WRITING GRAPH for big cities
//-----------------------------------------------------------------
  FILE* fileOut = fopen("resuGraph_bigcities.dat", "w");
  for(int i = 0; i<cities->number; i++){
    fprintf(fileOut, "%i %i\n", i, voisin[i]);
  }
  fclose(fileOut);
  
  for(int i = 0; i < cities->number; i++){ 
    taille_reseau += poids(cities,i,voisin[i]);
  }
//-----------------------------------------------------------------
//--- DEALLOCATE arrays
//-----------------------------------------------------------------
  free(dansS);
  free(voisin);
  free(dist);

  free(cities->dpt);
  free(cities->name);
  free(cities->pop);
  free(cities->lon);
  free(cities->lat);
  free(cities);

//---------------------------------------------------------------
//--- Réunion de tous les fichiers
//---------------------------------------------------------------
  FILE* citiesOutputFile = NULL;
  citiesOutputFile = fopen("resuCities.dat","w");
  FILE* graphOutputFile = NULL;
  graphOutputFile = fopen("resuGraph.dat","w");
  
  if(citiesOutputFile != NULL && graphOutputFile != NULL){
    int n = 0;
    int nbVilles = 0;
    for(int dpt = 1; dpt < 95; dpt ++){
      char citiesFilename[50];
      sprintf(citiesFilename,"resuCities_%d.dat",dpt);
      char graphFilename[50];
      sprintf(graphFilename,"resuGraph_%d.dat",dpt);
      //printf("== Merging cities file with population >= %i and department = %i from %s ==\n", popMin, dpt, citiesFilename);
      //printf("== Merging graph  file with population >= %i and department = %i from %s ==\n", popMin, dpt, graphFilename);

      FILE* citiesInputFile = NULL;
      citiesInputFile = fopen(citiesFilename, "r");
      FILE* graphInputFile = NULL;
      graphInputFile = fopen(graphFilename, "r");
      
      if(citiesInputFile != NULL && graphInputFile != NULL){
        char line[512];
        const char s[2] = " ";
        char *token;

        nbVilles = 0;
        while(fgets(line, 512, citiesInputFile) != NULL){
          token = strtok(line, s);
          int myPop = atoi(token);
          token = strtok(NULL, s);
          float myLon = atof(token);
          token = strtok(NULL, s);
          float myLat = atof(token);

          fprintf(citiesOutputFile, "%i %f %f\n", myPop, myLon, myLat);
          nbVilles ++;
        }
        while(fgets(line, 512, graphInputFile) != NULL){
          token = strtok(line, s);
          int villeA = atoi(token);
          token = strtok(NULL, s);
          int villeB = atoi(token);
          fprintf(graphOutputFile, "%i %i\n", villeA+n, villeB+n);
        }
      }
      n += nbVilles;
      if (remove(citiesFilename) != 0) {
        printf("The file %s is not deleted.",citiesFilename);
      }
      if (remove(graphFilename) != 0) {
        printf("The file %s is not deleted.",graphFilename);
      }
    }
  }
//-----------------------------------------------------------------
//--- FICHIERS DPT REUNIS DANS resuGraph.dat et resuCities.dat
//-----------------------------------------------------------------

  printf("\n %i cities: \n\n", nb_villes);
  printf("Taille du reseau: %1.0f km\n",taille_reseau);

  return 0;
}
