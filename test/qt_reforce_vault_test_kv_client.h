#pragma once

#include <QThread>
#include "./qt_reforce_vault_test.h"
#include "../src/qt_reforce_vault_kv_client.h"

namespace QtReforce {

class UT_KvClient : public QtReforce::ObjectTest
{
    Q_OBJECT
    Q_VAULT_OBJECT_TEST(UT_KvClient)
public:
    const QVariantHash settings={
                                   {"url","http://localhost:1234"},
                                   {"token","00000000-0000-0000-0000-000000000000"},
                                   {"secretsPath","vault://secret/secret"},
                                   {"secretsName","YourAppName"},
                                   };

    const QVariantHash data=QVariantHash{
                                           {"keyA","valueA"},
                                           {"keyB","valueB"},
                                           {"keyC","valueC"},
                                           };
    Q_INVOKABLE void test_loaded()
    {
        QtVault::KvClient kvClient;

        kvClient
            .setting(settings)
            .pull();

        while(kvClient.isLoading()){
            QThread::msleep(1);
            qApp->processEvents();
        }
        auto values=kvClient.get();

        QCOMPARE_EQ(values.size(),2);
        QVERIFY(values.contains("a"));
        QVERIFY(values.contains("c"));
        QCOMPARE_EQ(values.value("a"),"bbbbb");
        QCOMPARE_EQ(values.value("c"),"dddddd");
    }

    void connectionExemple()
    {
        QtVault::KvClient kvClient(settings);
        kvClient
            //reset values of QtVault::KvClient
            .clear()
            //set a new settings
            .setting(settings)

            //replace all values for key value
            .values("keyA","valueA")

            //clean existing values
            .clean()
            //clean existing values and replace from values of vault server
            .pull();
    }

    void gettingValues()
    {
        QtVault::KvClient kvClient(settings);
        kvClient
            //clean existing values and replace from values of vault server
            .pull();
        qDebug()
            //getting all existing values
            <<kvClient.get()
            //getting value using string types: QString, QChar, ....
            <<kvClient.get("keyA")
            //getting map values using list types: QVariantList and QStringList
            <<kvClient.get(QVariantList{"keyA","keyB"})
            <<kvClient.get(QStringList{"keyA","keyB"})
            //getting map values using map types: QVariantHash, QVariantMap and QVariantPair
            <<kvClient.get(data);
    }


    void pushingValues(){
        //changin values
        QtVault::KvClient kvClient(settings);
        kvClient
            //clean existing values and replace from values of vault server
            .pull()

            //replace all values for key value
            .values("keyA","valueA")

            //adding a new values for key value
            .put("keyD","valueD")
            .put("keyE","valueE")
            .put("keyF","valueF")

            //remove values using key name
            .rm("keyA")//remove by name

            //remove values using key from list types: QVariantList and QStringList
            .rm(QVariantList{"keyA","keyA","keyC"})//remove by list
            .rm(QStringList{"keyA","keyA","keyC"})//remove by list

            //remove values using key from map types: QVariantHash, QVariantMap and QVariantPair
            .rm(QVariantHash{{"keyA",{}}})//remove by map
            .rm(QVariantMap{{"keyB",{}}})//remove by map

            //envia newValues para criar uma nova revisão no servidor do vault
            .push();
    }

};

}

