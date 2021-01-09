#pragma once

#include "common.h"

class DbIntf
{
public:
  virtual Coordinate GetCoorByAdrress(const Address& adr, uint32_t timeLeftSec) = 0;
  virtual void PutCoordinate(const Address& adr, const Coordinate& coor) = 0;

  virtual Weather GetWeatherByCoor(const Coordinate& adr, uint32_t timeLeftSec, uint32_t distanceMetr) = 0;
  virtual void PutWeather(const Coordinate& coor, const Weather& weather) = 0;
};
using DbIntfPtr = std::shared_ptr<DbIntf>;
