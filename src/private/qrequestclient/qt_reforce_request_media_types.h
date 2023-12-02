#pragma once

#include <QObject>
#include "./qt_reforce_request_client_global.h"

namespace QtReforce {

//!
//! \brief The QMediaType class
//!
class Q_REFORCE_REQUEST_CLIENT_EXPORT QMediaType:public QObject{
    Q_OBJECT
public:
    explicit QMediaType(QObject *parent=nullptr):QObject{parent}{}

    static const QByteArray ALL()                           { static const auto value=QByteArray("*/*");                                return value; }
    static const QByteArray APPLICATION_ATOM_XML()          { static const auto value=QByteArray("application/atom+xml");               return value; }
    static const QByteArray APPLICATION_CBOR()              { static const auto value=QByteArray("application/cbor");                   return value; }
    static const QByteArray APPLICATION_FORM_URLENCODED()   { static const auto value=QByteArray("application/x-www-form-urlencoded");  return value; }
    static const QByteArray APPLICATION_GRAPHQL()           { static const auto value=QByteArray("application/graphql+json");           return value; }
    static const QByteArray APPLICATION_JSON()              { static const auto value=QByteArray("application/json");                   return value; }
    static const QByteArray APPLICATION_NDJSON()            { static const auto value=QByteArray("application/x-ndjson");               return value; }
    static const QByteArray APPLICATION_OCTET_STREAM()      { static const auto value=QByteArray("application/octet-stream");           return value; }
    static const QByteArray APPLICATION_PDF()               { static const auto value=QByteArray("application/pdf");                    return value; }
    static const QByteArray APPLICATION_PROBLEM_JSON()      { static const auto value=QByteArray("application/problem+json");           return value; }
    static const QByteArray APPLICATION_PROBLEM_XML()       { static const auto value=QByteArray("application/problem+xml");            return value; }
    static const QByteArray APPLICATION_RSS_XML()           { static const auto value=QByteArray("application/rss+xml");                return value; }
    static const QByteArray APPLICATION_STREAM_JSON()       { static const auto value=QByteArray("application/stream+json");            return value; }
    static const QByteArray APPLICATION_XHTML_XML()         { static const auto value=QByteArray("application/xhtml+xml");              return value; }
    static const QByteArray APPLICATION_XML()               { static const auto value=QByteArray("application/xml");                    return value; }
    static const QByteArray IMAGE_GIF()                     { static const auto value=QByteArray("image/gif");                          return value; }
    static const QByteArray IMAGE_JPEG()                    { static const auto value=QByteArray("image/jpeg");                         return value; }
    static const QByteArray IMAGE_PNG()                     { static const auto value=QByteArray("image/png");                          return value; }
    static const QByteArray MULTIPART_FORM_DATA()           { static const auto value=QByteArray("multipart/form-data");                return value; }
    static const QByteArray MULTIPART_MIXED()               { static const auto value=QByteArray("multipart/mixed");                    return value; }
    static const QByteArray MULTIPART_RELATED()             { static const auto value=QByteArray("multipart/related");                  return value; }
    static const QByteArray TEXT_EVENT_STREAM()             { static const auto value=QByteArray("text/event-stream");                  return value; }
    static const QByteArray TEXT_HTML()                     { static const auto value=QByteArray("text/html");                          return value; }
    static const QByteArray TEXT_MARKDOWN()                 { static const auto value=QByteArray("text/markdown");                      return value; }
    static const QByteArray TEXT_PLAIN()                    { static const auto value=QByteArray("text/plain");                         return value; }
    static const QByteArray TEXT_XML()                      { static const auto value=QByteArray("text/xml");                           return value; }
};

}

