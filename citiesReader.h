#ifndef CITIES_READER
#define CITIES_READER

typedef struct ListOfCities ListOfCities;

struct ListOfCities
{
  int    number;
  int*    dpt;
  char** name;
  int*   pop;
  float* lon;
  float* lat;
};

ListOfCities* citiesReader(int popMin, int dpt);
ListOfCities* bigcitiesReader();

float poids(ListOfCities* cities, int a, int b);

#endif
