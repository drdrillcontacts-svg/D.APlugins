#include "Core/Suite.h"
#include <iostream>
int main() {
    const char* names[]{"DVerb","DDelay","DComp","DEQ","DSat","DTrans","DStereo","DSess","DGate","DFX"};
    std::cout << "{";
    for(int k=0;k<10;++k){if(k)std::cout<<",";std::cout<<"\""<<names[k]<<"\":[";
        auto p=da::parameters(static_cast<da::Kind>(k));
        for(size_t i=0;i<p.size();++i){if(i)std::cout<<",";auto& a=p[i];
            std::cout<<"{\"id\":\""<<a.id<<"\",\"name\":\""<<a.name<<"\",\"module\":\""<<a.module<<"\",\"unit\":\""<<a.unit<<"\",\"min\":"<<a.min<<",\"max\":"<<a.max<<",\"initial\":"<<a.initial<<",\"discrete\":"<<(a.discrete?"true":"false")<<",\"learnable\":"<<(da::canLearn(static_cast<da::Kind>(k), a.id)?"true":"false")<<"}";}
        std::cout<<"]";}
    std::cout<<"}\n";
}
