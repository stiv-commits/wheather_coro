#include "WeatherClient.h"
#include "HttpClient.h"
#include "logger.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/version.hpp>

namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace ssl = boost::asio::ssl;

//api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={your api key}

namespace
{
  //api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={your api key}
  //api.openweathermap.org/data/2.5/weather?q=London,uk&APPID=eaef71f556bb19737788ab3cb4bd757e
  std::string GetUrl(const Coordinate& coord, const std::string& key)
  {
    std::string url;
    url = "/data/2.5/weather?";
    url += "lat=" + std::to_string(coord.lat) +
      "&lon=" + std::to_string(coord.lon) +
      "&appid=" + key;

    return url;
  }

  //{"coord": { "lon": 139,"lat": 35},
  //  "weather": [
  //    {
  //      "id": 800,
  //      "main": "Clear",
  //      "description": "clear sky",
  //      "icon": "01n"
  //    }
  //  ],
  //  "base": "stations",
  //  "main": {
  //    "temp": 281.52,
  //    "feels_like": 278.99,
  //    "temp_min": 280.15,
  //    "temp_max": 283.71,
  //    "pressure": 1016,
  //    "humidity": 93
  //  },
  //  "wind": {
  //    "speed": 0.47,
  //    "deg": 107.538
  //  },
  //  "clouds": {
  //    "all": 2
  //  },
  //  "dt": 1560350192,
  //  "sys": {
  //    "type": 3,
  //    "id": 2019346,
  //    "message": 0.0065,
  //    "country": "JP",
  //    "sunrise": 1560281377,
  //    "sunset": 1560333478
  //  },
  //  "timezone": 32400,
  //  "id": 1851632,
  //  "name": "Shuzenji",
  //  "cod": 200
  //}
  Weather ParseWeatherResponse(const http::response<http::dynamic_body>& res)
  {
    Weather weather;
    if (res.result() == http::status::ok)
    {
      weather.data = boost::beast::buffers_to_string(res.body().data());
    }
    return weather;
  }
}
WeatherClient::WeatherClient(const Config& cfg, const Dependencies& dep)
  : cfg_(cfg), dep_(dep)
{
  prefixLogConn_ = "[OPENWEATHER] ";

  httpclient::ManagerConn::Config cfgclient;
  cfgclient.numConn = cfg_.numConnection;
  cfgclient.url = cfg_.url;
  cfgclient.port = cfg_.port;
  cfgclient.nameClient = "OPENWEATHER";

  httpclient::ManagerConn::Dependencies depclient(dep_.ioc);

  httpsClient_ = std::make_shared<httpclient::ManagerConn>(cfgclient, depclient);
}

boost::asio::awaitable <Weather> WeatherClient::GetWeather(const Coordinate& coord)
{
  LOG_DEBUG(prefixLogConn_ << "Try get weather by coordinate : " << coord);
  namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

  http::request<http::string_body> req{ http::verb::get, GetUrl(coord, cfg_.key) , 11 };
  req.set(http::field::host, cfg_.url);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::connection, "keep-alive");

  auto res = co_await httpsClient_->SendRecvAsync(req);

  Weather weather = ParseWeatherResponse(res);
  if (weather.Empty())
  {
    LOG_ERROR(prefixLogConn_ << "Cant get weather" );
    co_return weather;
  }

  LOG_DEBUG(prefixLogConn_ << "Get Ok , weather: " << weather);

  co_return weather;
}

bool WeatherClient::Init()
{
  httpsClient_->Init();

  return true;
}

void WeatherClient::Stop()
{
  httpsClient_->Stop();
}


