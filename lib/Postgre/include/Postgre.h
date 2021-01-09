#pragma once

#include "DbIntf.h"

#include <memory>

class Postgre : public DbIntf
{
public:
  struct Dependencies
  {
  };

  struct Config
  {
    std::string ip;
    std::string port;
    std::string login;
    std::string passwd;
  };

  Postgre(const Config& cfg, const Dependencies& dep);
  virtual ~Postgre();

  Coordinate GetCoorByAdrress(const Address& adr, uint32_t timeLeftSec) override;
  void PutCoordinate(const Address& adr, const Coordinate& coor) override;

  Weather GetWeatherByCoor(const Coordinate& adr, uint32_t timeLeftSec, uint32_t distanceMetr) override;
  void PutWeather(const Coordinate& coor, const Weather& weather) override;

private:

  Config cfg_;
  Dependencies dep_;
};