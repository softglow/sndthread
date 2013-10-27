#include <cmath>
#include "sndbloop.h"

SndBloop::SndBloop(QObject *parent) :
    SndThread(parent), frequency(440)
{
    basicInit();
}

SndBloop::SndBloop (int freq, QObject *parent) :
    SndThread(parent), frequency(freq)
{
    basicInit();
}

void SndBloop::recalculate() {
    int sign = delta > 0 ? 1 : -1;
    // samples-per-cycle = seconds-per-cycle * samples-per-second
    // seconds-per-cycle = 1/frequency
    float period_samples = sample_rate / frequency;

    limit = 1 << (8*sizeof(snd_sample_t) - 1);

    // we have to cover 0-LIMIT four times (+LIMIT 0 -LIMIT 0 ...) per period
    delta = float(limit * 4) / period_samples;
    delta *= sign;
}

void SndBloop::basicInit() {
    // inherited
    channels = 2;
    sample_rate = 48000;

    // ours
    prev = 0;
    delta = 1; // just for the sign

    recalculate();
}

void SndBloop::getNext(snd_sample_t *frame) {
    float cur;
    float future;
    int i;

    cur = prev + delta;

    for (i = 0; i < channels; ++i) {
        frame[i] = snd_sample_t(cur);
    }

    future = cur + delta;
    if (future >= limit - 1 || future <= -limit) {
        delta *= -1;
    }

    prev = cur;
}

void SndBloop::generate(SndBuf * buf) {
    int frame_len;
    snd_sample_t frame[channels];

    frame_len = buf->free() / channels / sizeof(snd_sample_t);

    while (frame_len--) {
        getNext(frame);
        buf->append(frame, channels);
    }
}

int SndBloop::getFrequency() {
    return frequency;
}

int SndBloop::setFrequency (int freq) {
    if (freq > 15 && freq < 22050)
        frequency = freq;
    return frequency;
}

int SndBloop::setSampleRate(int rate) {
    int rv;

    if (rate == sample_rate)
        return rate;

    rv = SndThread::setSampleRate(rate);
    recalculate();
    return rv;
}
