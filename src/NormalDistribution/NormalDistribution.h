// NormalDistribution: a table of the normal density over 0..3 sigma (a
// cubic falloff), sampled by GetProbability. NormalDistribution.cpp's static
// initializer builds GlobalNormalDistribution(100); EcoSystem::GenerateEcoSystem
// samples one through LpNormalDistribution.
#ifndef NORMALDISTRIBUTION_NORMALDISTRIBUTION_H
#define NORMALDISTRIBUTION_NORMALDISTRIBUTION_H

class NormalDistribution {
public:
    NormalDistribution(int resolution);
    ~NormalDistribution();
    float GetProbability(float mean, float x, float sigma);

    float *m_table;                     // +0x0, density over 0..3 sigma
    int m_tableSize;                    // +0x4
};

extern NormalDistribution GlobalNormalDistribution;

#endif
