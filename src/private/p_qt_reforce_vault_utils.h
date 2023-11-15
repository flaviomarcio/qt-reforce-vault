#pragma once

#include <QMetaEnum>
#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QVariantHash>
#include <QVariantList>
#include <QStringList>
#include <QJsonDocument>
#include <QFile>

//!
//! \brief The RequestUtil class
//!

class Utils
{
public:

    static int toEnumType(const QMetaEnum &e, const QVariant &newValues){
        QByteArray keyName;
        switch (newValues.typeId()) {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Long:
        case QMetaType::ULong:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Double:
        {
            auto v=newValues.toInt();
            keyName=QByteArray{e.key(v)}.toLower();
            break;
        }
        case QMetaType::QString:
        case QMetaType::QByteArray:
        case QMetaType::QChar:
        {
            keyName=newValues.toByteArray().trimmed().toLower();
            break;
        }
        default:
            break;
        }

        for (int i = 0; i < e.keyCount(); ++i) {
            auto key=QByteArray{e.key(i)};
            if(keyName==key.toLower()){
                return e.keyToValue(key);
                break;
            }
        }

        return -1;
    }

    static QVariantHash toHash(const QVariant &data)
    {
        switch (data.typeId()) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
            return data.toHash();
        case QMetaType::QString:
        case QMetaType::QByteArray:
        {
            auto bytes=data.toByteArray().trimmed();
            if(bytes.isEmpty())
                return {};
            if(QFile::exists(bytes)){
                QFile file(bytes);
                if(!file.open(QFile::ReadOnly)){
                    qWarning()<<file.errorString();
                    return {};
                }
                bytes=file.readAll().trimmed();
                file.close();
            }
            if(bytes.isEmpty())
                return {};

            auto vHash=QJsonDocument::fromJson(bytes).toVariant().toHash();
            if(vHash.isEmpty()){
                auto lines=bytes.split('\n');
                for(auto&line:lines){
                    line=line.trimmed();
                    if(line.isEmpty())
                        continue;

                    if(!line.contains("=")){
                        vHash.insert(line,{});
                        continue;
                    }
                    auto keyValue=line.split('=');
                    if(keyValue.last().trimmed().isEmpty()){
                        vHash.insert(line,{});
                        continue;
                    }
                    vHash.insert(keyValue.takeFirst(),keyValue.join('='));
                }
            }
            return vHash;
        }
        default:
            break;
        }

        return {};

    }

};


