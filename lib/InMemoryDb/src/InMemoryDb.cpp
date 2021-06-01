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

namespace bg = boost::geometry;
namespace bgm = bg::model;
namespace bgi = bg::index;

typedef bg::model::point<double, 2, bg::cs::cartesian> point;

struct WheatheRef {
  Weather weather;
  Coordinate coor;
  time_t time;
  point location;
};

template <>
struct bgi::indexable<WheatheRef>
{
  typedef point result_type;
  point operator()(const WheatheRef& c) const { return c.location; }
};

class InMemoryDb::WeatherByCoorStorage
{

public:

  Weather GetWeatherByCoor(const Coordinate& coor, uint32_t timeLeftSec, uint32_t distanceMetr)
  {
    point pt(coor.lat, coor.lon);

    auto it = rtree_.qbegin(bgi::nearest(pt, 1000));
    if (it != rtree_.qend())
    {
      auto dist = CalcGPSDistance(it->coor.lat, it->coor.lon, coor.lat, coor.lon);

      if (dist > distanceMetr) return Weather{};

      if (time(nullptr) > it->time + timeLeftSec) return Weather{};

      return it->weather;
    }

    return Weather{};
  }

  void PutWeather(const Coordinate& coor, const Weather& weather)
  {
    WheatheRef we;
    we.location = { coor.lat, coor.lon };
    we.weather = weather;
    we.coor = coor;
    we.time = time(nullptr);
    rtree_.insert(std::move(we));
  }

private:

  const double PI = 3.14159265358979323846;
  const double RADIO_TERRESTRE = 6372797.56085;
  const double GRADOS_RADIANES = PI / 180;

  float CalcGPSDistance(float latitud1, float longitud1, float latitud2, float longitud2) {
    double haversine;
    double temp;
    double distancia_puntos;
    
    using namespace std;

    auto latitud1Dob = latitud1 * GRADOS_RADIANES;
    auto longitud1Dob = longitud1 * GRADOS_RADIANES;
    auto latitud2Dob = latitud2 * GRADOS_RADIANES;
    auto longitud2Dob = longitud2 * GRADOS_RADIANES;

    haversine = (pow(sin((1.0 / 2) * (latitud2Dob - latitud1Dob)), 2)) + ((cos(latitud1Dob)) * (cos(latitud2Dob)) * (pow(sin((1.0 / 2) * (longitud2Dob - longitud1Dob)), 2)));
    temp = 2 * asin(min(1.0, sqrt(haversine)));
    distancia_puntos = RADIO_TERRESTRE * temp;

    return static_cast<float>(distancia_puntos);
  }

  typedef bgi::rtree< WheatheRef, bgi::linear<16> > rtree_t;
  rtree_t rtree_;

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
  return weatherByCoorStorage_->PutWeather(coor, weather);
}