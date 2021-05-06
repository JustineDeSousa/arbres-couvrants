#ifndef CITIES_READER
#define CITIES_READER



typedef struct ListOfCities ListOfCities;

struct ListOfCities
{
  int    number;
  char** name;
  int*   pop;
  float* lon;
  float* lat;
};

ListOfCities* citiesReader(int popMin);
float poids(ListOfCities* cities, int a, int b);

#endif
