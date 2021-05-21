// Compilation:
//    icc -std=c99 -mkl -qopenmp main.c citiesReader.c
//    icc -std=c99 -mkl -qopenmp -qopt-report=1 -qopt-report-annotate=html main.c citiesReader.c
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

#define R 6378
#define pi 3.14
#define conversion pi/180

inline float poids(ListOfCities* cities, int a, int b){
  if(a==b)return 0;
  return R*acos( sin(cities->lat[a] * conversion)*sin(cities->lat[b]*conversion) + cos((cities->lon[a] - cities->lon[b])*conversion)*cos(cities->lat[a]*conversion)*cos(cities->lat[b]*conversion) );
}

int main() {

  bool version;
  printf("Classic or Variant?"); //0 for classic version, 1 for variant version
  scanf("%i",&version);

  
  //Version classique
  if (version == 0){
    int popMin;
    
    printf("Minimal population? ");
    scanf("%i", &popMin);

    ListOfCities* cities;
    float taille_reseau = 0.0;
    int nb_villes = 0;
    double duration = 0;
  
    cities = citiesReader_classic(popMin);
    nb_villes += cities->number;
//-----------------------------------------------------------------
//--- COMPUTING graph
//-----------------------------------------------------------------

    //TEMPS DE CALCUL: entrée dans l'algo
    unsigned MKL_INT64 t0;
    mkl_get_cpu_clocks(&t0);

    // allocation des variables
    bool* dansS = malloc(cities->number*sizeof(bool));
    int* voisin = malloc(cities->number*sizeof(int));
    float* dist = malloc(cities->number*sizeof(float));
  
    //Initialisation
    dansS[0] = true; //On démarre du sommet 0
    dist[0] = 0; //dist(0,0)=0
    for(int i = 1; i < cities->number; i++){
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

      #pragma omp parallel for
      for(int j = 0; j < cities->number; j++){
        if( dansS[j] == false && dist[j] > poids(cities,i,j)){
          dist[j] = poids(cities,i,j);
          voisin[j] = i;
        }
      }
      k ++;
    }

    //TEMPS DE CALCUL: sortie de l'algo
    unsigned MKL_INT64 t1;
    mkl_get_cpu_clocks(&t1);

  // Écriture du graphe (chaque ligne correspond à une arête)
  
    FILE* fileOut = NULL;
    fileOut = fopen("resuGraph.dat", "w");
    for(int i=0; i<cities->number; i++){
      for(int j=0; j<i; j++){
        fprintf(fileOut, "%i %i\n", i, voisin[i]);
      }
    }
    fclose(fileOut);

    for(int i = 0; i < cities->number; i++){ 
      taille_reseau += poids(cities,i,voisin[i]);
      if(i==-1) printf("taille reseau = %f",taille_reseau);
    }
    

//-----------------------------------------------------------------
//--- DEALLOCATE arrays
//-----------------------------------------------------------------
    free(dansS);
    free(voisin);
    free(dist);

    free(cities->name);
    free(cities->pop);
    free(cities->lon);
    free(cities->lat);
    free(cities);

    duration = (double)(t1 - t0)/mkl_get_clocks_frequency()/1e9; 

    printf("\t%i cities: \n", nb_villes);
    printf("Taille du reseau: %1.0f km\n",taille_reseau);
    printf("Time is %fs\n ",duration);
    
    return 0;
  }

  //Version variante
  if (version == 1){
    int popMin;
    printf("Minimal population? ");
    scanf("%i", &popMin);

    // allocation des variables
    ListOfCities* cities;
    float taille_reseau = 0;
    int nb_villes = 0;
    double time = 0;
    bool* dansS = malloc(1024*sizeof(bool));
    int* voisin = malloc(1024*sizeof(int));
    float* dist = malloc(1024*sizeof(float));

    for(int dpt = 1; dpt<96; dpt++){
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
          if( dansS[j] == false && dist[j] < minDist){
            minDist = dist[j];
            i = j;
          }
        }
        dansS[i] = true;
      
        #pragma omp parallel for
        for(int j = 0; j < cities->number; j++){
          if( dansS[j] == false && dist[j] > poids(cities,i,j)){
            dist[j] = poids(cities,i,j);
            voisin[j] = i;
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
      time += (double)(t1-t0);

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
        if(dpt==0 && i ==0) printf("taille reseau dpt %i ville %i= %f\n",dpt,i,taille_reseau_dpt);
      }
      taille_reseau += taille_reseau_dpt;
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
        if( dansS[j] == false && dist[j] < minDist){
          minDist = dist[j];
          i = j;
        }
      }
      dansS[i] = true;
    
      #pragma omp parallel for
      for(int j = 0; j < cities->number; j++){
        if( dansS[j] == false && dist[j] > poids(cities,i,j)){
          dist[j] = poids(cities,i,j);
          voisin[j] = i;
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
      for(int dpt = 1; dpt < 96; dpt ++){
        char citiesFilename[64];
        sprintf(citiesFilename,"resuCities_%d.dat",dpt);
        char graphFilename[64];
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

    printf("\t%i cities: \n", nb_villes);
    printf("Taille du reseau: %1.0f km\n",taille_reseau);
    printf("Time is %fs\n ",duration);

    return 0;
  }
}
