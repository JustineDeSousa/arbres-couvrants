#include "citiesReader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

ListOfCities* citiesReader(int popMin, int dpt, char* filename){

  ListOfCities* cities = malloc(sizeof(ListOfCities));

//----------------------------------------------------------------------
//--- READING cities with population greater than or equal to 'popMin'
//--- and which department == 'dpt' (si dpt == 0, lit pour tous les dpt)
//----------------------------------------------------------------------

  printf("== Reading cities with population >= %i and department = %i from 'citiesList.csv' ==\n", popMin,dpt);

  FILE* inputFile = NULL;
  inputFile = fopen("citiesList.csv", "r");
  if(inputFile != NULL){

    char line[512];
    const char s[2] = ",";
    char *token;
    
    // Count the number of cities with population greater than or equal to 'popMin'
    cities->number = 0;
    while(fgets(line, 512, inputFile) != NULL){
      token = strtok(line, s);
      token = strtok(NULL, s);
      int myDpt = atoi(token);
      for(int i=0; i<13; i++){
        token = strtok(NULL, s);
      }
      int myPop = atoi(token);
      if( ((myDpt == dpt) || (dpt==0)) && myPop >= popMin){
        cities->number++;  
      }
      
    }
    fseek(inputFile, 0, SEEK_SET);

    // Allocate arrays
    cities->dpt  = malloc(cities->number*sizeof(int));
    cities->name = malloc(cities->number*sizeof(char*));
    cities->pop  = malloc(cities->number*sizeof(int));
    cities->lon  = malloc(cities->number*sizeof(float));
    cities->lat  = malloc(cities->number*sizeof(float));
    
    // Reading/Saving data
    int index = 0;
    char *myName = malloc(32*sizeof(char));
    while(fgets(line, 512, inputFile) != NULL){
      //printf("%s\n",line);
      token = strtok(line, s);
      token = strtok(NULL, s);
      int myDpt = atoi(token);
      for(int i=0; i<2;  i++) token = strtok(NULL, s);
      strncpy(myName, token, 32);
      for(int i=0; i<11; i++) token = strtok(NULL, s);
      int myPop = atoi(token);
      for(int i=0; i<5;  i++) token = strtok(NULL, s);
      float myLon = atof(token);
      for(int i=0; i<1;  i++) token = strtok(NULL, s);
      float myLat = atof(token);
      
      if( ((myDpt == dpt)||(dpt == 0)) && (myPop >= popMin) ){
        cities->dpt[index] = myDpt;
        cities->name[index] = (char*) malloc(32*sizeof(char));
        strncpy(cities->name[index], myName, 32);
        cities->pop[index] = myPop;
        cities->lon[index] = myLon;
        cities->lat[index] = myLat;
        index++;
      }
    }
    free(myName);
    fclose(inputFile);
  }
//--------------------------------------------------------------------
//--- WRITING cities with population greater than or equal to 'popMin'
//--------------------------------------------------------------------

  printf("== Writing cities with population >= %i and department = %i in '%s' ==\n", popMin,dpt,filename);

  FILE* outputFile = NULL;
  outputFile = fopen(filename, "w");
  if(outputFile != NULL){
    for(int i=0; i<cities->number; i++){
      fprintf(outputFile, "%i %f %f\n", cities->pop[i], cities->lon[i], cities->lat[i]);
    }
    fclose(outputFile);
  }

  return cities;
}




ListOfCities* bigcitiesReader(){

  ListOfCities* cities = malloc(sizeof(ListOfCities));

//--------------------------------------------------------------------
//--- READING the largest city of every provinces
//--------------------------------------------------------------------

  printf("== Reading the biggest city of every province ==\n");
  FILE* inputFile = NULL;
  inputFile = fopen("citiesList.csv", "r");
  if(inputFile != NULL){

    char line[512];
    const char s[2] = ",";
    char *token;
    
    cities->number = 94; //There are 94 provinces in the interior of France

    // Allocate arrays
    cities->dpt  = malloc(cities->number*sizeof(int));
    cities->name = malloc(cities->number*sizeof(char*));
    cities->pop  = malloc(cities->number*sizeof(int));
    cities->lon  = malloc(cities->number*sizeof(float));
    cities->lat  = malloc(cities->number*sizeof(float));
    
    // Reading/Saving data
    int index = 0;
    int Dpt = 1;
    int popMax = 0;
    float Lon = 0.0;
    float Lat = 0.0;
    char Name[32];
    char lastcity[10];
    strcpy(lastcity,"FOSSES"); //the last city of the list
    char *myName = malloc(32*sizeof(char));
    while(fgets(line, 512, inputFile) != NULL){
      token = strtok(line, s);
      token = strtok(NULL, s);
      int myDpt = atoi(token);
      for(int i=0; i<2;  i++) token = strtok(NULL, s);
      strncpy(myName, token, 32);
      for(int i=0; i<11; i++) token = strtok(NULL, s);
      int myPop = atoi(token);
      for(int i=0; i<5;  i++) token = strtok(NULL, s);
      float myLon = atof(token);
      for(int i=0; i<1;  i++) token = strtok(NULL, s);
      float myLat = atof(token);
      
      //the largest city of the province
      if (myDpt == Dpt){
        if (myPop >= popMax){
          popMax = myPop;
          Lon = myLon;
          Lat = myLat;
          strcpy(Name, myName);
        }
      }

      //put the detail of the largest city of the province in the list
      if (myDpt != Dpt){
        cities->dpt[index] = Dpt;
        cities->name[index] = (char*) malloc(32*sizeof(char));
        strncpy(cities->name[index], Name, 32);
        cities->pop[index] = popMax;
        cities->lon[index] = Lon;
        cities->lat[index] = Lat;
        index++;
        popMax = 0;
        Lon = 0.0;
        Lat = 0.0;
        Dpt += 1;
        if (Dpt == 20){
          Dpt += 1;
        }
      }
      
      // read the detail of Paris
      if (myDpt == 75){
        cities->dpt[index] = myDpt;
        cities->name[index] = (char*) malloc(32*sizeof(char));
        strncpy(cities->name[index], myName, 32);
        cities->pop[index] = myPop;
        cities->lon[index] = myLon;
        cities->lat[index] = myLat;
        index++;
        Dpt++;
      }

      //if all the cities has been read
      if (strcmp(lastcity,myName)==0){
        cities->dpt[index] = Dpt;
        cities->name[index] = (char*) malloc(32*sizeof(char));
        strncpy(cities->name[index], Name, 32);
        cities->pop[index] = popMax;
        cities->lon[index] = Lon;
        cities->lat[index] = Lat;
      }
    }
    free(myName);
    fclose(inputFile);
  }

//--------------------------------------------------------------------
//--- WRITING largest cities of every provinces
//--------------------------------------------------------------------

  printf("== Writing largest cities of every provinces in 'resuCities.dat' ==\n");

  FILE* outputFile = NULL;
  outputFile = fopen("resuBigCities.dat", "w");
  if(outputFile != NULL){
    for(int i=0; i<cities->number; i++){
      fprintf(outputFile, "%i %f %f\n", cities->pop[i], cities->lon[i], cities->lat[i]);
    }
    fclose(outputFile);
  }

  return cities;

}


float poids(ListOfCities* cities, int a, int b){
  int R = 6378;
  float pi = 3.14;
  float conversion = pi/180;
  return R*acos( sin(cities->lat[a] * conversion)*sin(cities->lat[b]*conversion) + cos((cities->lon[a] - cities->lon[b])*conversion)*cos(cities->lat[a]*conversion)*cos(cities->lat[b]*conversion) );
}
