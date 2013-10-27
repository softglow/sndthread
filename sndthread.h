#ifndef SNDTHREAD_H
#define SNDTHREAD_H

#include "sndthread_global.h"
#include "sndbuf.h"
#include <QObject>

// Runs off-main-thread, in a QThread.exec() event loop.
// Relies on the event loop to block it when there's nothing to do.
// Synchronizes with the main thread via shared buffer (SndBuf).
class SNDTHREADSHARED_EXPORT SndThread : public QObject
{
    Q_OBJECT

protected:
    // MUST have values set in derived classes' ctors
    int sample_rate;
    short channels;

    // Producer API (whole protected function list)
    virtual void generate (SndBuf *buffer); // runs emulator; called from start()


public:
    explicit SndThread (QObject *parent = 0);
    virtual ~SndThread();

    virtual int setSampleRate (int rate);

public slots:
    // Consumer API
    void start (SndBuf * buffer); // generates into the buffer until it's full

signals:
    void started (); // start (initial)
    void finished (SndBuf * buffer); // start (final)
};

#endif // SNDTHREAD_H
