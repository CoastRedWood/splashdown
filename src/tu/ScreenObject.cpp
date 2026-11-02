// ScreenObject.cpp  @ 0x00393650 (text 0x393650-0x393920)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's ScreenObject.cpp as one translation unit: a TU without a static
// initializer at the end of JetskiTraining.cpp's tu_map row (its RTTI
// restarts at BaseObject, after ManagedTextureGroup.cpp's).



#include "ScreenObject/ScreenObject.h"

// ---- src/ScreenObject/ctor.cpp
ScreenObject::ScreenObject(int enabled) : GameObject(enabled)
{
    m_iconManager = 0;
    m_textManager = 0;
    m_primitiveManager = 0;
    m_renderOrder[0] = 0;
    m_renderOrder[1] = 1;
    m_renderOrder[2] = 2;
    m_managers[0] = m_managers[1] = m_managers[2] = 0;
}

// ---- src/ScreenObject/dtor.cpp
ScreenObject::~ScreenObject()
{
    if (m_iconManager) {
        m_iconManager->Release();
        m_iconManager = 0;
    }
    if (m_primitiveManager) {
        m_primitiveManager->Release();
        m_primitiveManager = 0;
    }
    if (m_textManager) {
        m_textManager->Release();
        m_textManager = 0;
    }
}

// ---- src/ScreenObject/Create.cpp
ScreenObject *ScreenObject::Create(int maxIcons, int maxPrimitives, int maxTexts)
{
    m_iconManager = new ScreenIconManager(1);
    m_iconManager = m_iconManager->Create(maxIcons);
    m_managers[0] = m_iconManager;
    m_primitiveManager = new ScreenPrimitiveManager(1);
    m_primitiveManager = m_primitiveManager->Create(maxPrimitives);
    m_managers[1] = m_primitiveManager;
    m_textManager = new ScreenTextManager(0, 1);
    m_textManager = m_textManager->Create(maxTexts);
    m_managers[2] = m_textManager;
    return this;
}

// ---- src/ScreenObject/Render3D.cpp
int ScreenObject::Render3D()
{
    // Left over from a removed profiling meter.
    static int meterID = -1;

    for (int i = 0; i < 3; i++) {
        if (m_managers[m_renderOrder[i]])
            m_managers[m_renderOrder[i]]->Render3D();
    }
    return 1;
}
