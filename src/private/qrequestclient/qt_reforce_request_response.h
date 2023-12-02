#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QVariantHash>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include <QFile>
#include "./qt_reforce_request_client_global.h"

namespace QtReforce {
class QRequestClientPvt;

//!
//! \brief The QRequestResponse class
//!
struct Q_REFORCE_REQUEST_CLIENT_EXPORT QRequestResponse
{
    friend class QRequestClientPvt;
    friend class QRequestClient;
private:
    explicit QRequestResponse(QRequestClientPvt *p=nullptr):p{p}{}
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
    //! \brief bodyAsVariant
    //! \return
    //!
    const QVariant bodyAsVariant()const;

    //!
    //! \brief bodyAsFile
    //! \return
    //!
    QFile bodyAsFile();

    //!
    //! \brief bodyAs
    //! \param parent
    //! \return
    //!
    template <class T>
    T *bodyAs(QObject *parent=nullptr)
    {
        return new T(this->body(), parent);
    }
private:
    QRequestClientPvt *p=nullptr;
};

}

