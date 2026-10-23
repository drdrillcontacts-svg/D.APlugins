// Original synthetic percussion/plucked-tone stem; offline listening aid, NOT a listening certification.
#include "Core/Suite.h"
#include <fstream>
#include <iostream>
int main(int argc,char **argv){if(argc!=2)return 1;
 const size_t n=48000*8;std::vector<float> l(n),r(n),dry(n*2);
 uint32_t seed=12345;
 for(size_t i=0;i<n;++i){double t=double(i)/48000;float x=0;
  for(double onset:{.05,.35,.65,1.,1.45})if(t>=onset&&t<onset+.8){double d=t-onset;
   seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;
   float noise=float(seed%65536)/32768-1;
   x+=float(.13*std::exp(-d*7)*(std::sin(2*da::pi*330*d)+.4*std::sin(2*da::pi*990*d))+.09*noise*std::exp(-d*55));
  }
  l[i]=x;r[i]=x*.96f;dry[2*i]=l[i];dry[2*i+1]=r[i];
 }
 auto m=da::parameters(da::Kind::Verb);std::vector<float> v;
 for(auto &p:m)v.push_back(p.initial);
 auto set=[&](const char *id,float x){for(size_t i=0;i<m.size();++i)if(m[i].id==id)v[i]=x;};
 set("predelay",25);set("size",.6f);set("decay",2.2f);set("tone",10000);set("mix",1);set("width",1);
 set("density",.8f);set("diffusion",.8f);set("modulation",.08f);set("lowcut",120);set("midcut",-1);set("mode",1);
 da::Engine engine(da::Kind::Verb);engine.prepare(48000);
 std::vector<float> w(24000),z(w);engine.process(w.data(),z.data(),w.size(),v.data());
 engine.process(l.data(),r.data(),n,v.data());std::vector<float> wet(n*2);
 for(size_t i=0;i<n;++i){wet[2*i]=l[i];wet[2*i+1]=r[i];}
 std::ofstream f(argv[1],std::ios::binary);f.write(reinterpret_cast<const char*>(wet.data()),std::streamsize(wet.size()*sizeof(float)));
 std::ofstream d(std::string(argv[1])+".dry",std::ios::binary);d.write(reinterpret_cast<const char*>(dry.data()),std::streamsize(dry.size()*sizeof(float)));
 std::cout<<"Rendered 8 s synthetic 48 kHz stereo wet/dry\n";
}
