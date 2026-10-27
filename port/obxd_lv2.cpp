// Independent headless LV2 adapter, GPL-3.0-or-later, 2026.
// Synth DSP and parameter dispatch are derived from 2DaT/Obxd (see source archive).
#include "compat.hpp"
#include "Engine/SynthEngine.h"
#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/midi/midi.h>
#include <lv2/time/time.h>
#include <array>
#include <new>

static constexpr int ids[]={
#include "param_ids.inc"
};
static constexpr float defaults[]={
#include "defaults.inc"
};
static constexpr size_t N=sizeof(ids)/sizeof(ids[0]);
struct Plugin {
 SynthEngine synth;
 const LV2_Atom_Sequence* events=nullptr;
 float* left=nullptr;float* right=nullptr;
 std::array<const float*,N> ports{};
 std::array<float,N> previous{};
 double rate=44100,beat=0,bpm=120,speed=0;
 LV2_URID midi=0,object=0,blank=0,position=0,tempo=0,barBeat=0,timeSpeed=0,atomFloat=0,atomDouble=0;
 void parameter(int index,float newValue){
#include "params.inc"
 }
 void init(double sr,LV2_URID_Map* map){
  rate=sr;synth.setSampleRate(sr);
  for(size_t i=0;i<N;i++){previous[i]=defaults[i];parameter(ids[i],defaults[i]);}
  synth.procPitchWheel(0);synth.procModWheel(0);synth.allSoundOff();
  midi=map->map(map->handle,LV2_MIDI__MidiEvent);
  object=map->map(map->handle,LV2_ATOM__Object);blank=map->map(map->handle,LV2_ATOM__Blank);
  atomFloat=map->map(map->handle,LV2_ATOM__Float);atomDouble=map->map(map->handle,LV2_ATOM__Double);
  position=map->map(map->handle,LV2_TIME__Position);tempo=map->map(map->handle,LV2_TIME__beatsPerMinute);
  barBeat=map->map(map->handle,LV2_TIME__barBeat);timeSpeed=map->map(map->handle,LV2_TIME__speed);
 }
 double number(const LV2_Atom* a,double fallback){
  if(!a)return fallback;
  double v=fallback;
  if(a->type==atomFloat && a->size>=sizeof(float)){float f;memcpy(&f,a+1,sizeof f);v=f;}
  if(a->type==atomDouble && a->size>=sizeof(double))memcpy(&v,a+1,sizeof v);
  return std::isfinite(v)?v:fallback;
 }
 void event(const LV2_Atom* a){
  if(a->type==midi){
   const uint8_t* d=(const uint8_t*)(a+1);
   if(a->size<3)return;
   switch(d[0]&0xf0){
    case 0x90:if(d[2])synth.procNoteOn(d[1]&127,(d[2]&127)/127.f);else synth.procNoteOff(d[1]&127);break;
    case 0x80:synth.procNoteOff(d[1]&127);break;
    case 0xe0:synth.procPitchWheel((((d[2]&127)<<7)+(d[1]&127)-8192)/8192.f);break;
    case 0xb0:
     if(d[1]==1)synth.procModWheel((d[2]&127)/127.f);
     if(d[1]==64){if(d[2]>=64)synth.sustainOn();else synth.sustainOff();}
     if(d[1]==120){synth.sustainOff();synth.allSoundOff();}
     if(d[1]==123){synth.sustainOff();synth.allNotesOff();}
     break;
   }
  }else if((a->type==object||a->type==blank)&&a->size>=sizeof(LV2_Atom_Object_Body)){
   const auto* o=(const LV2_Atom_Object*)a;
   if(o->body.otype!=position)return;
   const LV2_Atom *t=nullptr,*b=nullptr,*s=nullptr;
   lv2_atom_object_get(o,tempo,&t,barBeat,&b,timeSpeed,&s,0);
   bpm=std::clamp(number(t,bpm),1.0,1000.0);beat=number(b,beat);speed=number(s,speed);
   synth.setPlayHead(bpm,beat);
  }
 }
 void render(uint32_t start,uint32_t end){
  for(uint32_t f=start;f<end;f++){synth.processSample(left+f,right+f);}
  beat+=(end-start)*bpm/(60.0*rate)*speed;
 }
 void run(uint32_t frames){
  if(!left||!right)return;
  for(size_t i=0;i<N;i++){
   float v=ports[i]?*ports[i]:defaults[i];if(!std::isfinite(v))v=defaults[i];v=std::clamp(v,0.f,1.f);
   if(v!=previous[i]){parameter(ids[i],v);previous[i]=v;}
  }
  synth.setPlayHead(bpm,beat);
  uint32_t cursor=0;
  if(events && events->atom.size>=sizeof(LV2_Atom_Sequence_Body)){
   const auto* ev=lv2_atom_sequence_begin(&events->body);
   const auto* end=(const uint8_t*)(&events->body)+events->atom.size;
   while((const uint8_t*)ev+sizeof(LV2_Atom_Event)<=end){
    if(ev->body.size>uint32_t(end-(const uint8_t*)(ev+1)))break;
    const uint32_t at=uint32_t(std::clamp<int64_t>(ev->time.frames,cursor,frames));
    render(cursor,at);cursor=at;event(&ev->body);
    ev=lv2_atom_sequence_next(ev);
   }
  }
  render(cursor,frames);
 }
};
static LV2_Handle create(const LV2_Descriptor*,double rate,const char*,const LV2_Feature* const* features){
 if(!std::isfinite(rate)||rate<8000||rate>384000)return nullptr;
 LV2_URID_Map* map=nullptr;
 for(auto f=features;f&&*f;++f)if(!strcmp((*f)->URI,LV2_URID__map))map=(LV2_URID_Map*)(*f)->data;
 if(!map)return nullptr;
 void* mem=::operator new(sizeof(Plugin),std::nothrow);if(!mem)return nullptr;
 // Legacy engine has fields without explicit initializers: initialize storage
 // before construction, then apply every original sound parameter.
 memset(mem,0,sizeof(Plugin));auto* p=new(mem)Plugin;
 p->init(rate,map);return p;
}
static void connect(LV2_Handle h,uint32_t i,void* data){
 auto* p=(Plugin*)h;if(i==0)p->events=(const LV2_Atom_Sequence*)data;
 else if(i==1)p->left=(float*)data;else if(i==2)p->right=(float*)data;
 else if(i-3<N)p->ports[i-3]=(const float*)data;
}
static void activate(LV2_Handle h){((Plugin*)h)->synth.allSoundOff();}
static void run(LV2_Handle h,uint32_t n){((Plugin*)h)->run(n);}
static void deactivate(LV2_Handle h){((Plugin*)h)->synth.allSoundOff();}
static void cleanup(LV2_Handle h){auto* p=(Plugin*)h;p->~Plugin();::operator delete(p);}
static const LV2_Descriptor descriptor={"urn:machinista:ports:obxd-2dat",create,connect,activate,run,deactivate,cleanup,nullptr};
extern "C" LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index){return index?nullptr:&descriptor;}
