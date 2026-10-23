#include "Core/Suite.h"
#include <iostream>
#include <stdexcept>
#include <vector>
void check(bool ok,const char *s){if(!ok)throw std::runtime_error(s);}
int main(){try{
 const auto meta=da::parameters(da::Kind::Verb);std::vector<float> v;for(const auto &p:meta)v.push_back(p.initial);
 const auto set=[&](const char *id,float x){for(size_t i=0;i<meta.size();++i)if(meta[i].id==id)v[i]=x;};
 set("mix",1);set("modulation",0);set("decay",1.5f);
 auto render=[&](float amplitude,int block){da::Engine e(da::Kind::Verb);e.prepare(48000);
   std::vector<float> warm(24000),wr(warm);e.process(warm.data(),wr.data(),warm.size(),v.data());
   std::vector<float> l(240000),r(l);l[0]=r[0]=amplitude;
   for(size_t i=0;i<l.size();i+=size_t(block))e.process(l.data()+i,r.data()+i,std::min(size_t(block),l.size()-i),v.data());
   return std::make_pair(l,r);};
 auto normal=render(.1f,257),small=render(.01f,257),differentBlocks=render(.1f,64);
 double early=0,late=0,stereo=0;size_t occupied=0;
 for(size_t i=0;i<normal.first.size();++i){const auto a=normal.first[i],b=normal.second[i];
  check(std::isfinite(a)&&std::isfinite(b),"Finite impulse response");
  check(std::abs(a-small.first[i]*10)<2e-6,"Normal-level reverb must stay linear, not saturate on every circulation");
  check(std::abs(a-differentBlocks.first[i])<1e-7,"Reverb must not depend on host block boundaries");
  if(i>2400&&i<24000){early+=a*a+b*b;if(std::abs(a)+std::abs(b)>1e-7)++occupied;stereo+=(a-b)*(a-b);}
  if(i>192000)late+=a*a+b*b;
 }
 check(early>1e-5&&stereo>1e-7&&occupied>12000,"Dense non-dual-mono late field");
 check(late<early*1e-4,"Tail must decay, not sustain a self-excited loop");
 set("tone",2000);auto dark=render(.1f,256);
 set("tone",14000);auto bright=render(.1f,256);
 const auto d=da::analyze(dark.first.data()+48000,dark.second.data()+48000,48000,48000);
 const auto b=da::analyze(bright.first.data()+48000,bright.second.data()+48000,48000,48000);
 std::cout<<"Tone HF ratios: dark="<<d.bands[3]/d.rms<<" rms="<<d.rms<<" bright="<<b.bands[3]/b.rms<<" rms="<<b.rms<<"\n";
 check(b.bands[3]/b.rms>d.bands[3]/d.rms*1.3,"Tone must retain controllable high-frequency tail detail");
 std::cout<<"PASS reverb: linear small-signal path, dense stereo IR, sample-block invariance, decaying tail\n";
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
