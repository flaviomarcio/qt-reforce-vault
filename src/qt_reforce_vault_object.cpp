#include "./qt_reforce_vault_object.h"

namespace QtVault{


class ObjectPvt:public QObject
{
public:
    Object *parent;
    explicit ObjectPvt(Object *parent):QObject{parent},parent{parent}{
    }

};

Object::Object(QObject *parent):QObject{parent},p{new ObjectPvt{this}}
{
}

}

