////////////////////////////////////////////////////////////////////////////////
/// @brief V8 client connection
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2013 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Dr. Frank Celler
/// @author Achim Brandt
/// @author Copyright 2008-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_V8CLIENT_V8CLIENT_CONNECTION_H
#define TRIAGENS_V8CLIENT_V8CLIENT_CONNECTION_H 1

#include "Basics/Common.h"
#include "Rest/HttpRequest.h"

#include <v8.h>

// -----------------------------------------------------------------------------
// --SECTION--                                              forward declarations
// -----------------------------------------------------------------------------

namespace triagens {
  namespace httpclient {
    class GeneralClientConnection;
    class SimpleHttpClient;
    class SimpleHttpResult;
  }

  namespace rest {
    class Endpoint;
  }
}

// -----------------------------------------------------------------------------
// --SECTION--                                          class V8ClientConnection
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

namespace triagens {
  namespace v8client {

////////////////////////////////////////////////////////////////////////////////
/// @brief class for http requests
////////////////////////////////////////////////////////////////////////////////

    class V8ClientConnection {
      private:
        V8ClientConnection (V8ClientConnection const&);
        V8ClientConnection& operator= (V8ClientConnection const&);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                      constructors and destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor
////////////////////////////////////////////////////////////////////////////////

        V8ClientConnection (triagens::rest::Endpoint*,
                            string,
                            const string&,
                            const string&,
                            double,
                            double,
                            size_t,
                            uint32_t,
                            bool);

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor
////////////////////////////////////////////////////////////////////////////////

        ~V8ClientConnection ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup ArangoDB
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief request location rewriter (injects database name)
////////////////////////////////////////////////////////////////////////////////

        static string rewriteLocation (void*, const string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief returns true if it is connected
////////////////////////////////////////////////////////////////////////////////

        bool isConnected ();

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the current database name
////////////////////////////////////////////////////////////////////////////////

        const string& getDatabaseName ();

////////////////////////////////////////////////////////////////////////////////
/// @brief set the current database name
////////////////////////////////////////////////////////////////////////////////

        void setDatabaseName (const string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief returns the version and build number of the arango server
////////////////////////////////////////////////////////////////////////////////

        const string& getVersion ();

////////////////////////////////////////////////////////////////////////////////
/// @brief get the last http return code
///
/// @return int          the code
////////////////////////////////////////////////////////////////////////////////

        int getLastHttpReturnCode ();

////////////////////////////////////////////////////////////////////////////////
/// @brief get the last error message
///
/// @return string          the error message
////////////////////////////////////////////////////////////////////////////////

        const std::string& getErrorMessage ();

////////////////////////////////////////////////////////////////////////////////
/// @brief get the endpoint string
///
/// @return string
////////////////////////////////////////////////////////////////////////////////

        const std::string getEndpointSpecification ();

////////////////////////////////////////////////////////////////////////////////
/// @brief get the simple http client
///
/// @return triagens::httpclient::SimpleHttpClient*    then client connection
////////////////////////////////////////////////////////////////////////////////

        triagens::httpclient::SimpleHttpClient* getHttpClient();

////////////////////////////////////////////////////////////////////////////////
/// @brief do a "GET" request
///
/// @param string location                     the request location
/// @param map<string, string> headerFields    additional header fields
/// @param raw                                 return the raw response
///
/// @return v8::Value                          a V8 JavaScript object
////////////////////////////////////////////////////////////////////////////////

        v8::Handle<v8::Value> getData (std::string const& location,
                                       map<string, string> const& headerFields,
                                       bool raw);

////////////////////////////////////////////////////////////////////////////////
/// @brief do a "DELETE" request
///
/// @param string location                     the request location
/// @param map<string, string> headerFields    additional header fields
/// @param raw                                 return the raw response
///
/// @return v8::Value                          a V8 JavaScript object
////////////////////////////////////////////////////////////////////////////////

        v8::Handle<v8::Value> deleteData (std::string const& location,
                                          map<string, string> const& headerFields,
                                          bool raw);

////////////////////////////////////////////////////////////////////////////////
/// @brief do a "HEAD" request
///
/// @param string location                     the request location
/// @param map<string, string> headerFields    additional header fields
/// @param raw                                 return the raw response
///
/// @return v8::Value                          a V8 JavaScript object
////////////////////////////////////////////////////////////////////////////////

        v8::Handle<v8::Value> headData (std::string const& location,
                                        map<string, string> const& headerFields,
                                        bool raw);

////////////////////////////////////////////////////////////////////////////////
/// @brief do an "OPTIONS" request
///
/// @param string location                     the request location
/// @param string body                         the request body
/// @param map<string, string> headerFields    additional header fields
/// @param raw                                 return the raw response
///
/// @return v8::Value                          a V8 JavaScript object
////////////////////////////////////////////////////////////////////////////////

        v8::Handle<v8::Value> optionsData (std::string const& location,
                                           std::string const& body,
                                           map<string, string> const& headerFields,
                                           bool raw);

////////////////////////////////////////////////////////////////////////////////
/// @brief do a "POST" request
///
/// @param string location                     the request location
/// @param string body                         the request body
/// @param map<string, string> headerFields    additional header fields
/// @param raw                                 return the raw response
///
/// @return v8::Value                          a V8 JavaScript object
////////////////////////////////////////////////////////////////////////////////

        v8::Handle<v8::Value> postData (std::string const& location,
                                        std::string const& body,
                                        map<string, string> const& headerFields,
                                        bool raw);

        v8::Handle<v8::Value> postData (std::string const& location,
                                        const char* body,
                                        const size_t bodySize,
                                        map<string, string> const& headerFields);

////////////////////////////////////////////////////////////////////////////////
/// @brief do a "PUT" request
///
/// @param string location                     the request location
/// @param string body                         the request body
/// @param map<string, string> headerFields    additional header fields
/// @param raw                                 return the raw response
///
/// @return v8::Value                          a V8 JavaScript object
////////////////////////////////////////////////////////////////////////////////

        v8::Handle<v8::Value> putData (std::string const& location,
                                       std::string const& body,
                                       map<string, string> const& headerFields,
                                       bool raw);

////////////////////////////////////////////////////////////////////////////////
/// @brief do a "PATCH" request
///
/// @param string location                     the request location
/// @param string body                         the request body
/// @param map<string, string> headerFields    additional header fields
/// @param raw                                 return the raw response
///
/// @return v8::Value                          a V8 JavaScript object
////////////////////////////////////////////////////////////////////////////////

        v8::Handle<v8::Value> patchData (std::string const& location,
                                         std::string const& body,
                                         map<string, string> const& headerFields,
                                         bool raw);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private functions
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup V8ClientConnection
/// @{
////////////////////////////////////////////////////////////////////////////////

    private:

////////////////////////////////////////////////////////////////////////////////
/// @brief executes a request
////////////////////////////////////////////////////////////////////////////////

      v8::Handle<v8::Value> requestData (rest::HttpRequest::HttpRequestType method,
                                         std::string const& location,
                                         const char* body,
                                         const size_t bodySize,
                                         map<string, string> const& headerFields);

////////////////////////////////////////////////////////////////////////////////
/// @brief executes a request
////////////////////////////////////////////////////////////////////////////////

      v8::Handle<v8::Value> requestData (rest::HttpRequest::HttpRequestType method,
                                         std::string const& location,
                                         std::string const& body,
                                         map<string, string> const& headerFields);

////////////////////////////////////////////////////////////////////////////////
/// @brief handles a result
////////////////////////////////////////////////////////////////////////////////
      
      v8::Handle<v8::Value> handleResult ();

////////////////////////////////////////////////////////////////////////////////
/// @brief executes a request and returns raw response
////////////////////////////////////////////////////////////////////////////////

      v8::Handle<v8::Value> requestDataRaw (rest::HttpRequest::HttpRequestType method,
                                            std::string const& location,
                                            std::string const& body,
                                            map<string, string> const& headerFields);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup V8ClientConnection
/// @{
////////////////////////////////////////////////////////////////////////////////

    private:

////////////////////////////////////////////////////////////////////////////////
/// @brief connection
////////////////////////////////////////////////////////////////////////////////

      triagens::httpclient::GeneralClientConnection* _connection;

////////////////////////////////////////////////////////////////////////////////
/// @brief database name
////////////////////////////////////////////////////////////////////////////////

      std::string _databaseName;

////////////////////////////////////////////////////////////////////////////////
/// @brief server version
////////////////////////////////////////////////////////////////////////////////

      std::string _version;

////////////////////////////////////////////////////////////////////////////////
/// @brief last http return code
////////////////////////////////////////////////////////////////////////////////

      int _lastHttpReturnCode;

////////////////////////////////////////////////////////////////////////////////
/// @brief last error message
////////////////////////////////////////////////////////////////////////////////

      std::string _lastErrorMessage;

////////////////////////////////////////////////////////////////////////////////
/// @brief underlying client
////////////////////////////////////////////////////////////////////////////////

      triagens::httpclient::SimpleHttpClient* _client;

////////////////////////////////////////////////////////////////////////////////
/// @brief last result
////////////////////////////////////////////////////////////////////////////////

      triagens::httpclient::SimpleHttpResult* _httpResult;
    };
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                       END-OF-FILE
// -----------------------------------------------------------------------------

// Local Variables:
// mode: outline-minor
// outline-regexp: "/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|/// @page\\|// --SECTION--\\|/// @\\}"
// End:
