#include "FactoryPresets.h"
#include <initializer_list>
namespace da {
std::array<FactoryPreset,50> factoryPresets(Kind kind) {
    const auto meta=parameters(kind);
    using V=std::pair<const char*,float>;
    struct Profile {const char *name;std::initializer_list<V> values;};
    // Explicit musical profiles; variants adjust relevant controls, never randomized state.
    const std::array<Profile,10> verb{{
      {"Vocal Air",{{"mode",1},{"predelay",32},{"decay",2.1f},{"tone",11000},{"lowcut",170}}},
      {"Drum Room",{{"mode",0},{"size",.32f},{"predelay",5},{"decay",.65f},{"tone",9000}}},
      {"Snare Plate",{{"mode",2},{"predelay",18},{"decay",1.35f},{"tone",10500},{"density",.95f}}},
      {"Piano Chamber",{{"mode",0},{"size",.65f},{"predelay",15},{"decay",1.8f},{"tone",8500}}},
      {"Acoustic Space",{{"mode",0},{"size",.42f},{"predelay",12},{"decay",1.1f},{"tone",10000}}},
      {"String Hall",{{"mode",1},{"size",.8f},{"predelay",28},{"decay",3.2f},{"tone",9500}}},
      {"Synth Cloud",{{"mode",1},{"size",.9f},{"predelay",45},{"decay",5},{"tone",8000}}},
      {"Bright Shimmer",{{"mode",3},{"size",.78f},{"predelay",38},{"decay",4},{"tone",12500}}},
      {"Dark Cathedral",{{"mode",1},{"size",1},{"predelay",65},{"decay",7},{"tone",4000}}},
      {"Short Ambience",{{"mode",0},{"size",.2f},{"predelay",2},{"decay",.35f},{"mix",.15f}}}
    }};
    const std::array<Profile,10> delay{{
      {"Vocal Eighth",{{"sync",1},{"division",6},{"feedback",.28f}}},
      {"Dotted Guitar",{{"sync",1},{"division",7},{"feedback",.42f},{"filter",7500}}},
      {"Quarter Echo",{{"sync",1},{"division",9},{"feedback",.35f}}},
      {"Triplet Motion",{{"sync",1},{"division",8},{"feedback",.32f}}},
      {"Sixteenth Pulse",{{"sync",1},{"division",3},{"feedback",.3f}}},
      {"Stereo Half",{{"sync",1},{"division",12},{"pingpong",1},{"feedback",.45f}}},
      {"Rock Slap",{{"time",95},{"feedback",.1f},{"pingpong",0},{"modulation",0}}},
      {"Tape Shadow",{{"time",310},{"filter",2800},{"feedback",.38f},{"modulation",.18f}}},
      {"Wide Doubler",{{"time",24},{"feedback",.06f},{"width",1.3f},{"pingpong",0}}},
      {"Ambient Cascade",{{"sync",1},{"division",13},{"feedback",.58f},{"filter",4500}}}
    }};
    const std::array<Profile,10> comp{{
      {"Vocal Level",{{"threshold",-22},{"ratio",3},{"attack",18},{"release",160}}},
      {"Drum Punch",{{"threshold",-15},{"ratio",4},{"attack",28},{"release",90}}},
      {"Bass Control",{{"threshold",-24},{"ratio",4},{"attack",12},{"release",220}}},
      {"Acoustic Even",{{"threshold",-20},{"ratio",2},{"attack",25},{"release",140}}},
      {"Piano Gentle",{{"threshold",-18},{"ratio",1.7f},{"attack",40},{"release",300}}},
      {"Bus Glue",{{"threshold",-12},{"ratio",2},{"attack",30},{"release",180}}},
      {"Parallel Weight",{{"threshold",-30},{"ratio",8},{"attack",8},{"mix",.45f}}},
      {"Transient Catch",{{"threshold",-14},{"ratio",6},{"attack",.8f},{"release",65}}},
      {"Soft Speech",{{"threshold",-28},{"ratio",2.8f},{"attack",8},{"release",120}}},
      {"Sustain Hold",{{"threshold",-26},{"ratio",5},{"attack",20},{"release",400}}}
    }};
    const std::array<Profile,10> eq{{
      {"Vocal Clarity",{{"b1gain",-2},{"b2gain",-2.5f},{"b3gain",2},{"b4gain",1.5f}}},
      {"Warm Voice",{{"b1gain",1},{"b2gain",1.5f},{"b3gain",-.5f},{"b4gain",-1}}},
      {"Drum Presence",{{"b1gain",2},{"b2gain",-3},{"b3gain",2.5f},{"b4gain",1}}},
      {"Bass Definition",{{"b1freq",70},{"b1gain",2},{"b2gain",-2},{"b3freq",1600},{"b3gain",2}}},
      {"Acoustic Detail",{{"b1gain",-2},{"b2gain",-1},{"b3gain",1},{"b4gain",2}}},
      {"Piano Body",{{"b1gain",1.5f},{"b2gain",-.5f},{"b3gain",-1},{"b4gain",1}}},
      {"String Silk",{{"b2gain",-1.5f},{"b3gain",-2},{"b4gain",1.5f}}},
      {"Synth Depth",{{"b1gain",3},{"b2gain",-2},{"b3gain",-1},{"b4gain",-2}}},
      {"Mix Polish",{{"b1shape",1},{"b1gain",.8f},{"b2gain",-1},{"b4shape",2},{"b4gain",1}}},
      {"Dark Texture",{{"b1gain",1},{"b3gain",-3},{"b4shape",2},{"b4gain",-4}}}
    }};
    const std::array<Profile,10> sat{{
      {"Vocal Warmth",{{"drive",4},{"tone",8500},{"density",.55f}}},
      {"Drum Edge",{{"drive",10},{"mode",1},{"mix",.3f}}},
      {"Bass Grit",{{"drive",12},{"tone",4200},{"mix",.4f}}},
      {"Acoustic Glow",{{"drive",3},{"tone",12000},{"mix",.25f}}},
      {"Piano Felt",{{"drive",5},{"tone",5500},{"density",.8f}}},
      {"Bus Color",{{"drive",2},{"tone",11000},{"mix",.2f}}},
      {"Synth Asymmetry",{{"mode",2},{"drive",11},{"mix",.45f}}},
      {"LoFi Shadow",{{"drive",9},{"tone",2200},{"density",.9f}}},
      {"Parallel Crunch",{{"mode",1},{"drive",16},{"mix",.2f}}},
      {"Smooth Harmonics",{{"drive",7},{"tone",8000},{"density",.7f}}}
    }};
    const std::array<Profile,10> trans{{
      {"Kick Punch",{{"attack",4},{"sustain",-2},{"sensitivity",.6f}}},
      {"Snare Snap",{{"attack",5},{"sustain",-3},{"shape",1.4f}}},
      {"Room Tamer",{{"attack",1},{"sustain",-7},{"shape",.8f}}},
      {"Bass Pick",{{"attack",3},{"sustain",1},{"sensitivity",.4f}}},
      {"Acoustic Pluck",{{"attack",2},{"sustain",-1},{"shape",1.2f}}},
      {"Soft Piano",{{"attack",-4},{"sustain",1.5f},{"shape",.7f}}},
      {"Percussion Tight",{{"attack",3},{"sustain",-5},{"sensitivity",.7f}}},
      {"Pad Bloom",{{"attack",-6},{"sustain",3},{"shape",.6f}}},
      {"Drum Sustain",{{"attack",-1},{"sustain",4},{"mix",.6f}}},
      {"Parallel Attack",{{"attack",7},{"sustain",-4},{"mix",.4f}}}
    }};
    const std::array<Profile,10> stereo{{
      {"Vocal Focus",{{"width",.8f},{"center",.5f},{"side",-1}}},
      {"Drum Stage",{{"width",1.12f},{"depth",.05f}}},
      {"Bass Center",{{"width",.4f},{"side",-5},{"depth",.5f}}},
      {"Acoustic Pair",{{"width",1.18f},{"side",-.5f}}},
      {"Piano Stage",{{"width",1.1f},{"center",.3f},{"depth",.1f}}},
      {"String Field",{{"width",1.25f},{"depth",.2f}}},
      {"Synth Spread",{{"width",1.4f},{"center",-.5f}}},
      {"Dark Sides",{{"width",1.15f},{"depth",.7f}}},
      {"Narrow Room",{{"width",.65f},{"center",.5f}}},
      {"Mix Perspective",{{"width",1.05f},{"side",-.5f},{"depth",.15f}}}
    }};
    const std::array<Profile,10> sess{{
      {"Vocal Sibilance",{{"frequency",6500},{"threshold",-25},{"range",5}}},
      {"Bright Voice",{{"frequency",8000},{"threshold",-27},{"range",7}}},
      {"Low Voice",{{"frequency",4500},{"threshold",-24},{"range",4}}},
      {"Soft Speech",{{"frequency",5500},{"threshold",-30},{"range",4},{"release",110}}},
      {"Cymbal Restraint",{{"frequency",8500},{"threshold",-22},{"range",3},{"attack",4}}},
      {"Acoustic Pick",{{"frequency",4000},{"threshold",-23},{"range",3}}},
      {"Air Retention",{{"frequency",9500},{"threshold",-24},{"range",2},{"focus",.4f}}},
      {"Hard Consonants",{{"frequency",5000},{"threshold",-28},{"range",7},{"attack",.5f}}},
      {"Bus High Tamer",{{"frequency",7000},{"threshold",-18},{"range",2},{"release",160}}},
      {"Parallel Silk",{{"frequency",6000},{"threshold",-30},{"range",8},{"mix",.5f}}}
    }};
    const std::array<Profile,10> gate{{
      {"Vocal Cleanup",{{"threshold",-45},{"hold",90},{"release",180},{"range",24}}},
      {"Kick Isolation",{{"threshold",-32},{"hold",40},{"release",100},{"range",50}}},
      {"Snare Isolation",{{"threshold",-30},{"hold",70},{"release",140},{"range",45}}},
      {"Tom Decay",{{"threshold",-36},{"hold",110},{"release",280},{"range",48}}},
      {"Guitar Noise",{{"threshold",-48},{"hold",70},{"release",170},{"range",36}}},
      {"Bass Tail",{{"threshold",-50},{"hold",130},{"release",350},{"range",24}}},
      {"Speech Pause",{{"threshold",-43},{"attack",4},{"hold",160},{"range",18}}},
      {"Percussion Tight",{{"threshold",-34},{"hold",18},{"release",55},{"range",45}}},
      {"Room Reduction",{{"threshold",-38},{"hold",45},{"release",230},{"range",15}}},
      {"Gentle Floor",{{"threshold",-55},{"hold",180},{"release",400},{"range",12}}}
    }};
    const std::array<Profile,10> fx{{
      {"Vocal Halo",{{"drive",2},{"time",320},{"delayMix",.12f},{"decay",1.8f},{"mix",.25f}}},
      {"Drum Space",{{"drive",4},{"time",110},{"decay",.6f},{"delayMix",.08f}}},
      {"Guitar Trail",{{"drive",5},{"time",375},{"feedback",.4f},{"decay",2.2f}}},
      {"Piano Cloud",{{"drive",1},{"time",500},{"decay",3.5f},{"filter",10000}}},
      {"Bass Texture",{{"drive",7},{"filter",3800},{"delayOn",0},{"decay",.5f},{"reverbMix",.08f}}},
      {"Synth Orbit",{{"time",650},{"feedback",.5f},{"decay",4},{"mix",.55f}}},
      {"LoFi Room",{{"drive",9},{"filter",2500},{"time",90},{"decay",.8f}}},
      {"Dark Cinematic",{{"filter",3200},{"time",850},{"feedback",.55f},{"decay",5.5f}}},
      {"Percussion Spark",{{"drive",3},{"filter",14000},{"time",180},{"decay",1.1f}}},
      {"Ambient Wash",{{"drive",2},{"time",1100},{"feedback",.6f},{"decay",6},{"reverbMix",.45f}}}
    }};
    const std::array<Profile,10>* profiles=&verb;
    switch(kind) {
     case Kind::Verb:profiles=&verb;break;case Kind::Delay:profiles=&delay;break;
     case Kind::Comp:profiles=&comp;break;case Kind::EQ:profiles=&eq;break;
     case Kind::Sat:profiles=&sat;break;case Kind::Trans:profiles=&trans;break;
     case Kind::Stereo:profiles=&stereo;break;case Kind::Sess:profiles=&sess;break;
     case Kind::Gate:profiles=&gate;break;case Kind::FX:profiles=&fx;break;
    }
    std::array<FactoryPreset,50> bank;
    const char* variants[]{"Natural","Focused","Soft","Open","Parallel"};
    for(size_t group=0;group<10;++group)for(size_t v=0;v<5;++v) {
        auto &p=bank[group*5+v];p.name=std::string((*profiles)[group].name)+" / "+variants[v];p.category=(*profiles)[group].name;
        for(const auto &m:meta)p.values.push_back(m.initial);
        const auto set=[&](const char* id,float value){for(size_t i=0;i<meta.size();++i)if(meta[i].id==id)p.values[i]=meta[i].constrain(value);};
        const auto get=[&](const char* id){for(size_t i=0;i<meta.size();++i)if(meta[i].id==id)return p.values[i];return 0.f;};
        for(const auto &pair:(*profiles)[group].values)set(pair.first,pair.second);
        const float amount=float(v);
        switch(kind) {
         case Kind::Verb:set("decay",get("decay")*(.85f+.12f*amount));set("predelay",get("predelay")+3*amount);set("tone",get("tone")*(.82f+.09f*amount));set("width",.85f+.09f*amount);set("mix",v==4?.14f:.2f+.025f*amount);break;
         case Kind::Delay:set("filter",get("filter")*(.75f+.14f*amount));set("feedback",get("feedback")+.025f*amount);set("mix",v==4?.12f:.18f+.035f*amount);set("width",.8f+.12f*amount);break;
         case Kind::Comp:set("threshold",get("threshold")-1.25f*amount);set("release",get("release")*(.8f+.12f*amount));set("knee",3+3*amount);if(v==4)set("mix",.5f);break;
         case Kind::EQ:for(int b=0;b<4;++b){const auto id="b"+std::to_string(b+1)+"gain";set(id.c_str(),get(id.c_str())*(.7f+.15f*amount));}set("b2q",.55f+.12f*amount);break;
         case Kind::Sat:set("drive",get("drive")+.7f*amount);set("tone",get("tone")*(.85f+.08f*amount));set("output",-1.5f-amount);if(v==4)set("mix",.18f);break;
         case Kind::Trans:set("attack",get("attack")*(.7f+.2f*amount));set("sustain",get("sustain")-.5f*amount);set("sensitivity",.35f+.1f*amount);if(v==4)set("mix",.5f);break;
         case Kind::Stereo:set("width",get("width")*(.85f+.07f*amount));set("depth",get("depth")+.04f*amount);set("side",get("side")-.25f*amount);break;
         case Kind::Sess:set("threshold",get("threshold")-amount);set("release",get("release")*(.85f+.12f*amount));set("focus",.45f+.1f*amount);if(v==4)set("mix",.6f);break;
         case Kind::Gate:set("threshold",get("threshold")+amount);set("hysteresis",3+amount);set("release",get("release")*(.85f+.1f*amount));break;
         case Kind::FX:set("filter",get("filter")*(.8f+.07f*amount));set("feedback",get("feedback")+.025f*amount);set("decay",get("decay")*(.85f+.1f*amount));set("mix",v==4?.18f:.3f+.04f*amount);break;
        }
    }
    bank[0].name="Init / Factory Default";bank[0].category="Init";
    for(size_t i=0;i<meta.size();++i)bank[0].values[i]=meta[i].initial;
    return bank;
}
}
