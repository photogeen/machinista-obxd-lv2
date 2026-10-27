#include <algorithm>
#include <cmath>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include "audio/lv2_host.hpp"
void require(bool b,const char* reason){if(!b)throw std::runtime_error(reason);}
int main(){try{
 maschine::LV2World world;
 std::cout<<"[";bool first=true;
 for(double rate:{44100.,48000.,96000.}){
  maschine::LV2Plugin p(world,"urn:machinista:ports:obxd-2dat",rate);
  std::array<float,256> l{},r{};
  auto peak=[&](){float v=0;for(int i=0;i<256;i++){require(std::isfinite(l[i])&&std::isfinite(r[i]),"non-finite audio");v=std::max(v,std::max(std::abs(l[i]),std::abs(r[i])));}return v;};
  for(int i=0;i<10;i++){p.run(l.data(),r.data(),nullptr,0);require(peak()<1e-6,"idle output not silent");}
  maschine::MidiEvent on{60,100};on.frame=128;p.run(l.data(),r.data(),&on,1);
  for(int i=0;i<128;i++)require(std::abs(l[i])<1e-6,"note started before its timestamp");
  float max=peak();double sum=0;unsigned count=0;auto start=std::chrono::steady_clock::now();
  for(int i=0;i<172;i++){
   p.run(l.data(),r.data(),nullptr,0);max=std::max(max,peak());
   for(float v:l){sum+=v*v;count++;}
  }
  require(max>1e-4&&max<2,"silent or excessive audio");
  maschine::MidiEvent off{60,0};p.run(l.data(),r.data(),&off,1);
  for(int i=0;i<172;i++)p.run(l.data(),r.data(),nullptr,0);
  require(peak()<1e-5,"note-off left a sounding voice");
  std::array<maschine::MidiEvent,8> chord{};
  for(int i=0;i<8;i++)chord[i]={48+i*2,100};
  p.run(l.data(),r.data(),chord.data(),chord.size());
  float chordPeak=0;
  for(int i=0;i<60;i++){p.run(l.data(),r.data(),nullptr,0);chordPeak=std::max(chordPeak,peak());}
  require(chordPeak>1e-4&&chordPeak<4,"bad polyphonic audio");
  maschine::MidiEvent panic{};panic.status=0xb0;panic.data1=120;panic.data2=0;
  p.run(l.data(),r.data(),&panic,1);for(int i=0;i<100;i++)p.run(l.data(),r.data(),nullptr,0);
  require(peak()<1e-5,"all-sound-off left a voice");
  // Verify control ports are live, rather than only present in metadata.
  auto energy=[&](float cutoff){
   p.desired[45]->store(cutoff);maschine::MidiEvent n{60,100};p.run(l.data(),r.data(),&n,1);
   double total=0;for(int i=0;i<100;i++){p.run(l.data(),r.data(),nullptr,0);peak();for(float v:l)total+=v*v;}
   p.run(l.data(),r.data(),&panic,1);for(int i=0;i<30;i++)p.run(l.data(),r.data(),nullptr,0);
   return total;
  };
  double closed=energy(0.05f),open=energy(0.9f);
  require(open>closed*2,"cutoff control has no audible effect");
  p.desired[3]->store(0);p.run(l.data(),r.data(),&on,1);
  for(int i=0;i<30;i++)p.run(l.data(),r.data(),nullptr,0);
  require(peak()<1e-5,"volume control does not mute");
  double seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
  if(!first)std::cout<<",";first=false;
  std::cout<<"{\"sampleRate\":"<<rate<<",\"blockSize\":256,\"peak\":"<<max<<",\"rms\":"<<std::sqrt(sum/count)<<",\"polyphonyPeak\":"<<chordPeak<<",\"renderSeconds\":"<<seconds<<",\"sampleAccurateNoteOn\":true,\"noteOff\":true,\"panic\":true,\"cutoffAndVolumeControls\":true,\"ports\":"<<p.controls.size()<<"}";
 }
 std::cout<<"]\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
