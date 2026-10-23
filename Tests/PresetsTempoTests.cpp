#include "Core/FactoryPresets.h"
#include <set>
#include <iostream>
#include <stdexcept>
#include <limits>
void check(bool ok,const char *m){if(!ok)throw std::runtime_error(m);}
int main(){try{
 for(int k=0;k<10;++k){const auto kind=static_cast<da::Kind>(k);const auto meta=da::parameters(kind);
  const auto bank=da::factoryPresets(kind);std::set<std::string> names;std::set<std::vector<float>> states;
  for(const auto &p:bank){names.insert(p.name);states.insert(p.values);check(p.values.size()==meta.size(),"Incomplete preset");
   for(size_t i=0;i<meta.size();++i)check(std::isfinite(p.values[i])&&meta[i].constrain(p.values[i])==p.values[i],"Unbounded preset");
   da::Engine e(kind);e.prepare(48000);std::vector<float> l(4096),r(4096);
   for(size_t i=0;i<l.size();++i){l[i]=.15f*std::sin(float(i)*.031f);r[i]=.12f*std::sin(float(i)*.047f);}
   e.process(l.data(),r.data(),l.size(),p.values.data());
   for(size_t i=0;i<l.size();++i)check(std::isfinite(l[i])&&std::isfinite(r[i]),"Preset DSP nonfinite");
  }
  check(names.size()==50&&states.size()==50,"50 unique names AND parameter snapshots required");
  for(size_t i=0;i<meta.size();++i)check(bank[0].values[i]==meta[i].initial,"Init must equal source defaults");
  std::cout<<"PASS bank "<<k<<": 50 complete unique bounded presets, audio smoke\n";
 }
 check(std::abs(da::syncedDelayMs(120,9)-500)<1e-9,"Quarter 120 BPM");
 check(std::abs(da::syncedDelayMs(120,7)-375)<1e-9,"Dotted eighth");
 check(std::abs(da::syncedDelayMs(120,8)-1000./6)<1e-9,"Triplet eighth");
 check(da::syncedDelayMs(10,14)==24000,"Whole note low BPM must not be limited to manual 2000 ms");
 auto meta=da::parameters(da::Kind::Delay);std::vector<float> v;
 for(const auto &p:meta)v.push_back(p.initial);
 for(size_t i=0;i<meta.size();++i){if(meta[i].id=="sync")v[i]=1;if(meta[i].id=="division")v[i]=9;if(meta[i].id=="mix")v[i]=1;if(meta[i].id=="feedback")v[i]=0;}
 for(double bpm:{60.,120.,240.}) {
  da::Engine e(da::Kind::Delay);e.prepare(48000);e.setTempo(bpm);
  std::vector<float> warm(24000),other(warm);e.process(warm.data(),other.data(),warm.size(),v.data());
  std::vector<float> l(50000),r(l);l[0]=r[0]=.25f;e.process(l.data(),r.data(),l.size(),v.data());
  size_t first=0;while(first<l.size()&&std::abs(l[first])<1e-5f)++first;
  const auto expected=size_t(std::llround(48000*60/bpm));
  check(first>=expected-1&&first<=expected+1,"Actual impulse arrival must follow host quarter-note timing");
  for(size_t i=0;i<l.size();++i)check(std::abs(l[i]-r[i])<1e-7,"Synced L/R grid must not have 1.1% drift");
  e.setTempo(std::numeric_limits<double>::quiet_NaN());check(!e.tempoIsValid()&&e.tempo()==bpm,"Hold last valid tempo");
 }
 std::cout<<"PASS note math, long divisions, audio impulse at 60/120/240 BPM, stereo alignment, missing-tempo hold\n";
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
