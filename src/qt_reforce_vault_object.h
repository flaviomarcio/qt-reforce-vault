#pragma once

#include <QObject>
#include "./qt_reforce_vault_global.h"


namespace QtVault {
class ObjectPvt;

//!
//! \brief The Object class
//!
class Q_REFORCE_VAULT_EXPORT Object: public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief Object
    //! \param parent
    //!
    Q_INVOKABLE explicit Object(QObject *parent=nullptr);
private:
    //!
    //! \brief p
    //!
    ObjectPvt *p=nullptr;
};

}

