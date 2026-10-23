#pragma once
#include "Suite.h"
namespace da {
struct FactoryPreset { std::string name, category; std::vector<float> values; };
// Fully materialized at construction, never generated in processBlock or a program callback.
std::array<FactoryPreset,50> factoryPresets(Kind);
}
