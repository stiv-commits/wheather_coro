#pragma once

#include "DbIntf.h"

#include <memory>

class InMemoryDb : public DbIntf
{
public:
  struct Dependencies
  {
  };

  struct Config
  {
  };

  InMemoryDb(const Config& cfg, const Dependencies& dep);
  virtual ~InMemoryDb();

  Coordinate GetCoorByAdrress(const Address& adr, uint32_t timeLeftSec) override;
  void PutCoordinate(const Address& adr, const Coordinate& coor) override;

  Weather GetWeatherByCoor(const Coordinate& adr, uint32_t timeLeftSec, uint32_t distanceMetr) override;
  void PutWeather(const Coordinate& coor, const Weather& weather) override;

private:

  Config cfg_;
  Dependencies dep_;

  class CoorByAdressStorage;
  std::unique_ptr<CoorByAdressStorage> coorByAdressStorage_;

  class WeatherByCoorStorage;
  std::unique_ptr<WeatherByCoorStorage> weatherByCoorStorage_;

};