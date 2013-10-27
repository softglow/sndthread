#include "sndbuf.h"

bool SndBuf::allocate(int want_kb) {
    int len;

    if (buf)
        return false;

    len = 1024 * want_kb / sizeof(snd_sample_t);
    buf = new snd_sample_t[len];

    rptr = wptr = buf;
    end = buf + len;
    kb = want_kb;
    return true;
}

bool SndBuf::release() {
    if (! buf)
        return false;

    delete[] buf;
    buf = rptr = wptr = end = 0;
    kb = 0;
    return true;
}


SndBuf::SndBuf() {
    buf = 0;
    allocate(192); // 192 KB / 4 B:frame / 48 KHz ==> 1.0 seconds
}

SndBuf::SndBuf(int want_kb) {
    buf = 0;
    allocate(want_kb);
}

SndBuf::~SndBuf() {
    release();
}

bool SndBuf::sizeMatches (int want_kb) {
    return want_kb == kb;
}

bool SndBuf::resize (int want_kb) {
    snd_sample_t *old_buf = buf;
    int old_kb = kb;

    if (allocate(want_kb)) {
        delete[] old_buf;
        return true;
    } else {
        // end is only set on success of allocate() so we don't need to recompute it
        rptr = wptr = buf = old_buf;
        kb = old_kb;
        return false;
    }
}

void SndBuf::clear() {
    rptr = wptr = buf;
}

int SndBuf::used () {
    return buf && wptr && wptr >= buf && wptr <= end ?
                (wptr - buf) * sizeof(snd_sample_t) :
                -1;
}

int SndBuf::free () {
    return buf && end && wptr && buf <= wptr && wptr <= end ?
                (end - wptr) * sizeof(snd_sample_t) :
                -1;
}

int SndBuf::size () {
    return buf && end && end >= buf ?
                (end - buf) * sizeof(snd_sample_t) :
                -1;
}

int SndBuf::pos () {
    return buf && rptr && wptr && rptr >= buf && rptr <= wptr ?
                rptr - buf :
                -1;
}

int SndBuf::append(const snd_sample_t *data, int length) {
    int copied = 0;
    if (! (buf && end >= buf))
        return copied;

    while (wptr < end && copied < length) {
        *wptr++ = data[copied++];
    }
    return copied;
}

int SndBuf::shift(snd_sample_t *out) {
    if (! (buf && rptr && rptr >= buf && rptr <= end))
        return -1;
    if (rptr == end)
        return 0;

    *out = *rptr++;
    return 1;
}

int SndBuf::shift(snd_sample_t *out, int count) {
    int copied = 0;

    if (! (buf && rptr && rptr >= buf && rptr <= end))
        return -1;

    while (rptr < end && copied < count) {
        *out++ = *rptr++;
        ++copied;
    }

    return copied;
}
