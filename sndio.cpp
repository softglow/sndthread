#include <QtGlobal>
#include <QMutexLocker>
#include "sndio.h"

void SndIO::updateBuffers() {
    if (! (out_empty && gen_full))
        return;

    // swap the buffers
    out_empty = false;
    gen_full = false;

    gen_buf ^= out_buf;
    out_buf ^= gen_buf;
    gen_buf ^= out_buf;

    if (out_buf < 0 || gen_buf < 0)
        qFatal("out_buf or gen_buf underflow");
    if (out_buf > 1 || gen_buf > 1)
        qFatal("out_buf or gen_buf overflow");
    if (out_buf == gen_buf)
        qFatal("out_buf == gen_buf");

    bufs[gen_buf].clear();
}


SndIO::SndIO(const QAudioFormat &in_fmt, QObject *parent) :
    QIODevice(parent),
    fmt(in_fmt),
    buf_lock(QMutex::Recursive),
    gen_buf(0),
    out_buf(1),
    out_empty(true),
    gen_full(false)
{
}

SndIO::~SndIO()
{
}

void SndIO::start() {
    bool really = true;

    buf_lock.lock();
    if (gen_full || ! out_empty || gen_buf == out_buf)
        really = false;
    buf_lock.unlock();

    if (really)
        emit readyWrite(&bufs[gen_buf]);
}

qint64 SndIO::readData(char *data, qint64 maxlen) {
    QMutexLocker locker(&buf_lock);
    SndBuf * b = &bufs[out_buf];

    qint64 available;
    qint64 copy;
    qint64 written = 0;

    int samples;

    // set up for snd_sample_t to be signed 8|16-bit integer
    // we will handle both endian modes but NOT float/unsigned types
    int shifts[] = { 0, 8 };
    int masks[] = { 0xFF, 0xFF00 };
    int order[] = { 0, 1 };
    int i;

    int shifted;
    snd_sample_t s;
    bool was_gen_full = gen_full;
    bool was_out_empty = out_empty;
    bool emit_emptied = false;

    // FIXME: find channels somehow, not hardcoded 2
    if (maxlen < 2*int(sizeof(snd_sample_t)))
        return 0;

    samples = b->used() / sizeof(snd_sample_t);
    if (samples > 0)
        samples -= b->pos();

    if (samples < 0)
        return -1; // buffer is crashed

    copy = maxlen / sizeof(snd_sample_t);
    available = qint64(samples);
    if (available <= 0) {
        written = readSilence(data, copy);
        copy = 0;
        out_empty = true;
        emit underrun();
    }
    else if (copy > available) {
        copy = available;
        out_empty = true;
    }

    if (fmt.byteOrder() != QAudioFormat::LittleEndian) {
        order[0] ^= 1;
        order[1] ^= 1;
    }

    while (copy > written) {
        shifted = b->shift(&s);
        if (shifted < 1) {
            out_empty = true;
            break;
        }

        for (i = 0; i < int(sizeof(snd_sample_t)); i++) {
            *data++ = (s & masks[order[i]]) >> shifts[order[i]];
        }

        ++written;
    }

    if (out_empty && ! was_out_empty)
        emit_emptied = true;

    updateBuffers();

    if (emit_emptied)
        emit emptied();

    if (was_gen_full && ! gen_full)
        emit readyWrite(&bufs[gen_buf]);

    return written * sizeof(snd_sample_t);
}

qint64 SndIO::writeData(const char *data, qint64 len) {
    Q_UNUSED(data);
    Q_UNUSED(len);
    qWarning() << "SndIO::writeData - unimplemented!!";
    return -1; // ENOSYS
}

qint64 SndIO::readSilence(char *data, qint64 samples) {
    qint64 written = 0;
    int i;

    if (samples > 1024)
        samples = 1024;
    else
        samples &= ~1LL; // ensure round number of stereo frames

    qDebug() << "feeding" << samples << "samples of silence on underrun";

    while (samples > written) {
        for (i = 0; i < int(sizeof(snd_sample_t)); i++)
            *data++ = 0;

        ++written;
    }

    return written;
}

void SndIO::writeComplete() {
    QMutexLocker locker(&buf_lock);
    qint64 bytes = qint64(bufs[gen_buf].used());

    gen_full = true;

    emit bytesWritten(bytes);
    emit readyRead();

    updateBuffers();

    if (! gen_full)
        emit readyWrite(&bufs[gen_buf]);
}
