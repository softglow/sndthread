#ifndef SNDIO_H
#define SNDIO_H

#include <QIODevice>
#include <QMutex>
#include <QAudioFormat>
#include "sndbuf.h"
#include "sndthread_global.h"

class SNDTHREADSHARED_EXPORT SndIO : public QIODevice
{
    Q_OBJECT
private:
    QAudioFormat fmt;
    QMutex buf_lock;
    SndBuf bufs[2]; // swappable by updateBuffers
    int gen_buf;
    int out_buf;
    bool out_empty;
    bool gen_full;

    void updateBuffers (); // MUST call with buf_lock held

protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);
    virtual qint64 readSilence(char *data, qint64 samples);

public:
    SndIO(const QAudioFormat &in_fmt, QObject *parent = 0);
    ~SndIO();
    virtual void start (); // initiate first fill

signals:
    // bytes parameter on all signals is free space in the object's buffers
    void readyWrite (SndBuf * scratch); // after buffer swap in updateBuffers()
    void emptied ();  // out_empty at end of readData()
    void underrun (); // no data in out_buf during readData()

public slots:
    void writeComplete ();
};

#endif // SNDIO_H
