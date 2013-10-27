#include "sndthread.h"

void SndThread::generate (SndBuf * buffer) {
    snd_sample_t silence[] = {0};
    while (buffer->free() > 0)
        buffer->append(&silence[0], 1);
}


SndThread::SndThread(QObject *parent) : QObject(parent)
{
}

SndThread::~SndThread() {
}


int SndThread::setSampleRate(int rate) {
    if (rate < 8000) rate = 8000;
    if (rate > 192000) rate = 192000;

    sample_rate = rate;
    return sample_rate;
}

void SndThread::start(SndBuf *buffer) {
    emit started();
    generate(buffer);
    emit finished(buffer);
}
