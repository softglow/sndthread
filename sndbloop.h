#ifndef SNDBLOOP_H
#define SNDBLOOP_H

#include "sndthread.h"

// Implements a simple triangle wave generator to make sure sound output works.
class SNDTHREADSHARED_EXPORT SndBloop : public SndThread
{
    Q_OBJECT
private:
    int limit;

protected:
    int frequency;
    float delta;
    float prev;

    void basicInit ();
    void recalculate ();
    virtual void getNext(snd_sample_t *frame);
    virtual void generate(SndBuf *buf) Q_DECL_OVERRIDE;

public:
    explicit SndBloop(QObject *parent = 0);
    SndBloop(int freq, QObject *parent = 0);

    virtual int getFrequency ();
    virtual int setFrequency (int freq); // returns new value of tone frequency

    virtual int setSampleRate (int rate) Q_DECL_OVERRIDE;

signals:

public slots:

};

#endif // SNDBLOOP_H
