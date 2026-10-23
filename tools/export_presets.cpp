#include "Core/FactoryPresets.h"
#include <iostream>
#include <iomanip>
#include <limits>
int main(){
 const char *names[]{"DVerb","DDelay","DComp","DEQ","DSat","DTrans","DStereo","DSess","DGate","DFX"};
 std::cout<<std::setprecision(std::numeric_limits<float>::max_digits10)<<"{\"schema\":1,\"version\":\"0.4.0\",\"banks\":[";
 for(int k=0;k<10;++k){if(k)std::cout<<',';const auto kind=static_cast<da::Kind>(k);
  const auto bank=da::factoryPresets(kind);const auto meta=da::parameters(kind);
  std::cout<<"{\"plugin\":\""<<names[k]<<"\",\"presets\":[";
  for(size_t p=0;p<bank.size();++p){if(p)std::cout<<',';
   std::cout<<"{\"index\":"<<p+1<<",\"name\":\""<<bank[p].name<<"\",\"category\":\""<<bank[p].category<<"\",\"parameters\":{";
   for(size_t i=0;i<meta.size();++i){if(i)std::cout<<',';std::cout<<'"'<<meta[i].id<<"\":"<<bank[p].values[i];}
   std::cout<<"}}";
  }
  std::cout<<"]}";
 }
 std::cout<<"],\"divisions\":[";
 for(size_t i=0;i<da::divisionNames.size();++i){if(i)std::cout<<',';std::cout<<'"'<<da::divisionNames[i]<<'"';}
 std::cout<<"]}\n";
}
