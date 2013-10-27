#ifndef SNDBUF_H
#define SNDBUF_H

#include "sndthread_global.h"

typedef short snd_sample_t; // lazy, inflexible hack: I know the SNES is 16-bit

class SNDTHREADSHARED_EXPORT SndBuf
{
private:
    int kb;  // actually allocated buffer size
    snd_sample_t * buf; // base pointer
    snd_sample_t * wptr; // current output position
    snd_sample_t * rptr; // current reader position
    snd_sample_t * end; // immediately past the end of the buffer

    bool allocate (int want_kb); // allocates a fresh buffer (if none)
    bool release (); // deallocates the buffer (if present)

public:
    SndBuf();
    SndBuf(int want_kb);
    ~SndBuf();

    bool sizeMatches(int want_kb);
    bool resize(int want_kb);
    void clear();
    // used/free/size => wptr kB; pos => samples
    int used();
    int free();
    int size(); // used + free
    int pos();  // rptr position
    int append(const snd_sample_t *data, int length);
    int shift(snd_sample_t *out);
    int shift(snd_sample_t *out, int count);
};

#endif // SNDBUF_H
