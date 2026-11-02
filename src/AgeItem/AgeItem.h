// AgeItem: one entry an AgeManager tracks (a cached texture or mesh), with
// the release callback Purge calls; 0x14 bytes.
#ifndef AGEITEM_AGEITEM_H
#define AGEITEM_AGEITEM_H

struct AgeItem {
    unsigned int m_birthTime;           // +0x00, the manager's clock when last touched
    int (*m_callback)(void *, void *);  // +0x04
    void *m_context;                    // +0x08
    void *m_data;                       // +0x0C
    int m_size;                         // +0x10, summed by GetSize, weighed by Purge
};

#endif
