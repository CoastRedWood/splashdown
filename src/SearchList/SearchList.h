// SearchList — partial layout reconstructed from retail accessors.
#ifndef SEARCHLIST_SEARCHLIST_H
#define SEARCHLIST_SEARCHLIST_H

struct _D3DVECTOR;
class SearchableObject;

#include "SearchMethod/SearchMethod.h"

class SearchList : public SearchMethod {
public:
    SearchList(int maxObjects);
    unsigned int NextPlacementCode(unsigned int code);
    virtual ~SearchList();
    virtual unsigned int CalculatePointPlacementCode(_D3DVECTOR *, unsigned int);
    virtual unsigned int CalculatePointPlacementCode(float, float, float, unsigned int);
    virtual unsigned int CalculateBoxPlacementCode(_D3DVECTOR *, _D3DVECTOR *, unsigned int);
    virtual void InsertObject(SearchableObject *, unsigned int, _D3DVECTOR *, _D3DVECTOR *);
    virtual void RemoveObject(SearchableObject *, unsigned int);
    virtual int OpenSearch(_D3DVECTOR *, _D3DVECTOR *, _D3DVECTOR *, _D3DVECTOR *);
    virtual int OpenSearch(_D3DVECTOR *, _D3DVECTOR *, int *, SearchableObject ***, SearchableObject **);
    virtual SearchableObject *NextSearchObject();
    virtual void RewindSearch();
    virtual void CloseSearch();
    virtual void PerformPrepareGeometry();
    virtual void PerformRender3D();

    int m_maxObjects;               // +0x30, objects per block
    int m_numBlocks;                // +0x34
    int m_lastBlockCount;           // +0x38, objects in the last block
    SearchableObject ***m_blocks;   // +0x3C
    unsigned int m_freeList;        // +0x40 (RemoveObject)
    int m_searchIndex;              // +0x44
    int m_searchCount;              // +0x48
};

#endif
