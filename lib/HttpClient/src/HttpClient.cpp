#include "HttpClient.h"

#include <boost/asio/ssl.hpp>
#include "logger.h"
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>
namespace ssl = boost::asio::ssl;

//https://geocode-maps.yandex.ru/1.x/?apikey=6640ee66-af25-4715-88e8-113eb9a7df7c&geocode=Москва,+Тверская+улица,+дом+7﻿


//
//template<class = void>
//void
//load_root_certificates(ssl::context& ctx, boost::system::error_code& ec)
//{
//  std::string const cert =
//    /*  This is the DigiCert root certificate.
//
//        CN = DigiCert High Assurance EV Root CA
//        OU = www.digicert.com
//        O = DigiCert Inc
//        C = US
//
//        Valid to: Sunday, ?November ?9, ?2031 5:00:00 PM
//
//        Thumbprint(sha1):
//        5f b7 ee 06 33 e2 59 db ad 0c 4c 9a e6 d3 8f 1a 61 c7 dc 25
//    */
//    "-----BEGIN CERTIFICATE-----\n"
//    "MIILlzCCCn+gAwIBAgIQEzNmdWqsjo6ZWBhxvbod4DANBgkqhkiG9w0BAQsFADBf\n"
//    "MQswCQYDVQQGEwJSVTETMBEGA1UEChMKWWFuZGV4IExMQzEnMCUGA1UECxMeWWFu\n"
//    "ZGV4IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MRIwEAYDVQQDEwlZYW5kZXggQ0Ew\n"
//    "HhcNMTkwOTA1MTExMjM0WhcNMjAwOTA0MTExMjM0WjBmMRIwEAYDVQQDDAl5YW5k\n"
//    "ZXgucnUxEzARBgNVBAoMCllhbmRleCBMTEMxDDAKBgNVBAsMA0lUTzEPMA0GA1UE\n"
//    "BwwGTW9zY293MQ8wDQYDVQQIDAZSdXNzaWExCzAJBgNVBAYTAlJVMFkwEwYHKoZI\n"
//    "zj0CAQYIKoZIzj0DAQcDQgAEI82TnU+s6jNrxuWTPm67XwjFuXS1I6I5vjO1Jhok\n"
//    "n0/jWrD7l4/OMpIf36URXqAmQgsGgy2IjLj3SK1AoV4u3aOCCREwggkNMAwGA1Ud\n"
//    "EwEB/wQCMAAwaQYDVR0fBGIwYDAvoC2gK4YpaHR0cDovL2NybHMueWFuZGV4Lm5l\n"
//    "dC9jZXJ0dW0veWNhc2hhMi5jcmwwLaAroCmGJ2h0dHA6Ly95YW5kZXguY3JsLmNl\n"
//    "cnR1bS5wbC95Y2FzaGEyLmNybDBxBggrBgEFBQcBAQRlMGMwLAYIKwYBBQUHMAGG\n"
//    "IGh0dHA6Ly95YW5kZXgub2NzcC1yZXNwb25kZXIuY29tMDMGCCsGAQUFBzAChido\n"
//    "dHRwOi8vcmVwb3NpdG9yeS5jZXJ0dW0ucGwveWNhc2hhMi5jZXIwHwYDVR0jBBgw\n"
//    "FoAUN1zjGeCyjqGoTtLPq9Dc4wtcNU0wHQYDVR0OBBYEFIhadSmxLCL8t9xzmoJl\n"
//    "VEhPlgmwMEwGA1UdIARFMEMwCAYGZ4EMAQICMDcGDCqEaAGG9ncCBQEKAjAnMCUG\n"
//    "CCsGAQUFBwIBFhlodHRwczovL3d3dy5jZXJ0dW0ucGwvQ1BTMB0GA1UdJQQWMBQG\n"
//    "CCsGAQUFBwMBBggrBgEFBQcDAjAOBgNVHQ8BAf8EBAMCB4AwggXdBgNVHREEggXU\n"
//    "MIIF0IIKeWFuZGV4LmNvbYIJeWFuZGV4LmJ5ggp5YW5kZXgubmV0ggl5YW5kZXgu\n"
//    "a3qCCXlhbmRleC51YYIJeWFuZGV4LnJ1gg15YW5kZXguY29tLnRyghp3d3cueG4t\n"
//    "LWQxYWNwangzZi54bi0tcDFhaYIYbS54bi0tZDFhY3BqeDNmLnhuLS1wMWFpghZ4\n"
//    "bi0tZDFhY3BqeDNmLnhuLS1wMWFpghN4bWxzZWFyY2gueWFuZGV4LmJ5ghBwZW9w\n"
//    "bGUueWFuZGV4LmJ5gg92aWRlby55YW5kZXguYnmCEGltYWdlcy55YW5kZXguYnmC\n"
//    "DXd3dy55YW5kZXguYnmCDXd3dy55YW5kZXguZnKCC20ueWFuZGV4LmZyggl5YW5k\n"
//    "ZXguZnKCC20ueWFuZGV4LmVlggl5YW5kZXguZWWCEHBlb3BsZS55YW5kZXgua3qC\n"
//    "C20ueWFuZGV4Lmt6ghN4bWxzZWFyY2gueWFuZGV4Lmt6gg13d3cueWFuZGV4Lmt6\n"
//    "gg92aWRlby55YW5kZXgua3qCFGZhbWlseS55YW5kZXguY29tLnRygg9tLnlhbmRl\n"
//    "eC5jb20udHKCFHBlb3BsZS55YW5kZXguY29tLnRyghJhaWxlLnlhbmRleC5jb20u\n"
//    "dHKCFGltYWdlcy55YW5kZXguY29tLnRygg13d3cueWFuZGV4LmtnghF3d3cueWFu\n"
//    "ZGV4LmNvbS5hbYILbS55YW5kZXgubWSCCXlhbmRleC5tZIILbS55YW5kZXgubHaC\n"
//    "EGltYWdlcy55YW5kZXgudWGCEmdhbWUueWFuZGV4LmNvbS50coITdmlkZW8ueWFu\n"
//    "ZGV4LmNvbS50coINd3d3LnlhbmRleC51YYINd3d3LnlhbmRleC51eoIPbS55YW5k\n"
//    "ZXguY29tLmdlgg13d3cueWFuZGV4Lm1kggttLnlhbmRleC5ydYILbS55YW5kZXgu\n"
//    "dG2CDm0ueWFuZGV4LmNvLmlsggx5YW5kZXguY28uaWyCDXd3dy55YW5kZXguZWWC\n"
//    "CXlhbmRleC5sdIILbS55YW5kZXgudXqCEXd3dy55YW5kZXguY29tLmdlgg13d3cu\n"
//    "eWFuZGV4LmF6ghB3d3cueWFuZGV4LmNvLmlsggttLnlhbmRleC5rZ4IRd3d3Lnlh\n"
//    "bmRleC5jb20udHKCEnBsYXkueWFuZGV4LmNvbS50coIPbS55YW5kZXguY29tLmFt\n"
//    "ghRnb3JzZWwueWFuZGV4LmNvbS50coIJeWFuZGV4Lmx2ghBwZW9wbGUueWFuZGV4\n"
//    "LnJ1ggttLnlhbmRleC51YYIQcGVvcGxlLnlhbmRleC51YYIJeWFuZGV4LmF6gg92\n"
//    "aWRlby55YW5kZXgucnWCDG0ueWFuZGV4LmNvbYIXeG1sc2VhcmNoLnlhbmRleC5j\n"
//    "b20udHKCDXd3dy55YW5kZXgubHaCEGltYWdlcy55YW5kZXgucnWCE3htbHNlYXJj\n"
//    "aC55YW5kZXgudWGCDXd3dy55YW5kZXgudG2CDnd3dy55YW5kZXguY29tgg13d3cu\n"
//    "eWFuZGV4Lmx0ggl5YW5kZXgudG2CDXd3dy55YW5kZXgucnWCFHhtbHNlYXJjaC55\n"
//    "YW5kZXguY29tgg15YW5kZXguY29tLmdlghNnYW1lcy55YW5kZXguY29tLnRyghN4\n"
//    "bWxzZWFyY2gueWFuZGV4LnJ1ggttLnlhbmRleC5ieYIJeWFuZGV4LnRqgg92aWRl\n"
//    "by55YW5kZXgudWGCDXd3dy55YW5kZXgudGqCC20ueWFuZGV4Lmx0ghBpbWFnZXMu\n"
//    "eWFuZGV4Lmt6ghFwZW9wbGUueWFuZGV4LmNvbYISb3l1bi55YW5kZXguY29tLnRy\n"
//    "ggttLnlhbmRleC5heoINeWFuZGV4LmNvbS5hbYIQdmlkZW8ueWFuZGV4LmNvbYIR\n"
//    "aW1hZ2VzLnlhbmRleC5jb22CCXlhbmRleC5rZ4IJeWFuZGV4LnV6ggttLnlhbmRl\n"
//    "eC50ajCCAX8GCisGAQQB1nkCBAIEggFvBIIBawFpAHcApLkJkLQYWBSHuxOizGdw\n"
//    "Cjw1mAT5G9+443fNDsgN3BAAAAFtARvLPgAABAMASDBGAiEA2eu2MHUwKhvlVzVq\n"
//    "nzCD7xnZRbxUDLl/aW3ESd9lb/wCIQDT+VwK5G3+0/8j8TXsWdoDbC1uyEWpoqrX\n"
//    "MiksHnxJXAB2AFWB1MIWkDYBSuoLm1c8U/DA5Dh4cCUIFy+jqh0HE9MMAAABbQEb\n"
//    "zNQAAAQDAEcwRQIhAOpKP3IEWNRzIvltBAT7xmd9EqpKr+vFMCSxMsLJ5dX4AiBz\n"
//    "A7PGv8+O3rfhxh4JmBXoJNNW3fDGAajOPRTqZNr8+AB2AF6nc/nfVsDntTZIfdBJ\n"
//    "4DJ6kZoMhKESEoQYdZaBcUVYAAABbQEby5QAAAQDAEcwRQIhAKnrUMyjFrtWPvTs\n"
//    "CCPHyosSxuNmLk0MVfEbLDqZ9YHBAiBdttwYKFDN1ZxtMxqo1tLhtCfp0dhRh9/X\n"
//    "PHZ8O+4xHzANBgkqhkiG9w0BAQsFAAOCAQEAK0w3Vs5KCWVERxFwuiMALR5n3r5e\n"
//    "kJA3OmwFjUINqa3hkpL8qjMKgLMQpdnkQSJoAWGLvXbOJ9ypJRv14qeN55lE55rI\n"
//    "T53Y5N1CPyqy1pUqXRMKZLwcXUkao1ZaRtg6wQh+8sOYg7/9psgNNXTdQOI5RPs7\n"
//    "1hyx+N7b3Gh+YAe82aC5WdLJU2LvGSSgPaqrcZS91xlEgVlNx0TI0RLuhPXunytN\n"
//    "DhtLXBAI1tZzs5mcAMTGqMAb/2c2EpXUWz00B3hi9fvwWIVHXkrjCsj4aEAa2Xp8\n"
//    "mFPRVsOLaEF3vdiSB1J9vnPTAG2mjsoRv1RpMqg8jL8enL1BLGW4cd13kw==\n"
//    "-----END CERTIFICATE-----\n"
//
//
//    ;
//
//  ctx.add_certificate_authority(
//    boost::asio::buffer(cert.data(), cert.size()), ec);
//  if (ec)
//    return;
//}
//
//
//
//// Load the root certificates into an ssl::context
////
//// This function is inline so that its easy to take
//// the address and there's nothing weird like a
//// gratuituous template argument; thus it appears
//// like a "normal" function.
////
//
//
//
//inline
//void
//load_root_certificates(ssl::context& ctx)
//{
//  boost::system::error_code ec;
//  load_root_certificates(ctx, ec);
//  if (ec)
//  {
//    fail(ec, "certificate");
//  }
//}
//
//HttpsClient::HttpsClient(const HttpsCfg& cfg, boost::asio::io_context& ioc)
//  :clientCfg_(cfg), ioc_(ioc), stream_{ ioc_, ctx_ }
//{
//  load_root_certificates(ctx_);
//
//  //boost::asio::spawn(ioc_,
//  //  std::bind(
//  //    &HttpsClient::Start, this,
//  //    std::placeholders::_1));
//}
//
//void HttpsClient::Disconnect(net::yield_context& yld)
//{
//  boost::system::error_code ec;
//
//  stream_.async_shutdown(yld[ec]);
//
//  if (ec && ec != boost::system::errc::not_connected)
//    return fail(ec, "shutdown");
//
//}
//
//bool HttpsClient::Connect(net::yield_context& yld)
//{
//  boost::system::error_code ec;
//
//  // Set SNI Hostname (many hosts need this to handshake successfully)
//  if (!SSL_set_tlsext_host_name(stream_.native_handle(), clientCfg_.url.c_str()))
//  {
//    ec.assign(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
//    lerr << ec.message() << "\n";
//    return false;
//  }
//
//  // These objects perform our I/O
//  tcp::resolver resolver{ ioc_ };
//
//  // Look up the domain name
//  auto const results = resolver.async_resolve(clientCfg_.url, clientCfg_.port, yld[ec]);
//  if (ec)
//  {
//    fail(ec, "resolve");
//    return false;
//  }
//  linfo << "Url: " << clientCfg_.url << "[" << clientCfg_.port << "] resolved to: " <<
//    results.begin()->endpoint() << std::endl;
//  //--------------------------
//
//
//  // Make the connection on the IP address we get from a lookup
//  while (!fStop)
//  {
//    // Make the connection on the IP address we get from a lookup
//    linfo << "Try to connect: " << clientCfg_.url << "[" << clientCfg_.port << "]" << std::endl;
//    boost::asio::async_connect(stream_.next_layer(), results.begin(), results.end(), yld[ec]);
//    if (ec)
//    {
//      fail(ec, "connect");
//
//      boost::asio::deadline_timer timer(ioc_);
//      timer.expires_from_now(boost::posix_time::milliseconds(500));
//      timer.async_wait(yld[ec]);
//
//      continue;
//    }
//    linfo << "Try handshake: " << clientCfg_.url << "[" << clientCfg_.port << "]" << std::endl;
//    // Perform the SSL handshake
//    stream_.async_handshake(ssl::stream_base::client, yld[ec]);
//    if (ec)
//    {
//      fail(ec, "handshake");
//
//      stream_.async_shutdown(yld[ec]);
//
//      boost::asio::deadline_timer timer(ioc_);
//      timer.expires_from_now(boost::posix_time::milliseconds(500));
//      timer.async_wait(yld[ec]);
//
//      continue;
//    }
//
//    break;
//  }
//
//  linfo << "Connected: " << clientCfg_.url << "[" << clientCfg_.port << "]" << std::endl;
//  return true;
//}
//#include <thread>
//http::response<http::dynamic_body> HttpsClient::send_recv_async(net::yield_context& parentYield, http::request<http::string_body> req)
//{
//  auto event = std::make_shared<ReqAsync>(ioc_);
//  event->req = std::move(req);
//
//  data_.push_back(event);
//
//  if (timer_) timer_->cancel();
//
//  //event->wakeupTimer.expires_at(boost::posix_time::pos_infin);
//  event->wakeupTimer.expires_from_now(boost::posix_time::seconds(10));
//
//  boost::system::error_code ec;
//  event->wakeupTimer.async_wait(parentYield[ec]); // boost::asio::error::operation_aborted
//
//  return std::move(event->res);
//}
//
//void HttpsClient::Stop()
//{
//  fStop = true;
//  if (timer_) timer_->cancel();
//}
//
//bool HttpsClient::Start(net::yield_context yld)
//{
//
//  if (!Connect(yld)) return false;
//
//  timer_ = std::make_unique<boost::asio::deadline_timer>(ioc_);
//  boost::system::error_code ec;
//
//  timer_->expires_from_now(boost::posix_time::seconds(0));
//  while (!fStop)
//  {
//    timer_->async_wait(yld[ec]);
//    if (data_.empty())
//    {
//      timer_->expires_at(boost::posix_time::pos_infin);
//      continue;
//    }
//
//    auto data = std::move(data_.front());
//    data_.pop_front();
//
//    //tcp_stream stream(std::move(s));
//    //stream.expires_after(std::chrono::seconds(30));
//
//    linfo << "Send: " << data->req << std::endl;
//    http::async_write(stream_, data->req, yld[ec]);
//    if (OnSendRecvError(ec, yld))
//    {
//      data->wakeupTimer.cancel();
//      continue;
//    }
//
//    // This buffer is used for reading and must be persisted
//    boost::beast::flat_buffer b;
//
//    // Declare a container to hold the response
//    http::response<http::dynamic_body> res;
//
//    // Receive the HTTP response
//    http::async_read(stream_, b, res, yld[ec]);
//    if (OnSendRecvError(ec, yld))
//    {
//      data->wakeupTimer.cancel();
//      continue;
//    }
//    linfo << "Recive: " << res << std::endl;
//
//    data->res = std::move(res);
//    data->wakeupTimer.cancel();
//  }
//
//
//  Disconnect(yld);
//
//  return true;
//}
//
//bool HttpsClient::OnSendRecvError(const boost::system::error_code& err, net::yield_context& yld)
//{
//
//  using namespace boost;
//
//  switch (err.value()) {
//  case 0: return false;
//  case asio::error::interrupted:
//  case asio::error::in_progress:
//  {
//    return false;
//  }
//
//  case asio::error::connection_aborted:
//  case asio::error::connection_reset:
//  case asio::error::network_down:
//  case asio::error::network_reset:
//  case asio::error::not_connected:
//  case asio::error::shut_down:
//  case asio::error::broken_pipe:
//  case asio::error::timed_out:
//  case asio::error::eof:
//  {
//    lerr << "Connection broken " << err << std::endl;
//    Disconnect(yld);
//    Connect(yld);
//    return true;
//  }
//
//  default:
//  {
//    lerr << "Unhandled error on send/recv " << err << std::endl;
//    Disconnect(yld);
//    Connect(yld);
//    return true;
//  }
//  //boost::beast::http::error
//  }
//}
//

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

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
    };
    boost::asio::awaitable<bool> Connect(SslStream& stream, const HttpsCfg& cfg)
    {
      boost::system::error_code ec;

      if (!SSL_set_tlsext_host_name(stream.native_handle(), cfg.url.c_str()))
      {
        ec.assign(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
        LOG_ERROR(ec.message() << "\n");
        co_return false;
      }

      // These objects perform our I/O
      tcp::resolver resolver{ ioc_ };

      // Look up the domain name
      auto const results = co_await resolver.async_resolve(clientCfg_.url, clientCfg_.port, boost::asio::use_awaitable);


      co_return false;
    }
  };

  class ManagerConn
  {
  public:
    struct Dependencies
    {
      Dependencies(boost::asio::io_context& ioc)
        :ioc_(ioc)
      {}

      boost::asio::io_context& ioc_;
    };

    struct Config
    {
      uint8_t numConn = 1;
    };
    ManagerConn(const Config& cfg, const Dependencies& dep);

    void Init()
    {

    }
  };
}