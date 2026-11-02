// Rider_Stream_Info: one preloaded rider speech stream (the rider sound
// emitters' ContainerList<Rider_Stream_Info *> lists), 8 bytes.
#ifndef RIDER_STREAM_INFO_RIDER_STREAM_INFO_H
#define RIDER_STREAM_INFO_RIDER_STREAM_INFO_H

struct Rider_Stream_Info {
    char *m_name;                       // +0x00, malloc'd
    int m_stream;                       // +0x04, -1 if it failed to load
};

#endif
