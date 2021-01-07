#include "HttpClient.h"

#include <boost/asio/ssl.hpp>
#include "logger.h"


namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace ssl = boost::asio::ssl;

//https://geocode-maps.yandex.ru/1.x/?apikey=6640ee66-af25-4715-88e8-113eb9a7df7c&geocode=Москва,+Тверская+улица,+дом+7﻿



template<class = void>
void
load_root_certificates(ssl::context& ctx, boost::system::error_code& ec)
{
  std::string const cert =
    /*  This is the DigiCert root certificate.

        CN = DigiCert High Assurance EV Root CA
        OU = www.digicert.com
        O = DigiCert Inc
        C = US

        Valid to: Sunday, ?November ?9, ?2031 5:00:00 PM

        Thumbprint(sha1):
        5f b7 ee 06 33 e2 59 db ad 0c 4c 9a e6 d3 8f 1a 61 c7 dc 25
    */
    "-----BEGIN CERTIFICATE-----\n"
    "MIILlzCCCn+gAwIBAgIQEzNmdWqsjo6ZWBhxvbod4DANBgkqhkiG9w0BAQsFADBf\n"
    "MQswCQYDVQQGEwJSVTETMBEGA1UEChMKWWFuZGV4IExMQzEnMCUGA1UECxMeWWFu\n"
    "ZGV4IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MRIwEAYDVQQDEwlZYW5kZXggQ0Ew\n"
    "HhcNMTkwOTA1MTExMjM0WhcNMjAwOTA0MTExMjM0WjBmMRIwEAYDVQQDDAl5YW5k\n"
    "ZXgucnUxEzARBgNVBAoMCllhbmRleCBMTEMxDDAKBgNVBAsMA0lUTzEPMA0GA1UE\n"
    "BwwGTW9zY293MQ8wDQYDVQQIDAZSdXNzaWExCzAJBgNVBAYTAlJVMFkwEwYHKoZI\n"
    "zj0CAQYIKoZIzj0DAQcDQgAEI82TnU+s6jNrxuWTPm67XwjFuXS1I6I5vjO1Jhok\n"
    "n0/jWrD7l4/OMpIf36URXqAmQgsGgy2IjLj3SK1AoV4u3aOCCREwggkNMAwGA1Ud\n"
    "EwEB/wQCMAAwaQYDVR0fBGIwYDAvoC2gK4YpaHR0cDovL2NybHMueWFuZGV4Lm5l\n"
    "dC9jZXJ0dW0veWNhc2hhMi5jcmwwLaAroCmGJ2h0dHA6Ly95YW5kZXguY3JsLmNl\n"
    "cnR1bS5wbC95Y2FzaGEyLmNybDBxBggrBgEFBQcBAQRlMGMwLAYIKwYBBQUHMAGG\n"
    "IGh0dHA6Ly95YW5kZXgub2NzcC1yZXNwb25kZXIuY29tMDMGCCsGAQUFBzAChido\n"
    "dHRwOi8vcmVwb3NpdG9yeS5jZXJ0dW0ucGwveWNhc2hhMi5jZXIwHwYDVR0jBBgw\n"
    "FoAUN1zjGeCyjqGoTtLPq9Dc4wtcNU0wHQYDVR0OBBYEFIhadSmxLCL8t9xzmoJl\n"
    "VEhPlgmwMEwGA1UdIARFMEMwCAYGZ4EMAQICMDcGDCqEaAGG9ncCBQEKAjAnMCUG\n"
    "CCsGAQUFBwIBFhlodHRwczovL3d3dy5jZXJ0dW0ucGwvQ1BTMB0GA1UdJQQWMBQG\n"
    "CCsGAQUFBwMBBggrBgEFBQcDAjAOBgNVHQ8BAf8EBAMCB4AwggXdBgNVHREEggXU\n"
    "MIIF0IIKeWFuZGV4LmNvbYIJeWFuZGV4LmJ5ggp5YW5kZXgubmV0ggl5YW5kZXgu\n"
    "a3qCCXlhbmRleC51YYIJeWFuZGV4LnJ1gg15YW5kZXguY29tLnRyghp3d3cueG4t\n"
    "LWQxYWNwangzZi54bi0tcDFhaYIYbS54bi0tZDFhY3BqeDNmLnhuLS1wMWFpghZ4\n"
    "bi0tZDFhY3BqeDNmLnhuLS1wMWFpghN4bWxzZWFyY2gueWFuZGV4LmJ5ghBwZW9w\n"
    "bGUueWFuZGV4LmJ5gg92aWRlby55YW5kZXguYnmCEGltYWdlcy55YW5kZXguYnmC\n"
    "DXd3dy55YW5kZXguYnmCDXd3dy55YW5kZXguZnKCC20ueWFuZGV4LmZyggl5YW5k\n"
    "ZXguZnKCC20ueWFuZGV4LmVlggl5YW5kZXguZWWCEHBlb3BsZS55YW5kZXgua3qC\n"
    "C20ueWFuZGV4Lmt6ghN4bWxzZWFyY2gueWFuZGV4Lmt6gg13d3cueWFuZGV4Lmt6\n"
    "gg92aWRlby55YW5kZXgua3qCFGZhbWlseS55YW5kZXguY29tLnRygg9tLnlhbmRl\n"
    "eC5jb20udHKCFHBlb3BsZS55YW5kZXguY29tLnRyghJhaWxlLnlhbmRleC5jb20u\n"
    "dHKCFGltYWdlcy55YW5kZXguY29tLnRygg13d3cueWFuZGV4LmtnghF3d3cueWFu\n"
    "ZGV4LmNvbS5hbYILbS55YW5kZXgubWSCCXlhbmRleC5tZIILbS55YW5kZXgubHaC\n"
    "EGltYWdlcy55YW5kZXgudWGCEmdhbWUueWFuZGV4LmNvbS50coITdmlkZW8ueWFu\n"
    "ZGV4LmNvbS50coINd3d3LnlhbmRleC51YYINd3d3LnlhbmRleC51eoIPbS55YW5k\n"
    "ZXguY29tLmdlgg13d3cueWFuZGV4Lm1kggttLnlhbmRleC5ydYILbS55YW5kZXgu\n"
    "dG2CDm0ueWFuZGV4LmNvLmlsggx5YW5kZXguY28uaWyCDXd3dy55YW5kZXguZWWC\n"
    "CXlhbmRleC5sdIILbS55YW5kZXgudXqCEXd3dy55YW5kZXguY29tLmdlgg13d3cu\n"
    "eWFuZGV4LmF6ghB3d3cueWFuZGV4LmNvLmlsggttLnlhbmRleC5rZ4IRd3d3Lnlh\n"
    "bmRleC5jb20udHKCEnBsYXkueWFuZGV4LmNvbS50coIPbS55YW5kZXguY29tLmFt\n"
    "ghRnb3JzZWwueWFuZGV4LmNvbS50coIJeWFuZGV4Lmx2ghBwZW9wbGUueWFuZGV4\n"
    "LnJ1ggttLnlhbmRleC51YYIQcGVvcGxlLnlhbmRleC51YYIJeWFuZGV4LmF6gg92\n"
    "aWRlby55YW5kZXgucnWCDG0ueWFuZGV4LmNvbYIXeG1sc2VhcmNoLnlhbmRleC5j\n"
    "b20udHKCDXd3dy55YW5kZXgubHaCEGltYWdlcy55YW5kZXgucnWCE3htbHNlYXJj\n"
    "aC55YW5kZXgudWGCDXd3dy55YW5kZXgudG2CDnd3dy55YW5kZXguY29tgg13d3cu\n"
    "eWFuZGV4Lmx0ggl5YW5kZXgudG2CDXd3dy55YW5kZXgucnWCFHhtbHNlYXJjaC55\n"
    "YW5kZXguY29tgg15YW5kZXguY29tLmdlghNnYW1lcy55YW5kZXguY29tLnRyghN4\n"
    "bWxzZWFyY2gueWFuZGV4LnJ1ggttLnlhbmRleC5ieYIJeWFuZGV4LnRqgg92aWRl\n"
    "by55YW5kZXgudWGCDXd3dy55YW5kZXgudGqCC20ueWFuZGV4Lmx0ghBpbWFnZXMu\n"
    "eWFuZGV4Lmt6ghFwZW9wbGUueWFuZGV4LmNvbYISb3l1bi55YW5kZXguY29tLnRy\n"
    "ggttLnlhbmRleC5heoINeWFuZGV4LmNvbS5hbYIQdmlkZW8ueWFuZGV4LmNvbYIR\n"
    "aW1hZ2VzLnlhbmRleC5jb22CCXlhbmRleC5rZ4IJeWFuZGV4LnV6ggttLnlhbmRl\n"
    "eC50ajCCAX8GCisGAQQB1nkCBAIEggFvBIIBawFpAHcApLkJkLQYWBSHuxOizGdw\n"
    "Cjw1mAT5G9+443fNDsgN3BAAAAFtARvLPgAABAMASDBGAiEA2eu2MHUwKhvlVzVq\n"
    "nzCD7xnZRbxUDLl/aW3ESd9lb/wCIQDT+VwK5G3+0/8j8TXsWdoDbC1uyEWpoqrX\n"
    "MiksHnxJXAB2AFWB1MIWkDYBSuoLm1c8U/DA5Dh4cCUIFy+jqh0HE9MMAAABbQEb\n"
    "zNQAAAQDAEcwRQIhAOpKP3IEWNRzIvltBAT7xmd9EqpKr+vFMCSxMsLJ5dX4AiBz\n"
    "A7PGv8+O3rfhxh4JmBXoJNNW3fDGAajOPRTqZNr8+AB2AF6nc/nfVsDntTZIfdBJ\n"
    "4DJ6kZoMhKESEoQYdZaBcUVYAAABbQEby5QAAAQDAEcwRQIhAKnrUMyjFrtWPvTs\n"
    "CCPHyosSxuNmLk0MVfEbLDqZ9YHBAiBdttwYKFDN1ZxtMxqo1tLhtCfp0dhRh9/X\n"
    "PHZ8O+4xHzANBgkqhkiG9w0BAQsFAAOCAQEAK0w3Vs5KCWVERxFwuiMALR5n3r5e\n"
    "kJA3OmwFjUINqa3hkpL8qjMKgLMQpdnkQSJoAWGLvXbOJ9ypJRv14qeN55lE55rI\n"
    "T53Y5N1CPyqy1pUqXRMKZLwcXUkao1ZaRtg6wQh+8sOYg7/9psgNNXTdQOI5RPs7\n"
    "1hyx+N7b3Gh+YAe82aC5WdLJU2LvGSSgPaqrcZS91xlEgVlNx0TI0RLuhPXunytN\n"
    "DhtLXBAI1tZzs5mcAMTGqMAb/2c2EpXUWz00B3hi9fvwWIVHXkrjCsj4aEAa2Xp8\n"
    "mFPRVsOLaEF3vdiSB1J9vnPTAG2mjsoRv1RpMqg8jL8enL1BLGW4cd13kw==\n"
    "-----END CERTIFICATE-----\n"


    ;

  ctx.add_certificate_authority(
    boost::asio::buffer(cert.data(), cert.size()), ec);
  if (ec)
    return;
}

static inline
void
load_root_certificates(ssl::context& ctx)
{
  boost::system::error_code ec;
  load_root_certificates(ctx, ec);
  if (ec)
  {
    LOG_ERROR("certificate" << ec.message());
  }
}
//
//void HttpsClient::Stop()
//{
//  fStop = true;
//  if (timer_) timer_->cancel();
//}


//auto async_wait(boost::asio::deadline_timer& timer, uint32_t delayMs)
//{
//  struct Awaitable
//  {
//    boost::asio::deadline_timer& timer;
//    uint32_t delayMs;
//    boost::system::error_code ec;
//
//    ~Awaitable()
//    {
//    }
//
//    bool await_ready() const noexcept
//    {
//      return false;
//    }
//
//    void await_resume() const noexcept
//    {
//      //if (ec) throw boost::system::system_error(ec);
//    }
//
//    void await_suspend(std::coroutine_handle<> h) noexcept
//    {
//      timer.expires_from_now(boost::posix_time::milliseconds(delayMs));
//      timer.async_wait([this, h](auto ec) mutable {
//        this->ec = ec;
//        h.resume();
//        });
//    }
//
//  };
//
//  return Awaitable{ timer, delayMs };
//}

#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coro = boost::asio::this_coro;
#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

namespace httpclient
{
  class HttpClient
  {
  public:

    //
    using SslStream = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

    struct HttpsCfg
    {
      std::string url;
      std::string port;
      std::string idConn;
    };

    HttpClient(boost::asio::io_context& ioc, const HttpsCfg& cfg)
      :ioc_(ioc), stream_{ ioc_, ctx_ }, cfg_(cfg)
    {
      load_root_certificates(ctx_);
      prefixLogConn_ = "[CONN_" + cfg_.idConn + "] ";
    }

    bool IsResolve() const
    {
      return resoveResults_.size();
    }


    boost::asio::awaitable<bool> Resolve()
    {
      if (!SSL_set_tlsext_host_name(stream_.native_handle(), cfg_.url.c_str()))
      {
        boost::system::error_code ec;
        ec.assign(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
        LOG_ERROR(ec.message());
        co_return false;
      }

      tcp::resolver resolver{ ioc_ };

      try {
        resoveResults_ = co_await resolver.async_resolve(cfg_.url, cfg_.port, boost::asio::use_awaitable);

        LOG_INFO(prefixLogConn_ << "Url: " << cfg_.url << "[" << cfg_.port << "] resolved to: " <<
          resoveResults_.begin()->endpoint());
      }
      catch (std::exception& e) {
        LOG_ERROR(prefixLogConn_ << "Resolve " << cfg_.url << " failed. Exception: " << e.what());
        co_return  false;
      }

      co_return true;
    }

    boost::asio::awaitable<bool> Connect()
    {
      try {
        LOG_INFO("Try to connect: " << cfg_.url << "[" << cfg_.port << "]");

        const auto ret = co_await boost::asio::async_connect(stream_.next_layer(), resoveResults_.begin(),
          resoveResults_.end(), boost::asio::use_awaitable);

        LOG_INFO(prefixLogConn_ << "Try handshake: " << cfg_.url << "[" << cfg_.port << "]");

        co_await stream_.async_handshake(ssl::stream_base::client, boost::asio::use_awaitable);

      }
      catch (std::exception& e) {
        LOG_ERROR(prefixLogConn_ << "Connecting " << cfg_.url << "[" << cfg_.port << "]" << " failed. Exception: " << e.what());
        co_return  false;
      }

      LOG_INFO(prefixLogConn_ << "Connected: " << cfg_.url << "[" << cfg_.port << "]");

      co_return true;
    }

    boost::asio::awaitable<void> Disconnect()
    {
      try {
        co_await stream_.async_shutdown(boost::asio::use_awaitable);
      }
      catch (std::exception& e) {
        LOG_ERROR(prefixLogConn_ << "Disconnect " << cfg_.url << "[" << cfg_.port << "]" << " failed. Exception: " << e.what());
      }
      co_return;
    }

    boost::asio::awaitable<std::pair<bool, http::response<http::dynamic_body>>> SendRecvAsync(const http::request<http::string_body>& req)
    {
      http::response<http::dynamic_body> res;

      try {
        LOG_ERROR(prefixLogConn_ << "Send request : " << req);
        co_await http::async_write(stream_, req, boost::asio::use_awaitable);
      }
      catch (std::exception& e) {
        LOG_ERROR(prefixLogConn_ << "Send request to " << cfg_.url << "[" << cfg_.port << "]" << " failed. Exception: " << e.what());
        co_return std::pair{ false, res };
      }

      try {
        boost::beast::flat_buffer b;
        LOG_ERROR(prefixLogConn_ << "Try read answer");
        co_await http::async_read(stream_, b, res, boost::asio::use_awaitable);
        LOG_ERROR(prefixLogConn_ << res);
      }
      catch (std::exception& e) {
        LOG_ERROR(prefixLogConn_ << "Recieve answer from " << cfg_.url << "[" << cfg_.port << "]" << " failed. Exception: " << e.what());
        co_return std::pair{ false, res };
      }

      co_return std::pair{ true, res };
    }

  private:
    boost::asio::io_context& ioc_;
    boost::asio::ssl::context ctx_{ boost::asio::ssl::context::sslv23_client };
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream_;
    HttpsCfg cfg_;
    std::string prefixLogConn_;
    std::atomic_bool fStop_ = false;

    tcp::resolver::results_type resoveResults_;
  };
  using HttpClientPtr = std::shared_ptr<HttpClient>;




  ManagerConn::ManagerConn(const Config& cfg, const Dependencies& dep)
    :cfg_(cfg), dep_(dep)
  {
    prefixLogConn_ = "[" + cfg_.nameClient + "] ";
  }

  void ManagerConn::Init()
  {
    auto cntClient = cfg_.numConn;

    HttpClient::HttpsCfg cfg;
    //cfg.url = "geocode-maps.yandex.ru";
    //cfg.port = "443";
    cfg.url = cfg_.url;
    cfg.port = cfg_.port;

    while (cntClient--)
    {
      cfg.idConn = cfg_.nameClient + "_" + std::to_string(cntClient);
      auto httpClient = std::make_shared<HttpClient>(dep_.ioc, cfg);
      boost::asio::co_spawn(dep_.ioc, Connect(httpClient), detached);
    }
  }

  boost::asio::awaitable<http::response<http::dynamic_body>> ManagerConn::SendRecvAsync(const http::request<http::string_body>& req)
  {
    auto client = co_await GetClient();
    if (!client)
    {
      LOG_ERROR(prefixLogConn_ << "Cant get HttpClient");
      http::response<http::dynamic_body> res;
      co_return res;
    }

    auto [result, answer] = co_await client->SendRecvAsync(req);

    if (!result)
    {
      boost::asio::co_spawn(dep_.ioc, Reconnect(client), detached);
    }
    else
    {
      std::lock_guard l(mut_);
      httpClients_.push_back(std::move(client));
    }

    co_return answer;
  }

  boost::asio::awaitable<HttpClientPtr> ManagerConn::GetClient()
  {
    boost::asio::deadline_timer timer(dep_.ioc);

    HttpClientPtr client;
    static const uint32_t timeDelayMs = 10;
    int32_t timeTotalWaitMs = 3000;

    while (!fEnd_)
    {

      {
        std::lock_guard l(mut_);
        if (!httpClients_.empty())
        {
          client = *httpClients_.begin();
          httpClients_.pop_front();
        }
      }
      if (client) break;

      timer.expires_from_now(boost::posix_time::milliseconds(timeDelayMs));
      co_await timer.async_wait(boost::asio::use_awaitable);

      timeTotalWaitMs -= timeDelayMs;
      if (timeTotalWaitMs <= 0) break;
    }

    co_return client;
  }

  boost::asio::awaitable<void> ManagerConn::Reconnect(HttpClientPtr httpClient)
  {
    co_await httpClient->Disconnect();
    co_await Connect(httpClient);

    co_return;
  }

  boost::asio::awaitable<void> ManagerConn::Disconnect(HttpClientPtr httpClient)
  {
    co_await httpClient->Disconnect();
    co_return;
  }

  boost::asio::awaitable<void> ManagerConn::Connect(HttpClientPtr httpClientArg)
  {

    HttpClientPtr httpClient(httpClientArg);
    boost::asio::deadline_timer timer(dep_.ioc);

    while (!fEnd_)
    {
      if (httpClient->IsResolve()) break;
      if (co_await httpClient->Resolve()) break;

      timer.expires_from_now(boost::posix_time::milliseconds(1000));
      co_await timer.async_wait(boost::asio::use_awaitable);
    }

    if (fEnd_) co_return;

    while (!fEnd_)
    {
      if (co_await httpClient->Connect()) break;

      timer.expires_from_now(boost::posix_time::milliseconds(5000));
      co_await timer.async_wait(boost::asio::use_awaitable);
    }

    {
      std::lock_guard l(mut_);
      if (fEnd_) co_return;
      httpClients_.push_back(std::move(httpClient));
    }

    co_return;
  }

  void ManagerConn::Stop()
  {
    while(1)
    {
      std::lock_guard l(mut_);
      fEnd_ = true;
      if (!httpClients_.empty())
      {
        auto client = *httpClients_.begin();
        httpClients_.pop_front();
        boost::asio::co_spawn(dep_.ioc, Disconnect(client), detached);
      }
      else break;
    }
  }
}


#include "Executor.h"

boost::asio::awaitable<void> ProcessConn(boost::asio::io_context& ioc, httpclient::ManagerConn& man, int i)
{
  boost::asio::deadline_timer timer(ioc);

  LOG_INFO(i << " Start process");
  timer.expires_from_now(boost::posix_time::milliseconds(2000));
  co_await timer.async_wait(boost::asio::use_awaitable);

  LOG_INFO(i << " send req");

  std::string url = std::string("/1.x/") +
    "?apikey=" + "6640ee66-af25-4715-88e8-113eb9a7df7c" +
    "&format=json" +
    "&geocode=" +
    "Москва,+Тверская+улица,+дом+7﻿";
  //https://geocode-maps.yandex.ru/1.x/?apikey=6640ee66-af25-4715-88e8-113eb9a7df7c&geocode=Москва,+Тверская+улица,+дом+7﻿

  //http::request<http::string_body> req{ http::verb::get, url , 11 };
  //req.set(http::field::host, "geocode-maps.yandex.ru");
  //req.set(http::field::user_agent, "BEAST");
  //req.set(http::field::connection, "keep-alive");
  http::request<http::string_body> req;
  auto res = co_await man.SendRecvAsync(req);

  LOG_INFO(i << " response : \n" << res);

  co_return;
}

void HttpStart()
{
  boost::asio::io_context ioc;
  httpclient::ManagerConn::Dependencies dep(ioc);
  httpclient::ManagerConn::Config cfg;
  httpclient::ManagerConn man(cfg, dep);
  man.Init();

  for (int i = 0; i < 2; i++)
    boost::asio::co_spawn(ioc, ProcessConn(ioc, man, i), detached);

  ioc.run();
}