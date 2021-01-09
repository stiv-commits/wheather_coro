#include "InMemoryDb.h"

#include <unordered_map>
#include <tuple>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>

#include <boost/geometry/index/rtree.hpp>

#define prefixLogConn_ "[IN_MEM_DB]"

namespace
{
  template<typename... Args>
  size_t MakeHash(std::tuple<Args...> const& value)
  {
    auto tupleHash = []<typename Tuple, std::size_t... ids>(Tuple const& tuple, std::index_sequence<ids...> const&)
    {
      size_t result = 0;
      auto sumHash = [&result](auto val)
      {
        result += std::hash<decltype(val)>{}(val);
      };

      (sumHash(std::get<ids>(tuple)), ...);

      return result;
    };

    return tupleHash(value, std::make_index_sequence<sizeof...(Args)>());
  }
}

namespace std {

  template <>
  struct hash<Address>
  {
    std::size_t operator()(const Address& adr) const
    {
      auto tupleAdr = std::make_tuple(std::string_view(adr.city), std::string_view(adr.street), std::string_view(adr.house));
      return ::MakeHash(tupleAdr);
    }
  };

}

bool operator==(const Address& left, const Address& right) 
{
  return (left.city == right.city
    && left.house == right.house
    && left.street == right.street);
}

class InMemoryDb::CoorByAdressStorage
{
public:

  Coordinate GetCoorByAdrress(const Address& adr, uint32_t timeLeftSec)
  {
    std::shared_lock lock(m_);
    auto it = storage_.find(adr);
    if (it != storage_.end())
    {
      const auto& val = it->second;
      auto nowTs = time(nullptr);
      if (val.ts + timeLeftSec > nowTs) return val.coor;
    }
    return Coordinate{};
  }

  void PutCoordinate(const Address& adr, const Coordinate& coor)
  {
    std::unique_lock lock(m_);
    storage_.insert_or_assign(adr, Value{ coor, time(nullptr) });
  }

private:

  struct Value
  {
    Coordinate coor;
    time_t ts;
  };
  std::unordered_map<Address, Value> storage_;
  std::shared_mutex m_;

};

class InMemoryDb::WeatherByCoorStorage // TODO Must implemente RTree
{
public:

  Weather GetWeatherByCoor(const Coordinate& adr, uint32_t timeLeftSec, uint32_t distanceMetr)
  {
    return Weather{};
  }
  void PutWeather(const Coordinate& coor, const Weather& weather)
  {

  }
private:

};


InMemoryDb::InMemoryDb(const Config& cfg, const Dependencies& dep)
  :cfg_(cfg), 
  dep_(dep), 
  coorByAdressStorage_(std::make_unique<CoorByAdressStorage>()),
  weatherByCoorStorage_(std::make_unique<WeatherByCoorStorage>())
{

}

InMemoryDb::~InMemoryDb()
{
}

Coordinate InMemoryDb::GetCoorByAdrress(const Address& adr, uint32_t timeLeftSec)
{
  return coorByAdressStorage_->GetCoorByAdrress(adr, timeLeftSec);
}

void InMemoryDb::PutCoordinate(const Address& adr, const Coordinate& coor)
{
  coorByAdressStorage_->PutCoordinate(adr, coor);
}

Weather InMemoryDb::GetWeatherByCoor(const Coordinate& adr, uint32_t timeLeftSec, uint32_t distanceMetr)
{
  return weatherByCoorStorage_->GetWeatherByCoor(adr, timeLeftSec, distanceMetr);
}

void InMemoryDb::PutWeather(const Coordinate& coor, const Weather& weather)
{
}