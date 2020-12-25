
if(WIN32)
  set(OpenSSL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/openssl/include")
  set(OpenSSL_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/openssl/lib/libeay32.lib")
  set(OpenSSL_SSL_LIBRARY "${CMAKE_SOURCE_DIR}/third_party/openssl/lib/ssleay32.lib")
else()
  set(OpenSSL_INCLUDE_DIR "/usr/include")
  set(OpenSSL_LIBRARY "/usr/lib64/libcrypto.so")
  set(OpenSSL_SSL_LIBRARY "/usr/lib64/libssl.so")
endif()

set(OpenSSL_FOUND TRUE)
mark_as_advanced(OpenSSL_INCLUDE_DIR OpenSSL_LIBRARY)

if(OpenSSL_FOUND AND NOT TARGET OpenSSL::Crypto)
	message(STATUS " TARGET OpenSSL::Crypto " ${OpenSSL_INCLUDE_DIR})
  add_library(OpenSSL::Crypto UNKNOWN IMPORTED)
  set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OpenSSL_INCLUDE_DIR})
  set_property(TARGET OpenSSL::Crypto PROPERTY IMPORTED_LOCATION ${OpenSSL_LIBRARY})
endif()

if(OpenSSL_FOUND AND NOT TARGET OpenSSL::SSL)
	message(STATUS " TARGET OpenSSL::SSL " )
  add_library(OpenSSL::SSL UNKNOWN IMPORTED)
  set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OpenSSL_INCLUDE_DIR})
  set_property(TARGET OpenSSL::SSL PROPERTY IMPORTED_LOCATION ${OpenSSL_SSL_LIBRARY})
endif()
