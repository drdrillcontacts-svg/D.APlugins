#include "Core/InputCapture.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <stdexcept>
void check(bool ok, const char *message) { if(!ok) throw std::runtime_error(message); }
int main() {
 try {
    da::InputCapture c; c.prepare(48000);
    float left[512], right[512];
    std::fill_n(left,512,.25f);std::fill_n(right,512,-.125f);
    std::vector<float> l,r; double sr=0;
    check(c.request(),"Initial request"); c.cancel();
    check(c.request(),"Restart cancelled capture without running transport");
    c.process(left,right,512);c.cancel();
    check(c.request(),"Restart partially filled capture");
    for(int i=0;i<128;++i)c.process(left,right,512);
    check(c.take(l,r,sr)&&sr==48000&&l.size()==65536,"Complete stereo capture");
    for(size_t i=0;i<l.size();++i)check(l[i]==.25f&&r[i]==-.125f,"Stereo contents");
    check(c.request(),"New request");c.process(left,nullptr,512);c.prepare(96000);
    check(c.request(),"Sample rate change cancels incomplete capture");
    for(int i=0;i<128;++i)c.process(left,nullptr,512);
    check(c.take(l,r,sr)&&sr==96000&&l==r,"Mono duplicate / captured rate");
    check(c.request(),"Ready cancellation request");
    for(int i=0;i<128;++i)c.process(left,right,512);
    c.cancel();check(!c.take(l,r,sr),"Cancelled ready buffer must not be consumed");
    // Real concurrent audio/UI stress: validate every sample, not merely the state flag.
    c.prepare(48000);std::atomic<bool> done{false};
    std::thread audio([&] {
        float a[256],b[256];unsigned sample=0;
        while(!done.load()) {
            for(int i=0;i<256;++i){a[i]=float(sample++%(1u<<20));b[i]=-a[i];}
            c.process(a,b,256);
            std::this_thread::yield();
        }
    });
    int received=0;bool contents=true;
    auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(10);
    while(received<100&&std::chrono::steady_clock::now()<deadline) {
        c.cancel();c.request();
        while(!c.take(l,r,sr)&&std::chrono::steady_clock::now()<deadline)std::this_thread::yield();
        if(l.size()!=65536)break;
        for(size_t i=0;i<l.size();++i) {
            contents &= r[i]==-l[i];
            if(i) contents &= unsigned(l[i])==(unsigned(l[i-1])+1)%(1u<<20);
        }
        ++received;
    }
    done.store(true);audio.join();
    check(received==100&&contents,"Concurrent capture/restart sample integrity");
    std::cout<<"PASS capture cancel/restart, timeout recovery, mono/stereo, sample-rate reset, 100 concurrent captures\n";
 } catch(const std::exception &e) {std::cerr<<e.what()<<'\n';return 1;}
}
