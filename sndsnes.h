#ifndef SNDSNES_H
#define SNDSNES_H

#include "sndthread.h"

class SNDTHREADSHARED_EXPORT SndSnes : public SndThread
{
    //Q_OBJECT // uncomment to cause `undefined reference to vtable` link error
protected:
    virtual void generate() Q_DECL_OVERRIDE;
public:
    SndSnes(QObject *parent = 0);
    ~SndSnes();
};

#endif // SNDSNES_H
