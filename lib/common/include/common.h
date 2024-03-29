#pragma once

#include <string>
#include <iostream>
#include <iomanip>

struct Address
{
  std::string city;
  std::string street;
  std::string house;

  inline bool Empty() { return city.empty() || street.empty() || house.empty(); }
};

inline std::ostream& operator << (std::ostream& str, const Address& adr)
{
  str << "City: " << adr.city << " , street: " << adr.street << " , house: " << adr.house << " ";
  return str;
}

struct Coordinate
{
  float lat = 0;
  float lon = 0;
  inline bool Empty() { return !lat || !lon; }
};
inline std::ostream& operator << (std::ostream& str, const Coordinate& coor)
{
  typedef std::numeric_limits< float > flt;

  str << "lattitude: " << std::setprecision(flt::max_digits10) << coor.lat << " , longitude: " << coor.lon << " ";
  return str;
}

struct Weather
{
  std::string data;
  inline bool Empty() const { return data.empty(); }
};
inline std::ostream& operator << (std::ostream& str, const Weather& weath)
{
  std::string_view coorout;
  if (!weath.Empty())
  {
    if (weath.data.size() > 30) coorout = std::string_view(weath.data.data(), 30);
    else coorout = std::string_view(weath.data.data(), weath.data.size());
  }
  str << "Weather: " << std::endl << coorout << "...";
  return str;
}

std::string url_decode(std::string const& str);
std::string url_encode(const std::string& str);