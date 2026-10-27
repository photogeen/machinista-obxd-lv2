	switch(index)
	{
	case SELF_OSC_PUSH:
		synth.processSelfOscPush(newValue);
		break;
	case PW_ENV_BOTH:
		synth.processPwEnvBoth(newValue);
		break;
	case PW_OSC2_OFS:
		synth.processPwOfs(newValue);
		break;
	case ENV_PITCH_BOTH:
		synth.processPitchModBoth(newValue);
		break;
	case FENV_INVERT:
		synth.processInvertFenv(newValue);
		break;
	case LEVEL_DIF:
		synth.processLoudnessDetune(newValue);
		break;
	case PW_ENV:
		synth.processPwEnv(newValue);
		break;
	case LFO_SYNC:
		synth.procLfoSync(newValue);
		break;
	case ECONOMY_MODE:
		synth.procEconomyMode(newValue);
		break;
	case VAMPENV:
		synth.procAmpVelocityAmount(newValue);
		break;
	case VFLTENV:
		synth.procFltVelocityAmount(newValue);
		break;
	case ASPLAYEDALLOCATION:
		synth.procAsPlayedAlloc(newValue);
		break;
	case BENDLFORATE:
		synth.procModWheelFrequency(newValue);
		break;
	case FOURPOLE:
		synth.processFourPole(newValue);
		break;
	case LEGATOMODE:
		synth.processLegatoMode(newValue);
		break;
	case ENVPITCH:
		synth.processEnvelopeToPitch(newValue);
		break;
	case OSCQuantize:
		synth.processPitchQuantization(newValue);
		break;
	case VOICE_COUNT:
		synth.setVoiceCount(newValue);
		break;
	case BANDPASS:
		synth.processBandpassSw(newValue);
		break;
	case FILTER_WARM:
		synth.processOversampling(newValue);
		break;
	case BENDOSC2:
		synth.procPitchWheelOsc2Only(newValue);
		break;
	case BENDRANGE:
		synth.procPitchWheelAmount(newValue);
		break;
	case NOISEMIX:
		synth.processNoiseMix(newValue);
		break;
	case OCTAVE:
		synth.processOctave(newValue);
		break;
	case TUNE:
		synth.processTune(newValue);
		break;
	case BRIGHTNESS:
		synth.processBrightness(newValue);
		break;
	case MULTIMODE:
		synth.processMultimode(newValue);
		break;
	case LFOFREQ:
		synth.processLfoFrequency(newValue);
		break;
	case LFO1AMT:
		synth.processLfoAmt1(newValue);
		break;
	case LFO2AMT:
		synth.processLfoAmt2(newValue);
		break;
	case LFOSINWAVE:
		synth.processLfoSine(newValue);
		break;
	case LFOSQUAREWAVE:
		synth.processLfoSquare(newValue);
		break;
	case LFOSHWAVE:
		synth.processLfoSH(newValue);
		break;
	case LFOFILTER:
		synth.processLfoFilter(newValue);
		break;
	case LFOOSC1:
		synth.processLfoOsc1(newValue);
		break;
	case LFOOSC2:
		synth.processLfoOsc2(newValue);
		break;
	case LFOPW1:
		synth.processLfoPw1(newValue);
		break;
	case LFOPW2:
		synth.processLfoPw2(newValue);
		break;
	case PORTADER:
		synth.processPortamentoDetune(newValue);
		break;
	case FILTERDER:
		synth.processFilterDetune(newValue);
		break;
	case ENVDER:
		synth.processEnvelopeDetune(newValue);
		break;
	case XMOD:
		synth.processOsc2Xmod(newValue);
		break;
	case OSC2HS:
		synth.processOsc2HardSync(newValue);
		break;
	case OSC2P:
		synth.processOsc2Pitch(newValue);
		break;
	case OSC1P:
		synth.processOsc1Pitch(newValue);
		break;
	case PORTAMENTO:
		synth.processPortamento(newValue);
		break;
	case UNISON:
		synth.processUnison(newValue);
		break;
	case FLT_KF:
		synth.processFilterKeyFollow(newValue);
		break;
	case OSC1MIX:
		synth.processOsc1Mix(newValue);
		break;
	case OSC2MIX:
		synth.processOsc2Mix(newValue);
		break;
	case PW:
		synth.processPulseWidth(newValue);
		break;
	case OSC1Saw:
		synth.processOsc1Saw(newValue);
		break;
	case OSC2Saw:
		synth.processOsc2Saw(newValue);
		break;
	case OSC1Pul:
		synth.processOsc1Pulse(newValue);
		break;
	case OSC2Pul:
		synth.processOsc2Pulse(newValue);
		break;
	case VOLUME:
		synth.processVolume(newValue);
		break;
	case UDET:
		synth.processDetune(newValue);
		break;
	case OSC2_DET:
		synth.processOsc2Det(newValue);
		break;
	case CUTOFF:
		synth.processCutoff(newValue);
		break;
	case RESONANCE:
		synth.processResonance(newValue);
		break;
	case ENVELOPE_AMT:
		synth.processFilterEnvelopeAmt(newValue);
		break;
	case LATK:
		synth.processLoudnessEnvelopeAttack(newValue);
		break;
	case LDEC:
		synth.processLoudnessEnvelopeDecay(newValue);
		break;
	case LSUS:
		synth.processLoudnessEnvelopeSustain(newValue);
		break;
	case LREL:
		synth.processLoudnessEnvelopeRelease(newValue);
		break;
	case FATK:
		synth.processFilterEnvelopeAttack(newValue);
		break;
	case FDEC:
		synth.processFilterEnvelopeDecay(newValue);
		break;
	case FSUS:
		synth.processFilterEnvelopeSustain(newValue);
		break;
	case FREL:
		synth.processFilterEnvelopeRelease(newValue);
		break;
	case PAN1:
		synth.processPan(newValue,1);
		break;
	case PAN2:
		synth.processPan(newValue,2);
		break;
	case PAN3:
		synth.processPan(newValue,3);
		break;
	case PAN4:
		synth.processPan(newValue,4);
		break;
	case PAN5:
		synth.processPan(newValue,5);
		break;
	case PAN6:
		synth.processPan(newValue,6);
		break;
	case PAN7:
		synth.processPan(newValue,7);
		break;
	case PAN8:
		synth.processPan(newValue,8);
		break;
	}