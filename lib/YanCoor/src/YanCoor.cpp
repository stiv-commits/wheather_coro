#include "YanCoor.h"
#include "HttpClient.h"
#include "Common.h"
#include "logger.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cstdlib>
#include <algorithm>

namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace ssl = boost::asio::ssl;

//https://geocode-maps.yandex.ru/1.x/?apikey=6640ee66-af25-4715-88e8-113eb9a7df7c&geocode=Москва,+Тверская+улица,+дом+7﻿


namespace
{
  std::string GetUrl(const Address& adr, const std::string& key)
  {
    std::string url = std::string("/1.x/") +
      "?apikey=" + key +
      "&format=json" +
      "&geocode=" +
      url_encode(adr.city + "+" + adr.street + "+" + adr.house);

    return url;
  }


  Coordinate ParseResponse(http::response<http::dynamic_body> res, std::string& err)
  {
    Coordinate coor;
    if (res.result() == http::status::ok && res.body().size())
    {
      boost::property_tree::ptree pt;

      std::string body(boost::beast::buffers_to_string(res.body().data()));
      std::stringstream ss;
      ss << body;


      boost::property_tree::json_parser::read_json(ss, pt);
      try
      {
        auto GeoObjectCollection = pt.get_child("response.GeoObjectCollection");
        int found = GeoObjectCollection.get<int>("metaDataProperty.GeocoderResponseMetaData.found");
        if (found)
        {
          auto featureMember = GeoObjectCollection.get_child("featureMember");
          auto point = featureMember.begin()->second.get_child("GeoObject.Point");
          std::string pos = point.get<std::string>("pos");

          auto endlat = pos.find(' ');

          coor.lon = static_cast<float>(atof(pos.c_str()));
          coor.lat = static_cast<float>(atof(&pos[endlat]));
        }
      }
      catch (const boost::property_tree::ptree_error& e)
      {
        err = e.what();
      }

    }

    return coor;
  }

}

YanCoorClient::YanCoorClient(const Config& cfg, const Dependencies& dep)
  : cfg_(cfg), dep_(dep)
{
  prefixLogConn_ = "[YANCOOR] ";

  httpclient::ManagerConn::Config cfgclient;
  cfgclient.numConn = cfg_.numConnection;
  cfgclient.url = cfg_.url;
  cfgclient.port = cfg_.port;
  cfgclient.nameClient = "YANCOOR";

  httpclient::ManagerConn::Dependencies depclient(dep_.ioc);

  httpsClient_ = std::make_shared<httpclient::ManagerConn>(cfgclient, depclient);
}

YanCoorClient::~YanCoorClient()
{
  LOG_DEBUG(prefixLogConn_ << "YanCoorClient::~YanCoorClient " << this);
}

bool YanCoorClient::Init()
{
  httpsClient_->Init();

  return true;
}

boost::asio::awaitable<Coordinate> YanCoorClient::GetCoordinate(const Address& adr)
{
  LOG_DEBUG(prefixLogConn_<< "Try get coordinate by address : " << adr);

  namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

  http::request<http::string_body> req{ http::verb::get, GetUrl(adr, cfg_.key) , 10 };
  req.set(http::field::host, cfg_.url);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::connection, "keep-alive");

  auto res = co_await httpsClient_->SendRecvAsync(req);

  std::string error;
  Coordinate coor = ParseResponse(res, error);
  if (coor.Empty())
  {
    LOG_ERROR(prefixLogConn_ << "Cant get koordinate, error parse answer: " << error);
    co_return coor;
  }

  LOG_DEBUG(prefixLogConn_ << "Get Ok , coordinate : " << coor);

  co_return coor;
}


void YanCoorClient::Stop()
{
  httpsClient_->Stop(); 
}

