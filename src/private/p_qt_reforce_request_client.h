#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QVariantHash>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>

#ifndef QTREFORCE_QREQUEST_CLIENT
namespace QtReforce {
class RequestClientPvt;

//!
//! \brief The RequestClient class
//!
class RequestClient: public QObject
{
    Q_OBJECT
public:
    struct Response
    {
        friend class RequestClientPvt;
        friend class RequestClient;
    private:
        explicit Response(RequestClientPvt *p):p{p}{}
    public:

        //!
        //! \brief toMap
        //! \return
        //!
        const QVariantHash toMap()const;

        //!
        //! \brief isOK
        //! \return
        //!
        bool isOK()const;

        //!
        //! \brief isRunning
        //! \return
        //!
        bool isRunning()const;

        //!
        //! \brief headers
        //! \return
        //!
        const QVariantHash &headers()const;

        //!
        //! \brief statusCode
        //! \return
        //!
        int statusCode()const;

        //!
        //! \brief reasonPhrase
        //! \return
        //!
        QString reasonPhrase() const;

        //!
        //! \brief bodyAsBytes
        //! \return
        //!
        const QByteArray &body()const;

        //!
        //! \brief bodyAsMap
        //! \return
        //!
        const QVariantHash bodyAsMap()const;

        //!
        //! \brief bodyAsList
        //! \return
        //!
        const QVariantList bodyAsList()const;

        //!
        //! \brief bodyAsStringList
        //! \return
        //!
        const QStringList bodyAsStringList()const;

        //!
        //! \brief bodyAs
        //! \param parent
        //! \return
        //!
        template <class T>
        T *bodyAs(QObject *parent)const
        {
            return new T(this->body(), parent);
        }
    private:
        RequestClientPvt *p=nullptr;
    };

    typedef std::function<void()> VoidMethod;
    typedef std::function<void(RequestClient *request)> RequestMethod;
    typedef std::function<void(Response response)> ResponseMethod;

    class HttpEncoding {
    public:
        static const QByteArray CONTENT_LENGTH()            { static const auto value=QByteArray("Content-Length"); return value;}
        static const QByteArray CONTENT_TYPE()              { static const auto value=QByteArray("Content-Type"); return value;}
        static const QByteArray ACCEPT_ENCODING_HEADER()    { static const auto value=QByteArray("Accept-Encoding"); return value;}
        static const QByteArray CONTENT_ENCODING_HEADER()   { static const auto value=QByteArray("Content-Encoding"); return value;}
        static const QByteArray GZIP_ENCODING()             { static const auto value=QByteArray("gzip"); return value;}
        static const QByteArray DEFLATE_ENCODING()          { static const auto value=QByteArray("deflate"); return value;}
    };

    class MediaType{
    public:
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

    enum Method{
        Head, Get, Post, Put, Delete, List, Upload, Download
    };
    Q_ENUM(Method)

    //!
    //! \brief RequestUtil
    //! \param parent
    //!
    Q_INVOKABLE explicit RequestClient(QObject *parent=nullptr);

    //!
    //! \brief builder
    //! \return
    //!
    static RequestClient &builder(QObject *parent=nullptr);

    //!
    //! \brief onStarted
    //! \param method
    //!
    const RequestClient &onStarted(RequestClient::VoidMethod method)const;

    //!
    //! \brief onSent
    //! \param method
    //!
    const RequestClient &onSent(RequestClient::VoidMethod method)const;

    //!
    //! \brief onSuccessful
    //! \param method
    //!
    const RequestClient &onSuccessful(RequestClient::ResponseMethod method)const;

    //!
    //! \brief onFail
    //! \param method
    //!
    const RequestClient &onFail(RequestClient::ResponseMethod method)const;

    //!
    //! \brief onFinished
    //! \param method
    //!
    const RequestClient &onFinished(RequestMethod method)const;

    //!
    //! \brief call
    //! \return
    //!
    const RequestClient &call()const;

    //!
    //! \brief abort
    //! \return
    //!
    const RequestClient &abort();

    //!
    //! \brief isOK
    //! \return
    //!
    bool isOK()const;

    //!
    //! \brief response
    //! \return
    //!
    const Response response()const;

    //!
    //! \brief HEAD
    //! \return
    //!
    const RequestClient &HEAD()const;

    //!
    //! \brief GET
    //! \return
    //!
    const RequestClient &GET()const;

    //!
    //! \brief POST
    //! \return
    //!
    const RequestClient &POST()const;

    //!
    //! \brief PUT
    //! \return
    //!
    const RequestClient &PUT()const;

    //!
    //! \brief DELETE
    //! \return
    //!
    const RequestClient &DELETE()const;

    //!
    //! \brief LIST
    //! \return
    //!
    const RequestClient &LIST()const;

    //!
    //! \brief print
    //! \return
    //!
    const RequestClient &print()const;

    //!
    //! \brief printOnFail
    //! \return
    //!
    const RequestClient &printOnFail() const;
    const RequestClient &printOnFail(bool newValue) const;

    //!
    //! \brief aSync
    //! \return
    //!
    bool aSync();
    const RequestClient &aSync(bool newASync)const;

    //!
    //! \brief method
    //! \return
    //!
    Method method();
    const RequestClient &method(Method newMethod)const;

    //!
    //! \brief headers
    //! \return
    //!
    const QVariantHash &headers()const;
    const RequestClient &headers(const QVariantHash &newHeaders)const;
    const RequestClient &headers(const QString&key, const QString &value)const;

    //!
    //! \brief headerApplicationJson
    //! \return
    //!
    const RequestClient &headerApplicationJson()const;

    //!
    //! \brief headerFormUrlencoded
    //! \return
    //!
    const RequestClient &headerFormUrlencoded()const;

    //!
    //! \brief headerApplicationOctecStream
    //! \return
    //!
    const RequestClient &headerApplicationOctecStream()const;

    //!
    //! \brief headerApplicationCBor
    //! \return
    //!
    const RequestClient &headerApplicationCBor()const;

    //!
    //! \brief headerTextPlain
    //! \return
    //!
    const RequestClient &headerTextPlain()const;

    //!
    //! \brief headerApplicationXml
    //! \return
    //!
    const RequestClient &headerApplicationXml()const;

    //!
    //! \brief headerApplicationPdf
    //! \return
    //!
    const RequestClient &headerApplicationPdf()const;

    //!
    //! \brief url
    //! \return
    //!
    const QUrl &url()const;
    const RequestClient &url(const QUrl &newUrl)const;

    //!
    //! \brief args
    //! \return
    //!
    const QVariantHash &args()const;
    const RequestClient &args(const QString &key, const QVariant &value)const;
    const RequestClient &args(const QPair<QVariant, QVariant> &newArg)const;
    const RequestClient &args(const QVariantHash &newArgs)const;
    const RequestClient &args(const QVariantMap &newArgs)const;

    //!
    //! \brief body
    //! \return
    //!
    const QByteArray &body()const;
    const RequestClient &body(const QVariant &newBody)const;

private:
    //!
    //! \brief p
    //!
    RequestClientPvt *p=nullptr;
};
}

#endif
