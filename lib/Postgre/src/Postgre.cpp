#include "Postgre.h"


Postgre::Postgre(const Config& cfg, const Dependencies& dep)
  :cfg_(cfg), 
  dep_(dep)
{
}

Postgre::~Postgre()
{
}

Coordinate Postgre::GetCoorByAdrress(const Address& adr, uint32_t timeLeftSec)
{
  return Coordinate{};
}

void Postgre::PutCoordinate(const Address& adr, const Coordinate& coor)
{
}

Weather Postgre::GetWeatherByCoor(const Coordinate& adr, uint32_t timeLeftSec, uint32_t distanceMetr)
{
  return Weather{};
}

void Postgre::PutWeather(const Coordinate& coor, const Weather& weather)
{
}