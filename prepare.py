from pathlib import Path
import re, shutil, json, subprocess
root=Path(__file__).resolve().parent
src=root/'source'
port=root/'port';port.mkdir(exist_ok=True)
shutil.copytree(src/'Source/Engine',port/'Engine',dirs_exist_ok=True)
p=port/'Engine/SynthEngine.h';p.write_bytes(p.read_bytes().replace(b'#include "../PluginProcessor.h"',b'#include "../compat.hpp"'))
cpp=(src/'Source/PluginProcessor.cpp').read_text(errors='replace')
switch=cpp[cpp.index('\tswitch(index)'):cpp.index('\n\t//DIRTY HACK')]
enum=(src/'Source/Engine/ParamsEnum.h').read_text(errors='replace')
names=re.findall(r'\b[A-Za-z_]\w*\b',enum.split('enum ObxdParameters')[1].split('{')[1].split('}')[0]);names=names[:names.index('PARAM_COUNT')]
params=[(i,n) for i,n in enumerate(names) if f'case {n}:' in switch]
defs=dict.fromkeys(names,0.0)
for k,v in re.findall(r'values\[(\w+)\]\s*=\s*([0-9.]+)',(src/'Source/Engine/Params.h').read_text(errors='replace')):defs[k]=float(v)
labels=dict(re.findall(r'case (\w+):\s*return S\("([^"]+)"\)',cpp))
(port/'compat.hpp').write_text('''#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <climits>
#include <cstring>
#include <string>
using String=std::string;
namespace juce { constexpr float float_Pi=3.14159265358979323846f; }
using juce::float_Pi;
template<class T> T jmin(T a,T b){return std::min(a,b);}
template<class T> T jlimit(T lo,T hi,T x){return std::clamp(x,lo,hi);}
inline int roundToInt(double x){return int(std::lrint(x));}
inline void zeromem(void* p,size_t n){std::memset(p,0,n);}
// Small independent pseudorandom utility for the engine's analog variation.
class Random {
 uint64_t state;
public:
 explicit Random(uint64_t seed=0x4f425844):state(seed?seed:1){}
 uint64_t nextInt64(){state^=state>>12;state^=state<<25;state^=state>>27;return state*2685821657736338717ULL;}
 float nextFloat(){return float(nextInt64()>>40)*(1.0f/16777216.0f);}
 static Random& getSystemRandom(){static thread_local Random r;return r;}
};
''')
(port/'params.inc').write_text(switch)
(port/'param_ids.inc').write_text(','.join(n for _,n in params))
(port/'defaults.inc').write_text(','.join(str(defs[n])+'f' for _,n in params))
uri='urn:machinista:ports:obxd-2dat'
bundle=port/'obxd.lv2';bundle.mkdir(exist_ok=True)
(bundle/'manifest.ttl').write_text(f'''@prefix lv2: <http://lv2plug.in/ns/lv2core#> .
@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .
<{uri}> a lv2:Plugin; lv2:binary <obxd.so>; rdfs:seeAlso <obxd.ttl> .
''')
ports=['''[a lv2:InputPort, atom:AtomPort; lv2:index 0; lv2:symbol "events"; lv2:name "MIDI / Time"; atom:bufferType atom:Sequence; atom:supports midi:MidiEvent, time:Position]''']
for i,n in [(1,'left'),(2,'right')]:ports.append(f'[a lv2:OutputPort, lv2:AudioPort; lv2:index {i}; lv2:symbol "{n}"; lv2:name "{n.title()} Output"]')
for i,(_,n) in enumerate(params,3):ports.append(f'[a lv2:InputPort, lv2:ControlPort; lv2:index {i}; lv2:symbol "{n.lower()}"; lv2:name {json.dumps(labels.get(n,n))}; lv2:minimum 0; lv2:maximum 1; lv2:default {defs[n]}]')
(bundle/'obxd.ttl').write_text('''@prefix lv2: <http://lv2plug.in/ns/lv2core#> .
@prefix atom: <http://lv2plug.in/ns/ext/atom#> .
@prefix midi: <http://lv2plug.in/ns/ext/midi#> .
@prefix time: <http://lv2plug.in/ns/ext/time#> .
@prefix urid: <http://lv2plug.in/ns/ext/urid#> .
@prefix doap: <http://usefulinc.com/ns/doap#> .
'''+f'<{uri}> a lv2:Plugin, lv2:InstrumentPlugin; doap:name "OB-Xd (2DaT LV2)"; doap:license <https://www.gnu.org/licenses/gpl-3.0.html>; lv2:requiredFeature urid:map; lv2:minorVersion 0; lv2:microVersion 1; lv2:port\n'+',\n'.join(ports)+' .\n')
(root/'parameters.json').write_text(json.dumps([{'index':i+3,'engineIndex':j,'symbol':n.lower(),'default':defs[n]} for i,(j,n) in enumerate(params)],indent=2))
print(len(params),'sound parameters',subprocess.check_output(['git','-C',str(src),'rev-parse','HEAD'],text=True).strip())
