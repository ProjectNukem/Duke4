/*=============================================================================
ALAudioSubsystem.cpp: Unreal OpenAL Audio interface object.
Copyright 2015 Oldunreal.

Revision history:
* Old initial version by Joseph I. Valenzuela (based on AudioSubsystem.cpp)
* Heavily modified and completely rewritten by Jochen 'Smirftsch' Görnitz
* Porting and porting help for other UEngine1 games, code cleanups, efx refactoring and lipsynch by Sebastian 'Hanfling' Kaufel
=============================================================================*/

/*------------------------------------------------------------------------------------
Audio includes.
------------------------------------------------------------------------------------*/

#ifdef _WIN32
#include <comutil.h>
#else
#include <pthread.h>
#include <errno.h>
#endif
#include "ALAudio.h"
#include "WaveFormat.h"
#include "OggSupport.h"
#define EFX

#ifdef EFX
#pragma warning(push)
#pragma warning(disable:4305) // double to float truncation
EFXEAXREVERBPROPERTIES g_ReverbPreset[] = {
	REVERB_PRESET_GENERIC,
	REVERB_PRESET_PADDEDCELL,
	REVERB_PRESET_ROOM,
	REVERB_PRESET_BATHROOM,
	REVERB_PRESET_LIVINGROOM,
	REVERB_PRESET_STONEROOM,
	REVERB_PRESET_AUDITORIUM,
	REVERB_PRESET_CONCERTHALL,
	REVERB_PRESET_CAVE,
	REVERB_PRESET_ARENA,
	REVERB_PRESET_HANGAR,
	REVERB_PRESET_CARPETTEDHALLWAY,
	REVERB_PRESET_HALLWAY,
	REVERB_PRESET_STONECORRIDOR,
	REVERB_PRESET_ALLEY,
	REVERB_PRESET_FOREST,
	REVERB_PRESET_CITY,
	REVERB_PRESET_MOUNTAINS,
	REVERB_PRESET_QUARRY,
	REVERB_PRESET_PLAIN,
	REVERB_PRESET_PARKINGLOT,
	REVERB_PRESET_SEWERPIPE,
	REVERB_PRESET_UNDERWATER,
	REVERB_PRESET_DRUGGED,
	REVERB_PRESET_DIZZY,
	REVERB_PRESET_PSYCHOTIC,
	REVERB_PRESET_CASTLE_SMALLROOM,
	REVERB_PRESET_CASTLE_SHORTPASSAGE,
	REVERB_PRESET_CASTLE_MEDIUMROOM,
	REVERB_PRESET_CASTLE_LONGPASSAGE,
	REVERB_PRESET_CASTLE_LARGEROOM,
	REVERB_PRESET_CASTLE_HALL,
	REVERB_PRESET_CASTLE_CUPBOARD,
	REVERB_PRESET_CASTLE_COURTYARD,
	REVERB_PRESET_CASTLE_ALCOVE,
	REVERB_PRESET_FACTORY_ALCOVE,
	REVERB_PRESET_FACTORY_SHORTPASSAGE,
	REVERB_PRESET_FACTORY_MEDIUMROOM,
	REVERB_PRESET_FACTORY_LONGPASSAGE,
	REVERB_PRESET_FACTORY_LARGEROOM,
	REVERB_PRESET_FACTORY_HALL,
	REVERB_PRESET_FACTORY_CUPBOARD,
	REVERB_PRESET_FACTORY_COURTYARD,
	REVERB_PRESET_FACTORY_SMALLROOM,
	REVERB_PRESET_ICEPALACE_ALCOVE,
	REVERB_PRESET_ICEPALACE_SHORTPASSAGE,
	REVERB_PRESET_ICEPALACE_MEDIUMROOM,
	REVERB_PRESET_ICEPALACE_LONGPASSAGE,
	REVERB_PRESET_ICEPALACE_LARGEROOM,
	REVERB_PRESET_ICEPALACE_HALL,
	REVERB_PRESET_ICEPALACE_CUPBOARD,
	REVERB_PRESET_ICEPALACE_COURTYARD,
	REVERB_PRESET_ICEPALACE_SMALLROOM,
	REVERB_PRESET_SPACESTATION_ALCOVE,
	REVERB_PRESET_SPACESTATION_MEDIUMROOM,
	REVERB_PRESET_SPACESTATION_SHORTPASSAGE,
	REVERB_PRESET_SPACESTATION_LONGPASSAGE,
	REVERB_PRESET_SPACESTATION_LARGEROOM,
	REVERB_PRESET_SPACESTATION_HALL,
	REVERB_PRESET_SPACESTATION_CUPBOARD,
	REVERB_PRESET_SPACESTATION_SMALLROOM,
	REVERB_PRESET_WOODEN_ALCOVE,
	REVERB_PRESET_WOODEN_SHORTPASSAGE,
	REVERB_PRESET_WOODEN_MEDIUMROOM,
	REVERB_PRESET_WOODEN_LONGPASSAGE,
	REVERB_PRESET_WOODEN_LARGEROOM,
	REVERB_PRESET_WOODEN_HALL,
	REVERB_PRESET_WOODEN_CUPBOARD,
	REVERB_PRESET_WOODEN_SMALLROOM,
	REVERB_PRESET_WOODEN_COURTYARD,
	REVERB_PRESET_SPORT_EMPTYSTADIUM,
	REVERB_PRESET_SPORT_SQUASHCOURT,
	REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL,
	REVERB_PRESET_SPORT_LARGESWIMMINGPOOL,
	REVERB_PRESET_SPORT_GYMNASIUM,
	REVERB_PRESET_SPORT_FULLSTADIUM,
	REVERB_PRESET_SPORT_STADIUMTANNOY,
	REVERB_PRESET_PREFAB_WORKSHOP,
	REVERB_PRESET_PREFAB_SCHOOLROOM,
	REVERB_PRESET_PREFAB_PRACTISEROOM,
	REVERB_PRESET_PREFAB_OUTHOUSE,
	REVERB_PRESET_PREFAB_CARAVAN,
	REVERB_PRESET_DOME_TOMB,
	REVERB_PRESET_PIPE_SMALL,
	REVERB_PRESET_DOME_SAINTPAULS,
	REVERB_PRESET_PIPE_LONGTHIN,
	REVERB_PRESET_PIPE_LARGE,
	REVERB_PRESET_PIPE_RESONANT,
	REVERB_PRESET_OUTDOORS_BACKYARD,
	REVERB_PRESET_OUTDOORS_ROLLINGPLAINS,
	REVERB_PRESET_OUTDOORS_DEEPCANYON,
	REVERB_PRESET_OUTDOORS_CREEK,
	REVERB_PRESET_OUTDOORS_VALLEY,
	REVERB_PRESET_MOOD_HEAVEN,
	REVERB_PRESET_MOOD_HELL,
	REVERB_PRESET_MOOD_MEMORY,
	REVERB_PRESET_DRIVING_COMMENTATOR,
	REVERB_PRESET_DRIVING_PITGARAGE,
	REVERB_PRESET_DRIVING_INCAR_RACER,
	REVERB_PRESET_DRIVING_INCAR_SPORTS,
	REVERB_PRESET_DRIVING_INCAR_LUXURY,
	REVERB_PRESET_DRIVING_FULLGRANDSTAND,
	REVERB_PRESET_DRIVING_EMPTYGRANDSTAND,
	REVERB_PRESET_DRIVING_TUNNEL,
	REVERB_PRESET_CITY_STREETS,
	REVERB_PRESET_CITY_SUBWAY,
	REVERB_PRESET_CITY_MUSEUM,
	REVERB_PRESET_CITY_LIBRARY,
	REVERB_PRESET_CITY_UNDERPASS,
	REVERB_PRESET_CITY_ABANDONED,
	REVERB_PRESET_DUSTYROOM,
	REVERB_PRESET_CHAPEL,
	REVERB_PRESET_SMALLWATERROOM,
	REVERB_PRESET_UNDERSLIME,
	REVERB_PRESET_NONE
};
#pragma warning(pop)

EFXEAXREVERBPROPERTIES *GetEFXReverb(DWORD enumVal)
{
	//range check
	if (enumVal >= ARRAY_COUNT(g_ReverbPreset))
		enumVal = ARRAY_COUNT(g_ReverbPreset) - 1; // Han: Use REVERB_PRESET_NONE now to be more consistent.
	return &g_ReverbPreset[enumVal];
}
#endif

//static const TCHAR *g_pSection = TEXT("ALAudio.ALAudioSubsystem");

/*------------------------------------------------------------------------------------
	Globals
------------------------------------------------------------------------------------*/
static FThreadSafeCounter DeviceChangeCounter = 0;

/*------------------------------------------------------------------------------------
Internals and helpers. han: maybe we should put them in class scope.
------------------------------------------------------------------------------------*/

static FLOAT DeltaTime = 0.f; //Ugly

bool sourceIsPlaying(ALuint sid)
{
	ALint state;
	alGetSourcei(sid, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}
void TransformCoordinates(ALfloat Dest[3], const FVector &Source)
{
	Dest[0] = Source.X;
	Dest[1] = Source.Y;
#if DNF
	Dest[2] = Source.Z;
#else
	Dest[2] = -Source.Z;
#endif
}

bool FillSingleBuffer(ALAudioMusicHandle *MusicHandle, char OggBufferData[MUSIC_BUFFER_SIZE])
{
	DWORD size = 0;
	INT   section;
	long result;

	while (size < MUSIC_BUFFER_SIZE)
	{
		result = ov_read(MusicHandle->OggStream, OggBufferData + size, MUSIC_BUFFER_SIZE - size, 0, 2, 1, &section);

		double CurrentTime = ov_time_tell(MusicHandle->OggStream);
		double TotalTime = ov_time_total(MusicHandle->OggStream, -1);
		//debugf(TEXT("%i %i"),CurrentTime,TotalTime);
		if (CurrentTime >= TotalTime) //simple loop.
			ov_time_seek(MusicHandle->OggStream, 0);

		if (result > 0)
			size += result;
		else break;
	}

	if (size == 0)
		return 0;

	return 1;
}
DWORD FillSingleSoundBuffer(OggVorbis_File *OggStream, char OggBufferData[SOUND_BUFFER_SIZE])
{
	DWORD size = 0;
	INT   section;
	long result;

	while (size < SOUND_BUFFER_SIZE)
	{
		result = ov_read(OggStream, OggBufferData + size, SOUND_BUFFER_SIZE - size, 0, 2, 1, &section);

		if (result > 0)
			size += result;
		else break;
	}

	return size;
}

// Prints aligned HelpLine assuming 25 chars is enough.
void PrintAlignedHelpLine(FOutputDevice& Ar, const TCHAR* Cmd, const TCHAR* Help = TEXT(""))
{
	Ar.Logf(TEXT("%25s - %s"), Cmd, Help);
}

// Calls UnrealScript Event.
UBOOL ProcessScript(UObject* Object, FName Event, void* Parms = NULL, UBOOL bChecked = 0)
{
	check(Object);
	UFunction* Function = bChecked ? Object->FindFunctionChecked(Event) : Object->FindFunction(Event);
	if (Function == NULL)
		return 0;
	Object->ProcessEvent(Function, Parms);
	return 1;
}

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(WIN32)
void TimerBegin()
{
	// This will increase the precision of the kernel interrupt
	// timer. Although this will slightly increase resource usage
	// this will also increase the precision of sleep calls and
	// this will in turn increase the stability of the framerate
	timeBeginPeriod(1);
}
void TimerEnd()
{
	// Restore the kernel timer config
	timeEndPeriod(1);
}
#endif

/*------------------------------------------------------------------------------------
	Runnables
------------------------------------------------------------------------------------*/

#if !RUNE_CLASSIC
class UpdateBufferRunnable : public FRunnable
{
public:
	UpdateBufferRunnable() = delete;
	UpdateBufferRunnable(ALAudioMusicHandle* InMusic)
		: MusicHandle(InMusic)
	{}
	UBOOL Init() override
	{
		return TRUE;
	}
	DWORD Run() override
	{
		if (!MusicHandle || !MusicHandle->IsPlaying || RunnableCancelled)
			return 0;

#if !PlayBuffer
		struct		xmp_frame_info uxmpfi;
#endif

		ALint processed = 0;

		//AudioLog(TEXT("ALAudio: Start UpdateBuffer for %s"),*MusicHandle->MusicTitle);
		while (!RunnableCancelled)
		{
			if (UALAudioSubsystem::EndBuffering || !MusicHandle->IsPlaying || MusicHandle->EndPlaying || MusicHandle->BufferError)
				break;

			alGetSourcei(MusicHandle->musicsource, AL_BUFFERS_PROCESSED, &processed);

			/*
			ALint qeued;
			alGetSourcei(MusicHandle->musicsource, AL_BUFFERS_QUEUED, &qeued);
			debugf(TEXT("AL_BUFFERS_PROCESSED %i"),processed);
			*/

			if (MusicHandle->IsOgg)
			{
				while (processed--)
				{
					char OggBufferData[MUSIC_BUFFER_SIZE];
					if (!FillSingleBuffer(MusicHandle, OggBufferData))
						MusicHandle->EndPlaying = TRUE;

					ALuint buffer = ~0;
					do
						alSourceUnqueueBuffers(MusicHandle->musicsource, 1, &buffer);
					// alGetError() != AL_NO_ERROR - can't be used, since error can be raised in different thread
					while (!MusicHandle->EndPlaying && !UALAudioSubsystem::EndBuffering && buffer == ~0);
					alBufferData(buffer, MusicHandle->format, OggBufferData, MUSIC_BUFFER_SIZE, (ALsizei)MusicHandle->vorbisInfo->rate);
					alSourceQueueBuffers(MusicHandle->musicsource, 1, &buffer);
					if (!sourceIsPlaying(MusicHandle->musicsource))
					{
						alSourcePlay(MusicHandle->musicsource);
						if (!sourceIsPlaying(MusicHandle->musicsource))
							MusicHandle->BufferError = AL_INVALID_VALUE;
					}

					if (MusicHandle->EndPlaying || UALAudioSubsystem::EndBuffering)
						break;
				}
			}
			else
			{
				while (processed--)
				{
#if PlayBuffer
					if (xmp_play_buffer(MusicHandle->xmpcontext, MusicHandle->xmpBuffer, MUSIC_BUFFER_SIZE, -1) != 0)
						MusicHandle->EndPlaying = TRUE;
#else
					xmp_play_frame(MusicHandle->xmpcontext);
					xmp_get_frame_info(MusicHandle->xmpcontext, &uxmpfi);
#endif
					ALuint buffer = ~0;
					do
						alSourceUnqueueBuffers(MusicHandle->musicsource, 1, &buffer);
					// alGetError() != AL_NO_ERROR - can't be used, since error can be raised in different thread
					while (!MusicHandle->EndPlaying && !UALAudioSubsystem::EndBuffering && buffer == ~0);
#if PlayBuffer
					alBufferData(buffer, MusicHandle->format, MusicHandle->xmpBuffer, MUSIC_BUFFER_SIZE, MusicHandle->SampleRate);
					//debugf(TEXT("AL buffer - processed: %d - error: %d - musicsource: %d - buffer: %d"), processed, MusicHandle->BufferError, MusicHandle->musicsource, buffer);
#else
					alBufferData(buffer, MusicHandle->format, uxmpfi.buffer, uxmpfi.buffer_size, MusicHandle->SampleRate);
#endif
					alSourceQueueBuffers(MusicHandle->musicsource, 1, &buffer);
					if (!sourceIsPlaying(MusicHandle->musicsource))
					{
						alSourcePlay(MusicHandle->musicsource);
						if (!sourceIsPlaying(MusicHandle->musicsource))
							MusicHandle->BufferError = AL_INVALID_VALUE;
					}

					if (MusicHandle->EndPlaying || UALAudioSubsystem::EndBuffering)
						break;
				}
			}
			appSleep(0.01f);
		}

		if (MusicHandle)
		{
			//AudioLog(NAME_DevMusic,(TEXT("ALAudio: End UpdateBuffer for %s"),*MusicHandle->MusicTitle));
			MusicHandle->IsPlaying = FALSE;
			alSourceStop(MusicHandle->musicsource);
		}
		UALAudioSubsystem::EndBuffering = FALSE;
		return 0;
	}
	UBOOL Stop() override
	{
		RunnableCancelled = TRUE;
		return TRUE;
	}
	UBOOL Exit() override
	{
		return TRUE;
	}
private:
	UBOOL RunnableCancelled = FALSE;
	ALAudioMusicHandle* MusicHandle = nullptr;
};
#endif //!RUNE_CLASSIC

class DeviceMonitoringRunnable : public FRunnable
{
public:
	UBOOL Init() override
	{
		return TRUE;
	}
	DWORD Run() override
	{
		auto DeviceSpecifier = alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == AL_TRUE ? ALC_ALL_DEVICES_SPECIFIER : ALC_DEVICE_SPECIFIER;
		
		while (!RunnableCancelled)
		{
			const char* DeviceName = alcGetString(NULL, DeviceSpecifier);

			if (!PreviousDeviceName || strcmp(PreviousDeviceName, DeviceName) != 0)
			{
				if (PreviousDeviceName)
					free(PreviousDeviceName);

				PreviousDeviceName = strdup(DeviceName);
				DeviceChangeCounter.Increment(1);
			}

			appSleep(1);
		}
		return 0;
	}
	UBOOL Stop() override
	{
		RunnableCancelled = TRUE;
		return TRUE;
	}
	UBOOL Exit() override
	{
		if (PreviousDeviceName)
			free(PreviousDeviceName);
		PreviousDeviceName = nullptr;
		return TRUE;
	}
private:
	char* PreviousDeviceName = nullptr;
	volatile BOOL RunnableCancelled = FALSE;
};

/*------------------------------------------------------------------------------------
UALAudioSubsystem.
------------------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UALAudioSubsystem);

#if DNF
URender* UALAudioSubsystem::__Render = NULL;
#else
URenderBase* UALAudioSubsystem::__Render = NULL;
#endif

UALAudioSubsystem::UALAudioSubsystem()
{
	guard(UALAudioSubsystem::UALAudioSubsystem);

	MusicFade = 1.0;
	CurrentCDTrack = 255;
	UpdateTime();
	OldAssignedZone = NULL;
	Initialized = 0;

	unguard;
};

/*------------------------------------------------------------------------------------
OpenAL device selection
------------------------------------------------------------------------------------*/

void UALAudioSubsystem::StaticConstructor(SC_PARAM)
{
	guard(UALAudioSubsystem::StaticConstructor);

	RegisterNames();

	// Generate frequencies enum.
	UEnum* OutputRates = new(SC_GETCLASS, TEXT("OutputRates"))UEnum(NULL);
	new(OutputRates->Names)FName(TEXT("8000Hz"));
	new(OutputRates->Names)FName(TEXT("11025Hz"));
	new(OutputRates->Names)FName(TEXT("16000Hz"));
	new(OutputRates->Names)FName(TEXT("22050Hz"));
	new(OutputRates->Names)FName(TEXT("32000Hz"));
	new(OutputRates->Names)FName(TEXT("44100Hz"));
	new(OutputRates->Names)FName(TEXT("48000Hz"));
	new(OutputRates->Names)FName(TEXT("96000Hz"));
	new(OutputRates->Names)FName(TEXT("192000Hz"));

#if !RUNE_CLASSIC
	// Enum for libxmp sampling rate.
	UEnum* SampleRates = new(SC_GETCLASS, TEXT("SampleRates"))UEnum(NULL);
	new(SampleRates->Names)FName(TEXT("8000Hz"));
	new(SampleRates->Names)FName(TEXT("11025Hz"));
	new(SampleRates->Names)FName(TEXT("16000Hz"));
	new(SampleRates->Names)FName(TEXT("22050Hz"));
	new(SampleRates->Names)FName(TEXT("32000Hz"));
	new(SampleRates->Names)FName(TEXT("44100Hz"));
	new(SampleRates->Names)FName(TEXT("48000Hz"));

	// Generate list of interpolation types for libxmp.
	UEnum* MusicInterpolationTypes = new(SC_GETCLASS, TEXT("MusicInterpolationTypes"))UEnum(NULL);
	new(MusicInterpolationTypes->Names)FName(TEXT("NEAREST")); // Nearest neighbor.
	new(MusicInterpolationTypes->Names)FName(TEXT("LINEAR")); // Linear.
	new(MusicInterpolationTypes->Names)FName(TEXT("SPLINE")); // Cubic spline.

	// Generate list of DSP Filtereffect for libxmp
	UEnum* MusicDspTypes = new(SC_GETCLASS, TEXT("DSP Filtereffect"))UEnum(NULL);
	new (MusicDspTypes->Names)FName(TEXT("DSP_LOWPASS"));
	new (MusicDspTypes->Names)FName(TEXT("DSP_ALL"));
#endif // !RUNE_CLASSIC

	// Device selection and enumeration
	new(SC_GETCLASS, TEXT("PreferredDevice"), RF_Public) UStrProperty(CPP_PROPERTY(PreferredDevice), TEXT("Audio"), CPF_Config);
	UArrayProperty* A = new(SC_GETCLASS, TEXT("DetectedDevices"), RF_Public) UArrayProperty(CPP_PROPERTY(DetectedDevices), TEXT("Audio"), CPF_Config);
	A->Inner = new(A, TEXT("StrProperty0"), RF_Public) UStrProperty;

	// General options.
#if !RUNE_CLASSIC
	new(SC_GETCLASS, TEXT("SampleRate"), RF_Public)UByteProperty(CPP_PROPERTY(SampleRateNum), TEXT("Audio"), CPF_Config, SampleRates); // Should be moved to XMP specific options.
#endif
	new(SC_GETCLASS, TEXT("SoundVolume"), RF_Public)UByteProperty(CPP_PROPERTY(SoundVolume), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("SpeechVolume"), RF_Public)UByteProperty(CPP_PROPERTY(SpeechVolume), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("MusicVolume"), RF_Public)UByteProperty(CPP_PROPERTY(MusicVolume), TEXT("Audio"), CPF_Config);
#if ENGINE_VERSION!=1100
	new(SC_GETCLASS, TEXT("UseSpeechVolume"), RF_Public)UBoolProperty(CPP_PROPERTY(UseSpeechVolume), TEXT("Audio"), CPF_Config);
#endif
	new(SC_GETCLASS, TEXT("EffectsChannels"), RF_Public)UIntProperty(CPP_PROPERTY(EffectsChannels), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("UseDigitalMusic"), RF_Public)UBoolProperty(CPP_PROPERTY(UseDigitalMusic), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("bSoundAttenuate"), RF_Public)UBoolProperty(CPP_PROPERTY(bSoundAttenuate), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("DopplerFactor"), RF_Public)UFloatProperty(CPP_PROPERTY(DopplerFactor), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("OutputRate"), RF_Public)UByteProperty(CPP_PROPERTY(OutputRateNum), TEXT("Audio"), CPF_Config, OutputRates);
	new(SC_GETCLASS, TEXT("UseAutoSampleRate"), RF_Public)UBoolProperty(CPP_PROPERTY(UseAutoSampleRate), TEXT("Audio"), CPF_Config);

	// Misc.
	new(SC_GETCLASS, TEXT("ProbeDevicesOnly"), RF_Public)UBoolProperty(CPP_PROPERTY(ProbeDevicesOnly), TEXT("Audio"), CPF_Config);

#if !RUNE_CLASSIC
	// XMP specific options.
	new(SC_GETCLASS, TEXT("MusicInterpolation"), RF_Public)UByteProperty(CPP_PROPERTY(MusicInterpolation), TEXT("Audio"), CPF_Config, MusicInterpolationTypes);
	new(SC_GETCLASS, TEXT("MusicDsp"), RF_Public)UByteProperty(CPP_PROPERTY(MusicDsp), TEXT("Audio"), CPF_Config, MusicDspTypes);
	new(SC_GETCLASS, TEXT("MusicPanSeparation"), RF_Public)UIntProperty(CPP_PROPERTY(MusicPanSeparation), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("MusicStereoMix"), RF_Public)UIntProperty(CPP_PROPERTY(MusicStereoMix), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("MusicStereoAngle"), RF_Public)UIntProperty(CPP_PROPERTY(MusicStereoAngle), TEXT("Audio"), CPF_Config);
#endif // !RUNE_CLASSIC
	new(SC_GETCLASS, TEXT("MusicAmplify"), RF_Public)UIntProperty(CPP_PROPERTY(MusicAmplify), TEXT("Audio"), CPF_Config);

	// EFX specific options.
#if defined(EFX)
	new(SC_GETCLASS, TEXT("EmulateOldReverb"), RF_Public)UBoolProperty(CPP_PROPERTY(EmulateOldReverb), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("OldReverbIntensity"), RF_Public)UFloatProperty(CPP_PROPERTY(OldReverbIntensity), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("UseReverb"), RF_Public)UBoolProperty(CPP_PROPERTY(UseReverb), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("ReverbIntensity"), RF_Public)UFloatProperty(CPP_PROPERTY(ReverbIntensity), TEXT("Audio"), CPF_Config);
#endif
	new(SC_GETCLASS, TEXT("DetailStats"), RF_Public)UBoolProperty(CPP_PROPERTY(DetailStats), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("ViewportVolumeIntensity"), RF_Public)UFloatProperty(CPP_PROPERTY(ViewportVolumeIntensity), TEXT("Audio"), CPF_Config);

	// HRTF specific options.
#if defined(HRTF)
	UEnum* HRTFRequest = new(SC_GETCLASS, TEXT("HRTFRequest"))UEnum(NULL);
	new(HRTFRequest->Names)FName(TEXT("Autodetect"));
	new(HRTFRequest->Names)FName(TEXT("Enable"));
	new(HRTFRequest->Names)FName(TEXT("Disable"));

	new(SC_GETCLASS, TEXT("UseHRTF"), RF_Public)UByteProperty(CPP_PROPERTY(UseHRTF), TEXT("Audio"), CPF_Config, HRTFRequest);
#endif

#if !defined(LEGACY_STATIC_CONSTRUCTOR)
	SetDefaultValues();
#endif

	unguard;
}

void UALAudioSubsystem::SetDefaultValues()
{
	// General options.
	OutputRateNum = 5; // 44100Hz
	SampleRateNum = 5; // 44100Hz
	SoundVolume = 100;
	SpeechVolume = 100;
	MusicVolume = 60;
	EffectsChannels = 64;
	DopplerFactor = 0.01f;
	bSoundAttenuate = 1;
	UseDigitalMusic = 1;
	UseSpeechVolume = 1;
	UseAutoSampleRate = 1;
	MusicStereoAngle = 30;

	//HRTF control.
	UseHRTF = 0;

#if !RUNE_CLASSIC
	// XMP specific options.
	MusicInterpolation = 2; // Cubic spline.
	MusicDsp = 1; // DSP_ALL.
	MusicPanSeparation = 50;
	MusicStereoMix = 70;
	MusicAmplify = 0; // Normal.
#endif

	// Misc.
	ProbeDevicesOnly = 0;

	// EFX specific options.
#if defined(EFX)
	// Drasticaly reduce old reverb intensity for dx.
#if ENGINE_VERSION==1100
	OldReverbIntensity = 0.2f;
#else
	OldReverbIntensity = 1.0f;
	ReverbIntensity = 1.0f;
#endif
	ViewportVolumeIntensity = 1.0f;
	UseReverb = 1;
	EmulateOldReverb = 1;
#endif
}

/*------------------------------------------------------------------------------------
FUnknown Interface.
------------------------------------------------------------------------------------*/

#if defined(USE_UNEXT)
DWORD STDCALL UALAudioSubsystem::QueryInterface(const FGuid& RefIID, void** InterfacePtr)
{
	if (RefIID == IID_IHumanHeadAudioSubsystem)
	{
		*(IHumanHeadAudioSubsystem**)InterfacePtr = this;
		return 1;
	}
	return 0;
}
#endif

/*------------------------------------------------------------------------------------
UObject Interface.
------------------------------------------------------------------------------------*/

void UALAudioSubsystem::PostEditChange()
{
	guard(UALAudioSubsystem::PostEditChange);

	// General options.
	OutputRateNum = Clamp(OutputRateNum, (BYTE)0, (BYTE)8);
	SampleRateNum = Clamp(SampleRateNum, (BYTE)0, (BYTE)6);
	MusicVolume = Clamp(MusicVolume, (BYTE)0, (BYTE)255);
	SoundVolume = Clamp(SoundVolume, (BYTE)0, (BYTE)255);
	SpeechVolume = Clamp(SpeechVolume, (BYTE)0, (BYTE)255);
	EffectsChannels = Clamp(EffectsChannels, 1, MAX_EFFECTS_CHANNELS);
	DopplerFactor = Clamp(DopplerFactor, 0.f, 10.f);

#if !RUNE_CLASSIC
	// XMP specific options.
	MusicDsp = Clamp(MusicDsp, (BYTE)0, (BYTE)3);
	MusicInterpolation = Clamp(MusicInterpolation, (BYTE)0, (BYTE)2);
	MusicPanSeparation = Clamp(MusicPanSeparation, 0, 100);
	MusicStereoMix = Clamp(MusicStereoMix, 0, 100);
#endif
	MusicAmplify = Clamp(MusicAmplify, 0, 3);

#if defined(EFX)
	// EFX specific options.
	OldReverbIntensity = Clamp(OldReverbIntensity, 0.f, 10.f);
	ReverbIntensity = Clamp(ReverbIntensity, 0.f, 10.f);

	// Make sure Effects Extension is loaded if someone enabled Reverb.
	if (!ConditionalLoadEffectsExtension())
		GEffectsExtensionLoaded = false;
#endif

	// Load filter
	if (!LoadFilterExtension())
	{
		GFilterExtensionLoaded = false;
		bSoundAttenuate = 0;
	}

	ViewportVolumeIntensity = Clamp(ViewportVolumeIntensity, 0.f, 5.f);

	MusicStereoAngle = Clamp(MusicStereoAngle, (INT) 0, (INT)360); // 360°

	Angles[0] = MusicStereoAngle * PI / 180;
	Angles[1] = -Angles[0];

	unguard;
}

/*------------------------------------------------------------------------------------
UAudioSubsystem Interface.
------------------------------------------------------------------------------------*/

void UALAudioSubsystem::InitAttrList(ALCint* AttrList)
{
	guard(UALAudioSubsystem::InitAttrList);
	// I want to get rid of this maintaining indices by hand. --han
#define ATTRLIST_FREQUENCY 1
#define ATTRLIST_REFRESH   3
#define ATTRLIST_HRTF      5
	
	// Set specifies.
	AttrList[0] = ALC_FREQUENCY;
	AttrList[2] = ALC_REFRESH;
#if defined (HRTF)
	AttrList[4] = ALC_HRTF_SOFT;
#endif

	// Set default values.
	AttrList[ATTRLIST_FREQUENCY] = 44100;
	AttrList[ATTRLIST_REFRESH] = 60;
#if defined (HRTF)
	AttrList[ATTRLIST_HRTF] = ALC_DONT_CARE_SOFT;
#endif

	// Set requested output rate for OpenAL.
	INT Rates[] = { 8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000, 192000 };
	AttrList[ATTRLIST_FREQUENCY] = OutputRate = Rates[OutputRateNum];
	OutputRate = Rates[OutputRateNum];

#if defined (HRTF)

	/*
	Specifying the value for ALC_HRTF_SOFT of ALC_FALSE will request no HRTF mixing. The default
	value of ALC_DONT_CARE_SOFT will allow the AL to determine for itself
	whether HRTF should be used or not (depending on the detected device port
	or form factor, format, etc).
	*/
	switch (UseHRTF)
	{
		// Force enable.
	case 1:
		AttrList[ATTRLIST_HRTF] = ALC_TRUE;
		AudioLog(NAME_Init, TEXT("ALAudio: Trying to enable HRTF extension"));
		break;

		// Force disable.
	case 2:
		AttrList[ATTRLIST_HRTF] = ALC_FALSE;
		break;

		// Let ALSoft do whatever it pleases.
	default:
		AttrList[ATTRLIST_HRTF] = ALC_DONT_CARE_SOFT;
		AudioLog(NAME_Init, TEXT("ALAudio: Trying to autodetect HRTF. Note: Autodetection may only work with USB headphones."));
		break;
	}
#endif
	unguard;
}


#if DNF
/*-----------------------------------------------------------------------------
	Music Subsystem.
-----------------------------------------------------------------------------*/
static bool	GetNextStream;

static INT Rates[] = { 8000,11025,16000,22050,32000,44100,48000 };

void UALAudioSubsystem::MusicPlay(TCHAR* LevelPrefix, TCHAR* Filename, UBOOL Instant, FLOAT CrossfadeTime, UBOOL Push)
{
	if (CrossfadeTime <= 0.0f)
		CrossfadeTime = 0.5f;

	// An empty filename is a signal to halt all playing streams.
	if (!Filename || !*Filename)
	{
		if (DigitalDriver)
		{
			if (CurrentStream) { AIL_close_stream(CurrentStream); CurrentStream = NULL; }
			if (PendingStream) { AIL_close_stream(PendingStream); PendingStream = NULL; }
			NextJumpPattern.Empty();
			NextJumpPattern.Push(FString(TEXT("")));
		}
		return;
	}

	GDnExec->Printf(TEXT("*** MusicPlay: Filename:%s"), Filename);

	// Determine if the level prefix has changed:
	TCHAR TempBuffer[256];
	appStrncpy(TempBuffer, LevelPrefix, ARRAY_COUNT(TempBuffer));
	if (appStrchr(TempBuffer, '?'))
		*(TCHAR*)appStrchr(TempBuffer, '?') = '\0';
	FString NewLevelPrefix = TempBuffer;

	if (CurrentLevelPrefix != NewLevelPrefix)
	{
		CurrentLevelPrefix = NewLevelPrefix;

		FString LevelFileMask = TEXT("..\\Music\\");
		LevelFileMask += CurrentLevelPrefix;
		LevelFileMask += TEXT("_*.mp3");
		LevelFilenames = GFileManager->FindFiles(*LevelFileMask, 1, 0);
	}

	// If I am setting instead of pushing, clear the stack:
	if (!Push)
		NextJumpPattern.Empty();

	// Push the new filename
	NextJumpPattern.Push(FString(Filename));

	// Set the crossfade time.
	CrossfadeLength = CrossfadeTime;
	bInstant = Instant;

	//if(!CurrentStream)
	//{
	MusicComputeCrossfadeStart();
	//}
}

void UALAudioSubsystem::MusicUpdate()
{
	if (!DigitalDriver) return;
	double currentTime = appSeconds();
	AIL_set_digital_master_volume(DigitalDriver, 127 * Clamp(MusicVolume / 255.0, 0.0, 1.0));

	static double RegisteredCrossfadeStart = 0.0;
	if ((currentTime >= CrossfadeStart) && (CrossfadeStart != RegisteredCrossfadeStart))
	{
		RegisteredCrossfadeStart = CrossfadeStart;

		FString Filename = TEXT("..\\Music\\");
		FString NextJumpPatternTOS;

		if (NextJumpPattern.Num() == 0) NextJumpPatternTOS = FString(TEXT(""));
		else if (NextJumpPattern.Num() == 1) NextJumpPatternTOS = NextJumpPattern.TOS();
		else if (NextJumpPattern.Num() > 1) NextJumpPatternTOS = NextJumpPattern.Pop();

		GetNextStream = false;

		int count = 0;

		for (int i = 0; i < LevelFilenames.Num(); i++)
			if (appStrstr((TCHAR*)appStrlwr((TCHAR*)*LevelFilenames(i)), appStrlwr((TCHAR*)*NextJumpPatternTOS)))
				count++;

		if (count)
		{
			int selected = appRand() % count;

			count = 0;
			for (i = 0; i < LevelFilenames.Num(); i++)
				if (appStrstr((TCHAR*)appStrlwr((TCHAR*)*LevelFilenames(i)), appStrlwr((TCHAR*)*NextJumpPatternTOS)))
				{
					if (count == selected) break;
					count++;
				}

			Filename += *LevelFilenames(i);
		}
		else
			Filename += NextJumpPatternTOS;	// Try using the pattern itself as the filename.

		GDnExec->Printf(TEXT("OpenNextMP3 NextJumpPattern:%s ChosenFilename:%s"), *NextJumpPatternTOS, *Filename);

		// Should I set up a pending stream to crossfade to?
		if (CurrentStream)
		{
			if (!PendingStream)
			{
				PendingStream = AIL_open_stream(DigitalDriver, TCHAR_TO_ANSI(*Filename), 0);
				AIL_set_stream_volume(PendingStream, 0);
				AIL_start_stream(PendingStream);
			}
		}
		// Or do I need to open up the initial stream: 
		else
		{
			if (CurrentStream) { AIL_close_stream(CurrentStream); CurrentStream = NULL; }

			if (!(CurrentStream = AIL_open_stream(DigitalDriver, TCHAR_TO_ANSI(*Filename), 0)))
			{
				GDnExec->Printf(TEXT("Failed to open stream: %s"), *Filename);
				return;
			}
			AIL_start_stream(CurrentStream);
		}
		return;
	}

	if (PendingStream)
	{
		// Has the crossfade completed?
		if (currentTime >= CrossfadeStart + CrossfadeLength)
		{
			AIL_set_stream_volume(CurrentStream, 0);
			AIL_set_stream_volume(PendingStream, 127);

			AIL_close_stream(CurrentStream);
			CurrentStream = PendingStream;
			PendingStream = NULL;
			CrossfadeLength = 0.2f;	// Default switch speed.
			bInstant = false;			// Turn off instant.
			MusicComputeCrossfadeStart();
			return;
		}

		// Adjust the volumes of both streams:
		double alpha = (currentTime - CrossfadeStart) / CrossfadeLength;
		AIL_set_stream_volume(CurrentStream, (S32)((1.0 - alpha) * 127));
		AIL_set_stream_volume(PendingStream, (S32)(alpha * 127));
	}
}

void UALAudioSubsystem::MusicComputeCrossfadeStart()
{
	long TotalMilliseconds, CurrentMilliseconds;
	if (!bInstant && CurrentStream)
	{
		AIL_stream_ms_position(CurrentStream, &TotalMilliseconds, &CurrentMilliseconds);

		FLOAT CurrentSeconds = CurrentMilliseconds / 1000.0f;
		FLOAT TotalSeconds = TotalMilliseconds / 1000.0f;

		CrossfadeStart = appSeconds() + (TotalSeconds - CurrentSeconds) - CrossfadeLength;
	}
	else
	{
		CrossfadeStart = appSeconds();
	}

	GDnExec->Printf(TEXT("Seconds: %f CrossfadeStart:%f"), appSeconds(), CrossfadeStart);
}
#endif

UBOOL UALAudioSubsystem::Init()
{
	guard(UALAudioSubsystem::Init);

	// Enumerate OpenAL devices
	guard(ProbeDevices);
	const ALCchar *deviceList = NULL;
	try
	{
		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == AL_TRUE)
			deviceList = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER); // No idea why it fails specifically here is using a library with invalid instruction set, but better catching it to show at least a proper error message.
		else
			deviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	}
	catch (...)
	{
		appErrorf(TEXT("Your CPU doesn't support whatever enhanced instruction set (SSE2, SSE3, SSE4, AVX, ...) was used to build the OpenAL library with. Try a different OpenAL32.dll/libopenal.so."));
	}

	DetectedDevices.Empty();
	INT i = 0;
	CurrentDevice = -1;
	while ( deviceList && *deviceList )
	{
#if DNF
		FString Device(appFromAnsi(deviceList));
#elif MACOSX
		FString Device(deviceList, STRENCODING_UTF8);
#else
		FString Device(deviceList);
#endif
		if (PreferredDevice == Device)
			CurrentDevice = i;
		DetectedDevices.AddItem(Device);
		deviceList += strlen(deviceList) + 1;
		i++;
	}
	SaveConfig();
	unguard;

	if (ProbeDevicesOnly)
		return false;

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(WIN32)
	TimerBegin();
#endif

	/*
	debugf(TEXT("UseReverb %i"), UseReverb);
	debugf(TEXT("OldReverbIntensity %f"), OldReverbIntensity);
	debugf(TEXT("EmulateOldReverb %i"), EmulateOldReverb);
	debugf(TEXT("MusicAmplify %i"), MusicAmplify);
	debugf(TEXT("ProbeDevicesOnly %i"), ProbeDevicesOnly);
	debugf(TEXT("MusicStereoMix %i"), MusicStereoMix);
	debugf(TEXT("MusicPanSeparation %i"), MusicPanSeparation);
	debugf(TEXT("MusicDsp %i"), MusicDsp);
	debugf(TEXT("MusicInterpolation %i"), MusicInterpolation);
	debugf(TEXT("DopplerFactor %f"), DopplerFactor);
	debugf(TEXT("bSoundAttenuate %i"), bSoundAttenuate);
	debugf(TEXT("UseDigitalMusic %i"), UseDigitalMusic);
	debugf(TEXT("EffectsChannels %i"), EffectsChannels);
	debugf(TEXT("UseSpeechVolume %i"), UseSpeechVolume);
	debugf(TEXT("MusicVolume %i"), MusicVolume);
	debugf(TEXT("SpeechVolume %i"), SpeechVolume);
	debugf(TEXT("SoundVolume %i"), SoundVolume);
	debugf(TEXT("SampleRate %i"), SampleRate);
	debugf(TEXT("OutputRate %i"), OutputRate);
	debugf(TEXT("ALDevice %i"), ALDevice);
	*/

	// Validate ini settings...
	PostEditChange();

	ALCint AttrList[32];
	appMemzero(AttrList, sizeof(AttrList));
	InitAttrList(&AttrList[0]);

#if !RUNE_CLASSIC
	// SampleRate for XMP
	INT XMPRates[] = { 8000, 11025, 16000, 22050, 32000, 44100, 48000 };
	SampleRate = XMPRates[SampleRateNum];
#endif

	// Stereo Angle
	Angles[0] = MusicStereoAngle * PI / 180;
	Angles[1] = -Angles[0];

	/*------------------------------------------------------------------------------------
	Start of OpenAL init
	------------------------------------------------------------------------------------*/

	// Initialize OpenAL Audio library.
	Device = nullptr;

	// Select preferred device.
	if ( PreferredDevice != TEXT("") )
	{
		AudioLog(NAME_Init, TEXT("ALAudio: Trying to use preferred device: %s"), *PreferredDevice);
		Device = alcOpenDevice( appToAnsi(*PreferredDevice) );
	}
	if ( !Device )
	{
		AudioLog(NAME_Init, TEXT("ALAudio: Trying to use default device"));
		Device = alcOpenDevice(NULL);
		
		if (Device)
		{ // start monitor change of default device
			DeviceMonitoringRunnable = new ::DeviceMonitoringRunnable();
			DeviceMonitoringThread = GThreadFactory->CreateThread(DeviceMonitoringRunnable, TRUE, TRUE);
		}
	}
	if ( !Device )
	{
		GWarn->Logf(TEXT("ALAudio: Failed to Initialize Open AL device (alcOpenDevice)"));
		return false;
	}

	AudioLog(NAME_Init, TEXT("ALAudio: We are using OpenAL device: %s (%s)"), appFromAnsi(alcGetString(Device, ALC_DEVICE_SPECIFIER)), appFromAnsi(alcGetString(Device, ALC_ALL_DEVICES_SPECIFIER)));	
	AudioLog(NAME_Init, TEXT("ALAudio: Supported extensions: %s"), appFromAnsi(alcGetString(Device, ALC_EXTENSIONS)));
	AudioLog(NAME_Init, TEXT("ALAudio: OutputRateNum %i(%i Hz) SampleRateNum %i (%i Hz)"), OutputRateNum, OutputRate, SampleRateNum, SampleRate);

	context_id = alcCreateContext(Device, AttrList);
	// context_id = alcCreateContext(Device,NULL); // for testing -  NULL is perfectly valid

	if (context_id == NULL) {
		GWarn->Logf(TEXT("ALAudio: Failed to create context (alcCreateContext)"));
		return false;
	}
	alcMakeContextCurrent(context_id);
	if (alcGetError(Device) != ALC_NO_ERROR)
	{
		GWarn->Logf(TEXT("ALAudio: Failed to make context current (alcMakeContextCurrent)"));
		alcDestroyContext(context_id);
		context_id = NULL;
		alcCloseDevice(Device);
		return false;
	}

	ALsizei Freq = ALC_FALSE;
	alcGetIntegerv(Device, ALC_FREQUENCY, 1, &Freq);
	if (OutputRate != Freq)
	{
		AudioLog(NAME_Init, TEXT("ALAudio: Can't set OutputRate of %i, using system (OS) defaults of %i.\r\n")
			TEXT("Some backends don't support setting custom rates for specific applications (MMDevAPI or ALSA without dmix).\r\n")
			TEXT("In this case setting something higher than what the system is outputting is unnecessary since it's just going to be downsampled for output anyway,\r\n")
			TEXT("and setting it lower is just going to make it get upsampled after OpenAL Soft has already applied its filters and effects.\r\n")
			TEXT("Adjust your system settings instead."), OutputRate, Freq);
		OutputRate = Freq;
	}

	ALsizei MonoSources = ALC_FALSE, StereoSources = ALC_FALSE;
	alcGetIntegerv(Device, ALC_MONO_SOURCES, 1, &MonoSources);
	alcGetIntegerv(Device, ALC_STEREO_SOURCES, 1, &StereoSources);
	AudioLog(NAME_Init, TEXT("ALAudio: Audio hardware supports %i mono (usually sound) and %i stereo (usually music) sources."), MonoSources, StereoSources);
	if (EffectsChannels > MonoSources)
	{
		AudioLog(NAME_Init, TEXT("ALAudio: Reducing EffectsChannels to %i sources."), MonoSources);
		EffectsChannels = MonoSources;
		// While providing usually 1 stereo source, hardware devices will use 2 voices for a stereo sound with 1 voice for mono.
		// OpenALSoft lets you play whatever as long as you have a source to play it.
	}

#if !RUNE_CLASSIC
	if (UseAutoSampleRate)
	{
		UBOOL Matched = 0;

		// Search for output rate in xmp sample rate array. This is probably the best thing to happen.
		for (INT i = 0; !Matched && i<ARRAY_COUNT(XMPRates); i++)
		{
			if (XMPRates[i] == OutputRate)
			{
				SampleRate = XMPRates[i];
				Matched = 1;
			}
		}

		// The current ALAudio sample rate is not supported by libxmp, so try finding a good match (e.g. 48000 to 96000).
		for (INT i = ARRAY_COUNT(XMPRates) - 1; !Matched && i >= 0; i--)
		{
			if ((OutputRate % XMPRates[i]) == 0)
			{
				SampleRate = XMPRates[i];
				Matched = 1;
			}
		}

		if (Matched)
			AudioLog(NAME_Init, TEXT("ALAudio: auto matched SampleRate (%i Hz) to OutputRate (%i Hz)."), SampleRate, OutputRate);
		else
			AudioLog(NAME_Init, TEXT("ALAudio: failed to automatch SampleRate to OutputRate (%i Hz)."), OutputRate);
	}
#endif

	//OpenALSoft specific extensions.
	if (alcIsExtensionPresent(Device, "ALC_SOFT_HRTF") != ALC_TRUE)
	{
		GWarn->Logf(TEXT("ALAudio: No OpenALSoft extension ALC_SOFT_HRTF present!"));
		GOpenALSOFT = false;
	}
	else
	{
		debugf(NAME_Init, TEXT("ALAudio: OpenALSoft extension ALC_SOFT_HRTF found."));
		GOpenALSOFT = true;
	}

#if defined (HRTF)
	ALsizei hrtf = ALC_HRTF_DISABLED_SOFT;
	if ( GOpenALSOFT )
		alcGetIntegerv( Device, ALC_HRTF_STATUS_SOFT, 1, &hrtf);

	switch (hrtf)
	{
	case ALC_HRTF_DISABLED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled."));
		break;
	case ALC_HRTF_ENABLED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is enabled."));
		break;
	case ALC_HRTF_DENIED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled because it's not allowed on the	device. This may be caused by invalid resource permissions, or other user configuration that disallows HRTF."));
		break;
	case ALC_HRTF_REQUIRED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is enabled because it must be used on the device. This may be caused by a device that can only use HRTF, or other user configuration (like alsoft.ini or .alsoftrc) that forces HRTF to be used."));
		break;
	case ALC_HRTF_HEADPHONES_DETECTED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is enabled because the device reported headphones."));
		break;
	case ALC_HRTF_UNSUPPORTED_FORMAT_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled because the device does not support it with the current	format. Typically this is caused by non - stereo output or an incompatible output frequency."));
		break;
	default:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled."));
	}
	AudioLog(NAME_DevAudio, TEXT("ALAudio: Initialized with rate %i"), OutputRate);
#endif

#ifdef ALURE
	/*
	if (alureInitDevice(NULL,Attrlist) == AL_FALSE)
	GWarn->Logf(TEXT("ALAudio: alureInitDevice failed %s"),appFromAnsi(alureGetErrorString()));
	else AudioLog(NAME_Init, TEXT("ALAudio: ALURE successfully initialized using %s."),appFromAnsi(alcGetString(Device, ALC_DEVICE_SPECIFIER)));
	*/
#else
	alutInitWithoutContext(NULL, NULL);
	if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
	{
		GWarn->Logf(TEXT("ALAudio: alutInitWithoutContext failed %s"), (char *)alutGetErrorString(error));
	}
#endif

	AudioLog(NAME_DevAudio, TEXT("ALAudio: Using OutputRate of %i Hz for audio output"), OutputRate);
#if !RUNE_CLASSIC
	AudioLog(NAME_DevAudio, TEXT("ALAudio: Using SampleRate of %i Hz for libxmp"), SampleRate);
#endif

	/*------------------------------------------------------------------------------------
	End of OpenAL init
	------------------------------------------------------------------------------------*/

	alGetError(); //reset error state.

	// Match OpenAL distance to games distance. See ALAudioBuild.h for derivation of DEFAULT_UNREALUNITS_PER_METER.
    alSpeedOfSound(343.3f / (1.f / DEFAULT_UNREALUNITS_PER_METER));
    if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: Couldn't set speed of sound with error: %s"), appFromAnsi(alGetString(error)));

	alListenerf(AL_METERS_PER_UNIT, 1.f / DEFAULT_UNREALUNITS_PER_METER);
	if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: Couldn't set distance units with error: %s"), appFromAnsi(alGetString(error)));

	// set orientation
	ALfloat zup[] = { 0.0, -1.0, 0.0, 0.0, 0.0, -1.0 };
	alListenerfv(AL_ORIENTATION, zup);
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

	// initialize transition
#if !RUNE_CLASSIC
	MTo = NULL;
	MFrom = NULL;
#endif
	TransitionStartTime = 0.f;
	MusicFadeInTime = 0.f;
	MusicFadeOutTime = 0.f;
	Transiting = FALSE;
	CrossFading = TRUE;

	// OGG Helpers
	callbacks.read_func = MEM_readOgg;
	callbacks.seek_func = MEM_seekOgg;
	callbacks.close_func = MEM_closeOgg;
	callbacks.tell_func = MEM_tellOgg;

#if DNF
	/* NJS: Initialize AIL while we're here: */
	if (UseDigitalMusic)
	{
		AIL_quick_startup(1, 0, OutputRate, 16, MusicStereoMix ? 2 : 1);
		AIL_quick_handles(&DigitalDriver, &MidiDriver, &DLSDevice);
	}
#endif

	// Initialized!
	USound::Audio = this;
#if !RUNE_CLASSIC
	UMusic::Audio = this;
#endif
	Initialized = 1;

#ifdef EFX
	// Make sure Effects Extension is loaded if someone enabled Reverb.
	if (!ConditionalLoadEffectsExtension())
		GEffectsExtensionLoaded = false;
#endif

	// Load filter
	if (!LoadFilterExtension())
	{
		GFilterExtensionLoaded = false;
		bSoundAttenuate = 0;
	}

	AudioLog(NAME_Init, TEXT("ALAudio: Version %s found"), appFromAnsi(alGetString(AL_VERSION)));
	AudioLog(NAME_Init, TEXT("ALAudio subsystem initialized."));

	// Set master volume carefully to avoid overmodulation.
	alListenerf(AL_GAIN, 1.0f);
    if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: Couldn't set AL_GAIN parameters with error: %s"), appFromAnsi(alGetString(error)));

		StopAllSound();

	//alListenerf(AL_GAIN,static_cast<ALfloat>(GIsEditor ? 255 : SoundVolume) / AUDIO_MAXVOLUME);

#if RUNE_CLASSIC
	IsRawStreaming = 0;
	RawStreamStartTime = -1.f;
#endif

	return true;
	unguard;
}

/*------------------------------------------------------------------------------------
ConditionalLoadEffectsExtension()

Loads OpenAL Effects Extension if
- Initialized set to true. (returns false otherwise)
- Reverb is enabled. (returns true otherwise)
- Not already loaded. (returns true otherwise)
- Device is not NULL. (crashes otherwise)

Returns true if loading was successful. false otherwise.
------------------------------------------------------------------------------------*/

UBOOL UALAudioSubsystem::LoadFilterExtension()
{
	// Load Filter Object Management Functions.
#if !AL_ALEXT_PROTOTYPES
	alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
	alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
	alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
	if (alGenFilters == NULL || alDeleteFilters == NULL || alIsFilter == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Filter Object Management Functions."));
		return 0;
	}

	// Load Filter Object Property Functions.
	alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
	alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
	alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
	alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
	if (alFilteri == NULL || alFilteriv == NULL || alFilterf == NULL || alFilterfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Filter Object Property Functions."));
		return 0;
	}

	// Load Filter Object Query Property Functions.
	alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
	alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
	alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
	alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
	if (alGetFilteri == NULL || alGetFilteriv == NULL || alGetFilterf == NULL || alGetFilterfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Filter Object Query Property Functions."));
		return 0;
	}
#endif
	GFilterExtensionLoaded = true;
	return 1;
}

#ifdef EFX
UBOOL UALAudioSubsystem::ConditionalLoadEffectsExtension()
{
	// Don't premature run this code.
	if (!Initialized || !UseReverb)
		return 0;

	// Check for need to load the extension.
	if ( GEffectsExtensionLoaded )
		return 1;


	// TODO: Cleanup init code.
	check(Device);

	// initialize iEffectSlot.
	iEffectSlot = 0;

	// Check if effects extension is available.
	if (alcIsExtensionPresent(Device, (ALCchar*)ALC_EXT_EFX_NAME) != ALC_TRUE)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Reverb effects not available due to lack of OpenAL Effects Extension. Try updating Soundcard drivers and OpenAL."));
		return 0;
	}

	// Query version number of effects extension.
	ALint alc_efx_major_version, alc_efx_minor_version;

	alcGetError(Device);
	alcGetIntegerv(Device, ALC_EFX_MAJOR_VERSION, 1, &alc_efx_major_version);
	alcGetIntegerv(Device, ALC_EFX_MINOR_VERSION, 1, &alc_efx_minor_version);

	if (alcGetError(Device) != ALC_NO_ERROR)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to query OpenAL Effects Extension version. Try updating Soundcard drivers and OpenAL."));
		return 0;
	}

	debugf(NAME_Init, TEXT("OpenAL Effects extension version %i.%i found."), alc_efx_major_version, alc_efx_minor_version);

	// This should *really* never happen.
	if (alc_efx_major_version < 1)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("OpenAL Effects extension major version is less than 1. Explosion imminent."));
		return 0;
	}

	// Load Auxiliary Effect Slot Object Management Functions.
#if !AL_ALEXT_PROTOTYPES
	alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
	alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
	alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
	if (alGenAuxiliaryEffectSlots == NULL || alDeleteAuxiliaryEffectSlots == NULL || alIsAuxiliaryEffectSlot == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Slot Object Management Functions."));
		return 0;
	}

	// Load Auxiliary Effect Slot Object Property Functions.
	alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
	alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
	alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
	alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
	if (alAuxiliaryEffectSloti == NULL || alAuxiliaryEffectSlotiv == NULL || alAuxiliaryEffectSlotf == NULL || alAuxiliaryEffectSlotfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Slot Object Property Functions."));
		return 0;
	}

	// Load Auxiliary Effect Slot Object Query Property Functions.
	alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
	alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
	alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
	alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");
	if (alGetAuxiliaryEffectSloti == NULL || alGetAuxiliaryEffectSlotiv == NULL || alGetAuxiliaryEffectSlotf == NULL || alGetAuxiliaryEffectSlotfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Slot Object Query Property Functions."));
		return 0;
	}

	// Load Effect Object Management Functions.
	alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
	alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
	alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
	if (alGenEffects == NULL || alDeleteEffects == NULL || alIsEffect == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Object Management Functions."));
		return 0;
	}

	// Load Effect Object Property Functions.
	alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
	alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
	alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
	alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
	if (alEffecti == NULL || alEffectiv == NULL || alEffectf == NULL || alEffectfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Object Property Functions."));
		return 0;
	}

	// Load Effect Object Query Property Functions.
	alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
	alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
	alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
	alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
	if (alGetEffecti == NULL || alGetEffectiv == NULL || alGetEffectf == NULL || alGetEffectfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Object Query Property Functions."));
		return 0;
	}
#endif
	// Create auxiliary effect slot.
	alGetError();
	alGenAuxiliaryEffectSlots(1, &iEffectSlot);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		GWarn->Logf(TEXT("ALAudio: alGenAuxiliary create error: %s"), appFromAnsi(alGetString(error)));
		return 0;
	}
	else
		AudioLog(NAME_Init, TEXT("ALAudio: EFX initialized."));

	debugf(NAME_DevAudio, TEXT("Successfully loaded OpenAL Effects Extension functions."));

	// Everything's fine.
	GEffectsExtensionLoaded = true;
	return 1;
}
#endif

void UALAudioSubsystem::SetViewport(UViewport* InViewport)
{
	SetViewport(InViewport, CompatibilityFlags);
}

void UALAudioSubsystem::SetViewport(UViewport* InViewport, DWORD InCompatibilityFlags)
{
	guard(UALAudioSubsystem::SetViewport);
	UBOOL Changed = InViewport != Viewport;
#if RUNE_CLASSIC
	AudioLog(NAME_DevAudio, TEXT("ALAudio: SetViewport(InViewport=%s) -- START"), *FObjectFullName(InViewport));
#endif
	//debugf(TEXT("SetViewport!"));

#if UNREAL_TOURNAMENT_OLDUNREAL
	CompatibilityFlags = InCompatibilityFlags;
#endif

	OldAssignedZone = NULL;
	// Stop playing sounds.
	guard(StopSounds);
	for (INT i = 0; i<EffectsChannels; i++)
		StopSound(i);
	unguard;

	if (Viewport)
	{
#if !RUNE_CLASSIC
		guard(SetViewportStopMusic);
		if (Changed || !GIsEditor)
		{
			for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
				if (MusicIt->Handle)
					StopMusic(*MusicIt);
			MFrom = NULL;
			MTo = NULL;
		}
		unguard;
#endif
		Transiting = FALSE;
		guard(SetViewportUnregisterSounds);
		if (Changed)
			for (TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt)
				if (SoundIt->Handle)
					UnregisterSound(*SoundIt);
		unguard;
	}

	Viewport = InViewport;
	if (Viewport)
	{
		// Determine startup parameters.
		guard(ModifyViewportActor);
		// ALAudio once again wrote in random memory. --han
		//check(Viewport->Actor);
		if (Changed || !GIsEditor)
			if (Viewport->Actor && Viewport->Actor->Song && Viewport->Actor->Transition == MTRAN_None)
				Viewport->Actor->Transition = MTRAN_Instant;
		unguard;

		guard(SetViewportRegisterSounds);
		for (TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt)
			if (!SoundIt->Handle)
				RegisterSound(*SoundIt);
		unguard;
	}
#if RUNE_CLASSIC
	AudioLog(NAME_DevAudio, TEXT("ALAudio: SetViewport() -- END"));
#endif
	unguard;
}

UViewport* UALAudioSubsystem::GetViewport()
{
	return Viewport;
}

#if !RUNE_CLASSIC
void UALAudioSubsystem::RegisterMusic(UMusic* Music)
{
	guard(UALAudioSubsystem::RegisterMusic);
	AudioLog(NAME_DevMusic, TEXT("RegisterMusic %s"), *FObjectFullName(Music));

	if (!Music->Handle)
	{
		//debugf(TEXT("RegisterMusic !Music->Handle %s"),Music->GetName());
		alGetError(); // clear possible previous errors (just to be certain)
		ALAudioMusicHandle* MusicHandle = new ALAudioMusicHandle();
		Music->Data.Load();
		MusicHandle->IsOgg = IsOggFormat(&Music->Data(0), Music->Data.Num());
		if (!Music->Data.Num() || Music->Data.Num() <= 512)
		{
			GWarn->Logf(TEXT("ALAudio: Bad music length (%i bytes) on %s"), Music->Data.Num(), *FObjectFullName(Music));
			Music->Data.Unload();
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->SongSection = 255; //ensure to not load it again.
			Viewport->Actor->Song = NULL; // I mean, really sure to not load it again!
			Flush();// and clean up behind you!
			if (MusicHandle->IsOgg && MusicHandle->OggStream)
				delete MusicHandle->OggStream;
			if (MusicHandle)
				delete MusicHandle;

			return;
		}
		if (MusicHandle->IsOgg)
		{
			MusicHandle->OggStream = new OggVorbis_File;
			//debugf(TEXT("Playing OGG Stream %s"),Music->GetName());

			MusicHandle->MemOggFile.filePtr = new BYTE[Music->Data.Num()];
			appMemcpy(MusicHandle->MemOggFile.filePtr, &Music->Data(0), Music->Data.Num());
			MusicHandle->MemOggFile.curPtr = MusicHandle->MemOggFile.filePtr;
			MusicHandle->MemOggFile.fileSize = Music->Data.Num();
			INT Result = ov_open_callbacks((void *)&MusicHandle->MemOggFile, MusicHandle->OggStream, NULL, -1, callbacks);
			if (Result < 0)
			{
				GWarn->Logf(TEXT("ALAudio: OGG load module from memory error in %s"), *FObjectFullName(Music));
				Music->Data.Unload();
				check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
				Viewport->Actor->SongSection = 255; //ensure to not load it again.
				Viewport->Actor->Song = NULL; // I mean, really sure to not load it again!
				Flush();// and clean up behind you!
				if (MusicHandle->MemOggFile.filePtr)
					delete MusicHandle->MemOggFile.filePtr;
				if (MusicHandle->OggStream)
					delete MusicHandle->OggStream;
				if (MusicHandle)
					delete MusicHandle;
				return;
			}
		}
		else
		{
			MusicHandle->xmpcontext = xmp_create_context();
			xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_DEFPAN, MusicPanSeparation); //must be set before loading the module!
			if (xmp_load_module_from_memory(MusicHandle->xmpcontext, &(Music->Data(0)), Music->Data.Num()) < 0)
			{
				GWarn->Logf(TEXT("ALAudio: xmp_load_module_from_memory error in %s"), *FObjectFullName(Music));
				GWarn->Logf(TEXT("ALAudio: check track format!"));
				Music->Data.Unload();
				check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
				Viewport->Actor->SongSection = 255; //ensure to not load it again.
				Viewport->Actor->Song = NULL; // I mean, really sure to not load it again!
				Flush();// and clean up behind you!
				xmp_free_context(MusicHandle->xmpcontext);
				if (MusicHandle)
					delete MusicHandle;
				return;
			}
			MusicHandle->xmpBuffer = appMalloc(MUSIC_BUFFER_SIZE, TEXT("libxmp buffer"));
			if (!MusicHandle->xmpBuffer)
			{
				GWarn->Logf(TEXT("ALAudio: can't allocate Music buffer %s"), appFromAnsi(alGetString(error)));
				Music->Data.Unload();
				xmp_free_context(MusicHandle->xmpcontext);
				if (MusicHandle)
					delete MusicHandle;
				return;
			}
		}
		Music->Data.Unload();
		alGenBuffers(MUSIC_BUFFERS, MusicHandle->musicbuffers);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: alGenBuffers for music failed with: %s"), appFromAnsi(alGetString(error)));
			if (MusicHandle->IsOgg)
			{
				if (MusicHandle->MemOggFile.filePtr)
					delete MusicHandle->MemOggFile.filePtr;
				if (MusicHandle->OggStream)
					delete MusicHandle->OggStream;
			}
			else
			{
				if (MusicHandle->xmpBuffer)
					appFree(MusicHandle->xmpBuffer);
				xmp_free_context(MusicHandle->xmpcontext);
			}
			if (MusicHandle)
				delete MusicHandle;
			return;
		}

		alGenSources(1, &MusicHandle->musicsource);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: alGenSources failed %s"), appFromAnsi(alGetString(error)));
			if (MusicHandle->IsOgg)
			{
				if (MusicHandle->MemOggFile.filePtr)
					delete MusicHandle->MemOggFile.filePtr;
				if (MusicHandle->OggStream)
					delete MusicHandle->OggStream;
			}
			else
			{
				if (MusicHandle->xmpBuffer)
					appFree(MusicHandle->xmpBuffer);
				xmp_free_context(MusicHandle->xmpcontext);
			}
			if (MusicHandle)
				delete MusicHandle;
			return;
		}
		//debugf(TEXT("Setting musicvolume %f"),((FLOAT)MusicVolume/255));
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: Setting musicvolume error: %s"), appFromAnsi(alGetString(error)));
		}

		// Data into Handle
		if (MusicHandle->IsOgg)
		{
			MusicHandle->vorbisInfo = ov_info(MusicHandle->OggStream, -1);
			MusicHandle->vorbisComment = ov_comment(MusicHandle->OggStream, -1);

			MusicHandle->MusicTitle = Music->GetName();
			for (int i = 0; i < MusicHandle->vorbisComment->comments; i++)
			{
				MusicHandle->MusicTitle += TEXT(" ");
				MusicHandle->MusicTitle += appFromAnsi(MusicHandle->vorbisComment->user_comments[i]);
			}

			MusicHandle->MusicType = FString::Printf(TEXT("OGG Version:%i Channels:%i Rate:%i"), MusicHandle->vorbisInfo->version, MusicHandle->vorbisInfo->channels, MusicHandle->vorbisInfo->rate);
		}
		else
		{
			xmp_get_module_info(MusicHandle->xmpcontext, &xmpmi);
			MusicHandle->MusicType = appFromAnsi(xmpmi.mod->type);
			MusicHandle->MusicTitle = Music->GetName();
			MusicHandle->MusicTitle += TEXT(" ");
			MusicHandle->MusicTitle += appFromAnsi(xmpmi.mod->name);
		}
		Music->Handle = MusicHandle;
	}
	else
		AudioLog(NAME_DevMusic, TEXT("RegisterMusic already existing handle for %s"), Music->GetName());
	unguard;
}
#endif

#if !RUNE_CLASSIC
void UALAudioSubsystem::UnregisterMusic(UMusic* Music)
{
	guard(UALAudioSubsystem::UnregisterMusic);
	StopMusic(Music);
	if (MFrom == Music)
		MFrom = NULL;
	if (MTo == Music)
		MTo = NULL;
	unguard;
}

void UALAudioSubsystem::StopMusic(UMusic* Music)
{
	guard(UALAudioSubsystem::StopMusic);

	if (Music && Music->Handle)
	{
		ALAudioMusicHandle* MusicHandle = (ALAudioMusicHandle*)Music->Handle;

		AudioLog(NAME_DevMusic, TEXT("ALAudio: StopMusic: %s"), *FObjectFullName(Music));
		MusicHandle->EndPlaying = 1;
		alSourceStop(MusicHandle->musicsource);

		if (MusicHandle->BufferingThread)
		{
			MusicHandle->BufferingThread->Kill(TRUE, 100);
			GThreadFactory->Destroy(MusicHandle->BufferingThread);
			MusicHandle->BufferingThread = nullptr;
			MusicHandle->BufferingRunnable = nullptr;
		}

        if (MusicHandle->musicsource)
            alDeleteSources(1, &MusicHandle->musicsource);

        if (MusicHandle->IsOgg)
		{
			ov_clear(MusicHandle->OggStream);
			if ((error = alGetError()) != AL_NO_ERROR)
				GWarn->Logf(NAME_DevMusic, TEXT("ALAudio: OGG StopMusic alDeleteSources error: %s"), appFromAnsi(alGetString(error)));
			alDeleteBuffers(MUSIC_BUFFERS, MusicHandle->musicbuffers);
			if ((error = alGetError()) != AL_NO_ERROR)
				GWarn->Logf(NAME_DevMusic, TEXT("ALAudio: OGG StopMusic alDeleteBuffers error: %s"), appFromAnsi(alGetString(error)));
			delete MusicHandle->MemOggFile.filePtr;
			delete MusicHandle->OggStream;
		}
		else
		{
		    //alSourcei(MusicHandle->musicsource, AL_BUFFER, NULL);
			appFree(MusicHandle->xmpBuffer);
			xmp_end_player(MusicHandle->xmpcontext);
			xmp_release_module(MusicHandle->xmpcontext);
			xmp_free_context(MusicHandle->xmpcontext);

			if ((error = alGetError()) != AL_NO_ERROR)
				GWarn->Logf(NAME_DevMusic, TEXT("ALAudio: StopMusic alDeleteSources error: %s"), appFromAnsi(alGetString(error)));
			alDeleteBuffers(MUSIC_BUFFERS, MusicHandle->musicbuffers);
			if ((error = alGetError()) != AL_NO_ERROR)
				GWarn->Logf(NAME_DevMusic, TEXT("ALAudio: StopMusic alDeleteBuffers error: %s"), appFromAnsi(alGetString(error)));
		}
		Music->Handle = NULL;
		delete MusicHandle;
	}
	unguard;
}
#endif

#if !RUNE_CLASSIC
UBOOL UALAudioSubsystem::StartMusic(ALAudioMusicHandle* MusicHandle)
{
	guard(UALAudioSubsystem::StartMusic);
	debugf(TEXT("StartMusic %s MusicHandle->MusicBuffer %i,MUSIC_BUFFER_SIZE %i), IsPlaying %i"),*MusicHandle->MusicTitle,sizeof(MusicHandle->musicbuffers),MUSIC_BUFFER_SIZE, MusicHandle->IsPlaying);
	alGetError(); // clear possible previous errors (just to be certain)

	if (MusicHandle->IsOgg)
	{
		MusicHandle->IsPlaying = TRUE; //avoid being called again unless done or failed.
		INT i = 0;

		MusicHandle->vorbisInfo = ov_info(MusicHandle->OggStream, -1);
		MusicHandle->vorbisComment = ov_comment(MusicHandle->OggStream, -1);

		AudioLog(TEXT("ALAudio: playing %s"), *MusicHandle->MusicTitle);

		if (MusicHandle->vorbisInfo->channels == 1)
			MusicHandle->format = AL_FORMAT_MONO16;
		else
			MusicHandle->format = AL_FORMAT_STEREO16;

		/* Fill OpenAL buffers */
		for (i = 0; i < MUSIC_BUFFERS; i++)
		{
			char OggBufferData[MUSIC_BUFFER_SIZE];
			if (!FillSingleBuffer(MusicHandle, OggBufferData))
			{
				GWarn->Logf(TEXT("ALAudio: OGG Buffer Num:%i FillSingleBuffer failed"), i);
				Flush();
				check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
				Viewport->Actor->Song = NULL;
				MusicHandle->IsPlaying = FALSE;
				return 0;
			}

			alBufferData(MusicHandle->musicbuffers[i], MusicHandle->format, (ALvoid*)OggBufferData, MUSIC_BUFFER_SIZE, (ALsizei)MusicHandle->vorbisInfo->rate);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				//GWarn->Logf(TEXT("ALAudio: OGG Buffer Num:%i alBufferData failed %s"),i,appFromAnsi(alGetString(error)));
				continue;
			}
		}

		alSourceQueueBuffers(MusicHandle->musicsource, MUSIC_BUFFERS, MusicHandle->musicbuffers);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: OGG alSourceQueueBuffers failed %s"), appFromAnsi(alGetString(error)));
			Flush();
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->Song = NULL;
			MusicHandle->IsPlaying = FALSE;
			return 0;
		}

		alSourcePlay(MusicHandle->musicsource);
		if (alGetError() != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: OGG alSourcePlay failed %s"), appFromAnsi(alGetString(error)));
			Flush();
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->Song = NULL;
			MusicHandle->IsPlaying = FALSE;
			return 0;
		}
	}
	else if (xmp_start_player(MusicHandle->xmpcontext, SampleRate, 0) == 0)
	{
		MusicHandle->IsPlaying = TRUE; //avoid being called again unless done or failed.
		MusicHandle->format = AL_FORMAT_STEREO16;
		MusicHandle->SampleRate = SampleRate;
		if (MusicHandle->SongSection != 0)
			xmp_set_position(MusicHandle->xmpcontext, MusicHandle->SongSection);

		//debugf(TEXT("Initial Music Volume: %i"),xmp_get_player(MusicHandle->xmpcontext, XMP_PLAYER_VOLUME));

		xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_AMP, MusicAmplify);
		xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_MIX, MusicStereoMix);
		xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_INTERP, MusicInterpolation);

		//debugf(TEXT("MusicDsp:           %i"),MusicDsp);
		//debugf(TEXT("MusicAmplify:       %i"),MusicAmplify);
		//debugf(TEXT("MusicInterpolation: %i"),MusicInterpolation);

		if (MusicDsp == 0)
			xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_DSP, XMP_DSP_LOWPASS);
		else
			xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_DSP, XMP_DSP_ALL);

		//xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_VOLUME, 200);//arbitrary value, must be between 0 and 200

		AudioLog(TEXT("ALAudio: playing %s (%s)"), *MusicHandle->MusicTitle, *MusicHandle->MusicType);

		// Fill OpenAL buffers
		for (INT i = 0; i < MUSIC_BUFFERS; i++)
		{
#if PlayBuffer
			if (xmp_play_buffer(MusicHandle->xmpcontext, MusicHandle->xmpBuffer, MUSIC_BUFFER_SIZE, 1) != 0)
#else
			if (xmp_play_frame(MusicHandle->xmpcontext) != 0)
#endif
			{
				GWarn->Logf(TEXT("ALAudio: xmp_play_buffer error"));
				Flush();
				check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
				Viewport->Actor->Song = NULL;
				MusicHandle->IsPlaying = FALSE;
				return 0;
			}

#if PlayBuffer
			alBufferData(MusicHandle->musicbuffers[i], MusicHandle->format, MusicHandle->xmpBuffer, MUSIC_BUFFER_SIZE, MusicHandle->SampleRate);
#else
			xmp_get_frame_info(MusicHandle->xmpcontext, &xmpfi);
			alBufferData(MusicHandle->musicbuffers[i], MusicHandle->format, xmpfi.buffer, xmpfi.buffer_size, MusicHandle->SampleRate);
			//debugf(TEXT("Buffering %i %3d/%3d %3d/%3d with size %i"),i, xmpfi.pos, xmpmi.mod->len, xmpfi.row, xmpfi.num_rows,xmpfi.buffer_size);
#endif

			if ((error = alGetError()) != AL_NO_ERROR)
			{
				//GWarn->Logf(TEXT("ALAudio: libxmp Buffer Num:%i alBufferData failed %s"),i,appFromAnsi(alGetString(error)));
				continue;
			}
		}
		alSourceQueueBuffers(MusicHandle->musicsource, MUSIC_BUFFERS, MusicHandle->musicbuffers);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: libxmp alSourceQueueBuffers failed %s"), appFromAnsi(alGetString(error)));
			Flush();
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->Song = NULL;
			MusicHandle->IsPlaying = FALSE;
			return 0;
		}
		alSourcePlay(MusicHandle->musicsource);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: libxmp alSourcePlay failed %s"), appFromAnsi(alGetString(error)));
			Flush();
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->Song = NULL;
			MusicHandle->IsPlaying = FALSE;
			return 0;
		}
	}
	else
	{
		GWarn->Logf(TEXT("ALAudio: xmp_start_player failed to play %s"), *MusicHandle->MusicTitle);
		return 0;
	}

	MusicHandle->BufferingRunnable = new ::UpdateBufferRunnable(MusicHandle);
	MusicHandle->BufferingThread = GThreadFactory->CreateThread(MusicHandle->BufferingRunnable, FALSE, TRUE);
	if (!MusicHandle->BufferingThread)
	{
		// Some error occured.
		GWarn->Logf(TEXT("ALAudio: Failed to create a valid audio thread."));
		return 0;
	}

	// Music volume
	alSource3f(MusicHandle->musicsource, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(MusicHandle->musicsource, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSource3f(MusicHandle->musicsource, AL_DIRECTION, 0.0, 0.0, 0.0);
	alSourcef(MusicHandle->musicsource, AL_ROLLOFF_FACTOR, 0.0);
	alSourcei(MusicHandle->musicsource, AL_SOURCE_RELATIVE, AL_FALSE);
	alSourcef(MusicHandle->musicsource, AL_GAIN, (GIsEditor ? 1.f : ((FLOAT)MusicVolume / 255)));
	alSourcef(MusicHandle->musicsource, AL_MAX_GAIN, 1.f);
	alSourcefv(MusicHandle->musicsource, AL_STEREO_ANGLES, Angles); // The AL_STEREO_ANGLES tag can be used with alSourcefv() and two angles. The angles are specified anticlockwise relative to the real front, so a normal 60degree front stage is specified with alSourcefv(sid,AL_STEREO_ANGLES,+M_PI/6,-M_PI/6).

	return 1;

	unguard;
}
#endif

void UALAudioSubsystem::RegisterSound(USound* Sound)
{
	guard(UALAudioSubsystem::RegisterSound);
	checkSlow(Sound);

	if (Sound && !Sound->Handle)
	{
		ALenum  format = AL_FORMAT_STEREO16; //preset.

		// Set the handle to avoid reentrance.
		Sound->Handle = (void*)-1;

		// Load the data.
		Sound->Data.Load();

		// Han: Might be a good idea to check if anything was actually loaded instead of assuming that it was.
		if (Sound->Data.Num() <= 0)
		{
			debugf(NAME_DevAudio, TEXT("Can't register sound %s, because it has no Data."), *FObjectFullName(Sound));

			// Just in case (well, is this even safe to do in this case?).
			Sound->Data.Unload();

			Sound->Handle = NULL;
			return;
		}

		ALuint buffer = 0;
		ALuint filter = 0;
		UBOOL IsOgg = 0;
		alGetError(); //clear error buffer.
		alGenBuffers(1, &buffer);

		if ( GFilterExtensionLoaded )
			alGenFilters(1, &filter);

		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: alGenBuffers for sound %ls failed with: %ls"), *FObjectFullName(Sound), appFromAnsi(alGetString(error)));
			Sound->Data.Unload();
			Sound->Handle = NULL;
			if (alIsBuffer(buffer))
				alDeleteBuffers(1, &buffer);
			if (alIsFilter(filter))
				alDeleteFilters(1, &filter);
			return;
		}

		IsOgg = IsOggFormat(&Sound->Data(0), Sound->Data.Num());
		if (IsOgg)
		{
			//appMsgf(TEXT("IsOgg"));
			OggVorbis_File* OggStream;
			OggStream = new OggVorbis_File;
			memory_ogg_file MemOggFile;

			MemOggFile.filePtr = new BYTE[Sound->Data.Num()];
			appMemcpy(MemOggFile.filePtr, &Sound->Data(0), Sound->Data.Num());
			MemOggFile.curPtr = MemOggFile.filePtr;
			MemOggFile.fileSize = Sound->Data.Num();
			INT Result = ov_open_callbacks((void *)&MemOggFile, OggStream, NULL, -1, callbacks);
			if (Result < 0)
			{
				GWarn->Logf(TEXT("ALAudio: OGG sound %ls load from memory error"), *FObjectFullName(Sound));
				Sound->Data.Unload();
				Sound->Handle = NULL;
				delete MemOggFile.filePtr;
				delete OggStream;
				if (alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);
				if (alIsFilter(filter))
					alDeleteFilters(1, &filter);
				return;
			}
			vorbis_info*    vorbisInfo;
			vorbisInfo = ov_info(OggStream, -1);

			vorbis_comment* vorbisComment;
			vorbisComment = ov_comment(OggStream, -1);

			/*
			FString MusicType,MusicTitle=TEXT("");
			for(int i = 0; i < vorbisComment->comments; i++)
			MusicTitle += FString::Printf(TEXT(" %s"),appFromAnsi(vorbisComment->user_comments[i]));

			MusicType = FString::Printf(TEXT("OGG Version:%i Channels:%i Rate:%i"),vorbisInfo->version,vorbisInfo->channels,vorbisInfo->rate);
			appMsgf(TEXT("%s %s"),*MusicTitle, *MusicType);
			*/
			if (vorbisInfo->channels == 1)
				format = AL_FORMAT_MONO16;
			else
				format = AL_FORMAT_STEREO16;

			char OggBufferData[SOUND_BUFFER_SIZE];
			FSoundData Data(Sound);
			INT DecodedSize = 0;
			DWORD ReadSize = 0;
			while ((ReadSize = FillSingleSoundBuffer(OggStream, OggBufferData)))
			{
				Data.Add(ReadSize);
				appMemcpy(&Data(DecodedSize), OggBufferData, ReadSize);
				DecodedSize += ReadSize;
			}
			delete MemOggFile.filePtr;
			delete OggStream;
			//appMsgf(TEXT("DecodedSize %i Sound->Data.Num( %i %f"),DecodedSize, Sound->Data.Num(), freq);
			alBufferData(buffer, format, &Data(0), Data.Num(), (ALsizei)vorbisInfo->rate);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: OGG sound %ls alBufferData failed %ls"), *FObjectFullName(Sound), appFromAnsi(alGetString(error)));
				Sound->Data.Unload();
				Sound->Handle = NULL;
				if (alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);
				if (alIsFilter(filter))
					alDeleteFilters(1, &filter);
				return;
			}
		}
		else
		{
#ifdef ALURE
			if (alureBufferDataFromMemory(&Sound->Data(0), Sound->Data.Num(), buffer) == AL_FALSE)
			{
				GWarn->Logf(TEXT("ALAudio: alureBufferDataFromMemory for sound %ls failed: %ls"), *FObjectFullName(Sound), appFromAnsi(alureGetErrorString()));

				/*
				alureStream *stream;
				stream = alureCreateStreamFromMemory(&Sound->Data(0),Sound->Data.Num(), 250000, 3, NULL);
				if(!stream)
				GWarn->Logf(TEXT("ALAudio: alureCreateStreamFromMemory for sound %ls failed: %ls"), *FObjectFullName(Sound), appFromAnsi(alureGetErrorString()));
				*/

				Sound->Data.Unload();
				Sound->Handle = NULL;
				if (alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);
				if (alIsFilter(filter))
					alDeleteFilters(1, &filter);
				return;
			}
#else
			ALvoid	*sounddata;
			ALfloat freq;
			sounddata = alutLoadMemoryFromFileImage(&Sound->Data(0), Sound->Data.Num(), &format, &size, &freq);
			if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: alutLoadMemoryFromFileImage for sound %ls failed %ls"), *FObjectFullName(Sound), appFromAnsi(alutGetErrorString(error)));
				if (error == ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA)
				{
					Sound->Handle = NULL;
					if (alIsBuffer(buffer))
						alDeleteBuffers(1, &buffer);
					if (alIsFilter)
						alDeleteFilters(1, &filter);
					return;
				}
				// FIXME! Hack to get wav files which are unsupported by ALUT played anyway.
				// may cause problems with sound output, but seems to work.
				alBufferData(buffer, format, &Sound->Data(0), Sound->Data.Num(), (ALsizei)freq);
				if ((error = alGetError()) != AL_NO_ERROR)
				{
					GWarn->Logf(TEXT("ALAudio: alBufferData for sound %ls failed %ls"), *FObjectFullName(Sound), appFromAnsi(alGetString(error)));
					Sound->Handle = NULL;
					if (alIsBuffer(buffer))
						alDeleteBuffers(1, &buffer);
					if (alIsFilter)
						alDeleteFilters(1, &filter);
					return;
				}
			}
			else
			{
				alBufferData(buffer, format, sounddata, size, (ALsizei)freq);
				if ((error = alGetError()) != AL_NO_ERROR)
				{
					GWarn->Logf(TEXT("ALAudio: alBufferData for sound %ls failed %ls"), *FObjectFullName(Sound), appFromAnsi(alGetString(error)));
					Sound->Handle = NULL;
					if (alIsBuffer(buffer))
						alDeleteBuffers(1, &buffer);
					if (alIsFilter)
						alDeleteFilters(1, &filter);
					return;
				}
			}
#endif
		}

		if (buffer == AL_NONE)
		{
			GWarn->Logf(TEXT("ALAudio: Invalid sound format in %ls"), *FObjectFullName(Sound));
			Sound->Data.Unload();
			Sound->Handle = NULL;
			if (alIsBuffer(buffer))
				alDeleteBuffers(1, &buffer);
			if (alIsFilter(filter))
				alDeleteFilters(1, &filter);
			return;
		}

		// Data into handle
		ALAudioSoundHandle* AudioHandle = new ALAudioSoundHandle();
		AudioHandle->ID = buffer;
		AudioHandle->Filter = filter;
		//debugf(TEXT("Sound %s"),Sound->GetName());

		if (!IsOgg && GetSampleLoop(&Sound->Data(0), &Sound->Data(Sound->Data.Num() - 1), &AudioHandle->LoopStart, &AudioHandle->LoopEnd))
			AudioHandle->bLoopingSample = 1;

		// Get sound rate and duration
		ALint bufferSize, frequency, bitsPerSample, channels;
		alGetBufferi(buffer, AL_SIZE, &bufferSize);
		alGetBufferi(buffer, AL_FREQUENCY, &frequency);
		alGetBufferi(buffer, AL_CHANNELS, &channels);
		alGetBufferi(buffer, AL_BITS, &bitsPerSample);
		AudioHandle->Rate = Max( (FLOAT)(frequency*channels*(bitsPerSample/8)), 1.f); //Prevent inf
		AudioHandle->Duration = (FLOAT)bufferSize/AudioHandle->Rate;

		Sound->Handle = AudioHandle;

		// Unload the data.
		Sound->Data.Unload();
	}
	unguard;
}

void UALAudioSubsystem::UnregisterSound(USound* Sound)
{
	guard(UALAudioSubsystem::UnregisterSound);
	check(Sound);
	if (Sound->Handle)
	{
		ALAudioSoundHandle* AudioHandle = (ALAudioSoundHandle*)Sound->Handle;

		AudioLog(NAME_DevSound, TEXT("ALAudio: Unregister sound: %s"), *FObjectFullName(Sound));

		// Stop this sound.
		for (INT i = 0; i<EffectsChannels; i++)
			if (PlayingSounds[i].Sound == Sound)
				StopSound(i);

		if (AudioHandle)
		{
		  alDeleteBuffers(1, &AudioHandle->ID);
		    if (AudioHandle->Filter)
		      alDeleteFilters(1, &AudioHandle->Filter);
		      delete AudioHandle;
		}
		Sound->Handle = NULL;
	}
	unguard;
}

void UALAudioSubsystem::Flush()
{
	guard(UALAudioSubsystem::Flush);
	AudioLog(TEXT("ALAudio: Flushing channels"));

#if !RUNE_CLASSIC
	for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
		if (MusicIt->Handle)
			StopMusic(*MusicIt);
#endif

	for (TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt)
		if (SoundIt->Handle)
			UnregisterSound(*SoundIt);

	if (Viewport && Viewport->Actor)
	{
		// Determine startup parameters.
		if (Viewport->Actor->Song && Viewport->Actor->Transition == MTRAN_None)
			Viewport->Actor->Transition = MTRAN_Instant;
	}
	unguard;
}

// TODO: Check which debugf/AudioLog should be Ar.Logf().
UBOOL UALAudioSubsystem::Exec(const TCHAR* Cmd, FOutputDevice& Ar)
{
	guard(UALAudioSubsystem::Exec);
	const TCHAR* Str = Cmd;
	int order = 0;
#if !RUNE_CLASSIC
	UMusic* Song = ((!Viewport || !Viewport->Actor) ? NULL : Viewport->Actor->Song);
	UMusic* Music = NULL;
#endif
	ALAudioMusicHandle *MusicHandle = NULL;
	//debugf( NAME_DevMusic, TEXT("executing: %s"), Str);

	if (ParseCommand(&Str, TEXT("AudioHelp")))
	{
		PrintAlignedHelpLine(Ar, TEXT("ALAudioSubsystem commands are:"));
		PrintAlignedHelpLine(Ar, TEXT("AStat Audio/Detail"), TEXT("Render on screen audio status."));
		PrintAlignedHelpLine(Ar, TEXT("MusicOrder <0-255>"), TEXT("Change music section(IT/S3M/MIDI/etc. only), 255 = stop music"));
		PrintAlignedHelpLine(Ar, TEXT("AudioFlush"), TEXT("Flush music and soundchannels."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicOffset"), TEXT("Return currently playing music offset (in S)."));
		PrintAlignedHelpLine(Ar, TEXT("SetMusicOffset <Time>"), TEXT("Change currently playing music play offset time (in S)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicLen <MusicName>"), TEXT("Returns length of selected song (in S)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicType"), TEXT("Returns the type of song (STREAM/TRACKER)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicInfo <MusicName>"), TEXT("Returns format info about the song (OGG/IT/S3M/MIDI/etc)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicTitle <MusicName>"), TEXT("Returns the Title of a song."));
		PrintAlignedHelpLine(Ar, TEXT("MusicStop"), TEXT("Stops currently playing music."));
		PrintAlignedHelpLine(Ar, TEXT("MusicStart- Starts previously stopped music."));
		PrintAlignedHelpLine(Ar, TEXT("MusicRestart"), TEXT("Restarts currently playing music."));
		PrintAlignedHelpLine(Ar, TEXT("MusicRestore"), TEXT("Restores music from level."));
		PrintAlignedHelpLine(Ar, TEXT("MaxEffectsChannels"), TEXT("Returns number of maximum supported effect channels."));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("AudioFlush")))
	{
		Flush();
		return 1;
	}
	if (ParseCommand(&Str, TEXT("ASTAT")))
	{
		if (ParseCommand(&Str, TEXT("Audio")))
		{
			AudioStats ^= 1;
			if (AudioStats)
				Ar.Logf(TEXT("ALAudio: Audio stats enabled."));
			else Ar.Logf(TEXT("ALAudio: Audio stats disabled."));
			return 1;
		}
		if (ParseCommand(&Str, TEXT("Detail")))
		{
			DetailStats ^= 1;
			if (DetailStats)
			{
				AudioStats = 1;
				Ar.Logf(TEXT("ALAudio: Audio detail stats enabled."));
			}
			else
				Ar.Logf(TEXT("ALAudio: Audio detail stats disabled."));
			return 1;
		}
		if (ParseCommand(&Str, TEXT("Effects")))
		{
			EffectsStats ^= 1;
			if (EffectsStats)
			{
				AudioStats = 1;
				Ar.Logf(TEXT("ALAudio: Audio effects stats enabled."));
			}
			else
				Ar.Logf(TEXT("ALAudio: Audio effects stats disabled."));
			return 1;
		}
	}
#if !RUNE_CLASSIC
	if (Parse(Str, TEXT("MusicOrder"), order))
	{
		if (!Song || !Song->Handle)
			return 1;
		MusicHandle = (ALAudioMusicHandle*)Song->Handle;

		if (order == 255)
		{
			Ar.Logf(TEXT("ALAudio: Changing music order to %i (stop)."), order);
			check(Viewport);
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->SongSection = 255;
			Viewport->Actor->Transition = MTRAN_FastFade;
			return 1;
		}
		else
		{
			if (!MusicHandle->IsPlaying)
			{
				check(Viewport);
				check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
				Viewport->Actor->SongSection = order;
				Viewport->Actor->Transition = MTRAN_Instant;
				return 1;
			}
			if (MusicHandle->IsOgg)
			{
				Ar.Logf(TEXT("ALAudio: Not possible in OGG Music"));
			}
			else
			{
				Ar.Logf(TEXT("ALAudio: Changing music order to %i."), order);
				xmp_set_position(MusicHandle->xmpcontext, order);
			}
		}
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicOffset")))
	{
		Music = (appStrlen(Str) <= 1 ? Song : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music->Handle)
			{
				Ar.Logf(TEXT("-1"));
				return 1;
			}
		}
		MusicHandle = (ALAudioMusicHandle*)Music->Handle;

		INT Result = 0;
		if (MusicHandle->IsOgg)
		{
			Result = ov_time_tell(MusicHandle->OggStream);
		}
		else
		{
			if (MusicHandle->IsPlaying)
			{
				xmp_get_frame_info(MusicHandle->xmpcontext, &xmpfi);
				Result = xmpfi.time / 1000;
			}
		}
		Ar.Logf(TEXT("%i"), Result);
		return 1;
	}
	if (Parse(Str, TEXT("SetMusicOffset"), order))
	{
		if (!Song)
			return 1;
		if (!Song->Handle)
		{
			RegisterMusic(Song);
			if (!Song || !Song->Handle)
				return 1;
		}
		MusicHandle = (ALAudioMusicHandle*)Song->Handle;
		if (MusicHandle->IsOgg)
		{
			ov_time_seek(MusicHandle->OggStream, order);
		}
		else
		{
			INT Result = order * 1000;
			xmp_seek_time(MusicHandle->xmpcontext, Result);
		}
		Ar.Logf(TEXT("ALAudio: SetMusicOffset to %i."), order);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicLen")))
	{
		Music = (appStrlen(Str) <= 1 ? Song : FindObject<UMusic>(ANY_PACKAGE, Str));
		INT Result = -1;
		if (!Music)
		{
			Ar.Logf(TEXT("%i"), Result);
			return 1;
		}
		if (!Music->Handle)
		{
			Ar.Logf(TEXT("%i"), Result);
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}

		MusicHandle = (ALAudioMusicHandle*)Music->Handle;
		if (MusicHandle->IsOgg)
		{
			Result = ov_time_total(MusicHandle->OggStream, -1);
		}
		else
		{
			xmp_get_frame_info(MusicHandle->xmpcontext, &xmpfi);
			Result = xmpfi.total_time / 1000;
		}
		Ar.Logf(TEXT("%i"), Result);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicInfo")))
	{
		Music = (appStrlen(Str) <= 1 ? Song : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}
		MusicHandle = (ALAudioMusicHandle*)Music->Handle;
		Ar.Logf(TEXT("%s"), *MusicHandle->MusicType);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicType")))
	{

		Music = (appStrlen(Str) <= 1 ? Song : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}
		MusicHandle = (ALAudioMusicHandle*)Song->Handle;

		if (MusicHandle->IsOgg)
		{
			//debugf(TEXT("GetMusicType STREAM"));
			Ar.Logf(TEXT("STREAM"));
		}
		else
		{
			//debugf(TEXT("GetMusicType TRACKER"));
			Ar.Logf(TEXT("TRACKER"));
		}

		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicTitle")))
	{
		Music = (appStrlen(Str) <= 1 ? Song : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}
		MusicHandle = (ALAudioMusicHandle*)Music->Handle;
		Ar.Logf(TEXT("%s"), *MusicHandle->MusicTitle);

		return 1;
	}
	if (ParseCommand(&Str, TEXT("MusicStop")))
	{
		if (!Song || !Song->Handle)
			return 1;
		/*
		//Just stop player... distortions remaining. Should stop OpenAL as well, but then no point in keeping it.
		MusicHandle=(ALAudioMusicHandle*)Song->Handle;
		xmp_stop_module(MusicHandle->xmpcontext);
		*/

		//Fade to stop...
		check(Viewport);
		check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
		Viewport->Actor->SongSection = 255;
		Viewport->Actor->Transition = MTRAN_FastFade;
		//Viewport->Actor->Song=NULL;
		return 1;


		/*
		//Full Stop.
		Viewport->Actor->SongSection = 255;
		MusicHandle=(ALAudioMusicHandle*)Song->Handle;
		MusicHandle->EndPlaying=1;
		return 1;
		*/
	}
	if (ParseCommand(&Str, TEXT("MusicStart")))
	{
		if (appStrlen(Str) <= 1)
		{
			if (Song)
			{
				//AudioLog(TEXT("Starting %s"),*Song->GetName());
				check(Viewport);
				check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
				Viewport->Actor->SongSection = 0;
				Viewport->Actor->Transition = MTRAN_FastFade;
				return 1;
			}
			else return 1;
		}
		else
			AudioLog(TEXT("Trying to load %s"), Str);

		Music = FindObject<UMusic>(ANY_PACKAGE, Str);

		if (!Music)
		{
			UPackage* Pkg = Cast<UPackage>(LoadPackage(NULL, Str, LOAD_Forgiving));
			if (!Pkg)
				return 1;
			Music = FindObject<UMusic>(ANY_PACKAGE, Str);
		}
		if (Music)
		{
			//debugf(TEXT("Loaded Music: %s"),Music->GetName());
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
			check(Viewport);
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->Song = Music;
			Viewport->Actor->SongSection = 0;
			Viewport->Actor->Transition = MTRAN_Segue;
		}
		return 1;
	}
	if (ParseCommand(&Str, TEXT("MusicRestart")))
	{
		Music = (appStrlen(Str) <= 1 ? Song : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}
		MusicHandle = (ALAudioMusicHandle*)Music->Handle;

		if (MusicHandle->IsPlaying)
		{
			AudioLog(TEXT("Restarting %s"), *MusicHandle->MusicTitle);
			xmp_restart_module(MusicHandle->xmpcontext);
		}
		else
		{
			AudioLog(TEXT("Starting %s"), *MusicHandle->MusicTitle);
			check(Viewport);
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->Song = Music;
			Viewport->Actor->SongSection = 0;
			Viewport->Actor->Transition = MTRAN_Instant;
		}

		return 1;
	}
	if (ParseCommand(&Str, TEXT("MusicRestore")))
	{
		check(Viewport);
		check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
		if (!Viewport->Actor->Level || !Viewport->Actor->Level->Song)
			return 1;

		if (Song && Song->Handle)
			StopMusic(Song);

		AudioLog(TEXT("Trying to restore %s"), Viewport->Actor->Level->Song->GetName());
		Viewport->Actor->Song = Viewport->Actor->Level->Song;
		Viewport->Actor->SongSection = Viewport->Actor->Level->SongSection;
		Viewport->Actor->Transition = MTRAN_Instant;

		return 1;
	}
#endif // !RUNE_CLASSIC
	if (ParseCommand(&Str, TEXT("MaxEffectsChannels")))
	{
		Ar.Logf(TEXT("%i"), MAX_EFFECTS_CHANNELS);
		return 1;
	}
	// HAN: Out of OpenGLDrv.
	if (ParseCommand(&Str, TEXT("AudioBuild")))
	{
		Ar.Logf(TEXT("ALAudio built: %s"), appFromAnsi(__DATE__ " " __TIME__));
		return 1;
	}
	// Command out of X-Com: Enforcer, but there is no reason to not always include it.
	if (ParseCommand(&Str, TEXT("STOPALLSOUNDS")))
	{
		Ar.Logf(TEXT("Sounds stopped"));
		StopAllSound();
		return 1;
	}

	// This seems to be a 420 thing.
#if ENGINE_VERSION>420 || RUNE_CLASSIC
	// Pause and Unpause Audio.
	if (ParseCommand(&Str, TEXT("PAUSEAUDIO")))
	{
		Ar.Logf(TEXT("Audio Paused!! (not implemented)"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("UNPAUSEAUDIO")))
	{
		Ar.Logf(TEXT("Audio Unpaused!! (not implemented)"));
		return 1;
	}
#endif

	// X-Com: Enforcer.
#if defined(XCOM_ENFORCER)
	// Bink related stuff.
	if (ParseCommand(&Str, TEXT("SetBinkAudio")))
	{
		BinkSetSoundSystem(BinkOpenWaveOut, 0);
		Ar.Logf(TEXT("Bink is using Wave Out!"));
		return 1;
	}

	// CD Playback related.
	if (ParseCommand(&Str, TEXT("CDTRACK")))
	{
		INT TrackNum = appAtoi(Str);
		Ar.Logf(TEXT("CD Track %i (not implemented)"), TrackNum);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("STOPCD")))
	{
		Ar.Logf(TEXT("Stopping CD! (not implemented)"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("STOPTEST")))
	{
		Ar.Logf(TEXT("Stopping CD Test! (not implemented)"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("LOOPTRACK")))
	{
		INT TrackNum = appAtoi(Str);
		Ar.Logf(TEXT("Loop CD Track %i (not implemented)"), TrackNum);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDTEST")))
	{
		INT TrackNum = appAtoi(Str);
		Ar.Logf(TEXT("CD Test!! (not implemented)"));
		// Output will look like this:
		//Ar.Logf(TEXT("CurrentCDTrack = %i"), INDEX_NONE);
		//Ar.Logf(TEXT("Current Track = %i"), INDEX_NONE);
		//Ar.Logf(TEXT("Num Tracks = %i"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDPREV")))
	{
		Ar.Logf(TEXT("Previous CD Track %i (not implemented)"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDWANK")))
	{
		Ar.Logf(TEXT("Wanking CD Track %i (not implemented)"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDNEXT")))
	{
		Ar.Logf(TEXT("Next CD Track %i (not implemented)"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("LoopCDTrack")))
	{
		Ar.Logf(TEXT("Loop CD Track !! (not implemented)"));
		if (0)
			Ar.Logf(TEXT("CD Track was looped!!"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDVOLUME")))
	{
		FLOAT Volume = appAtof(Str);
		Ar.Logf(TEXT("CD Volume %f (not implemented)"), Volume);
		return 1;
	}
#endif

	// Output Selection.
	if (ParseCommand(&Cmd, TEXT("AUDIOOUTPUT")))
	{
		if (ParseCommand(&Cmd, TEXT("GETNUMDEVICES")))
		{
			Ar.Logf(TEXT("%d"), DetectedDevices.Num());
		}
		else if (ParseCommand(&Cmd, TEXT("GETDEVICENAME")))
		{
			const INT DriverNum = appAtoi(Cmd);
			if (DriverNum >= 0 && DriverNum < DetectedDevices.Num())
			{
				FString DetectedDevice = DetectedDevices(DriverNum);
				if (DetectedDevice.InStr(TEXT("OpenAL Soft on ")) == 0)
					DetectedDevice = DetectedDevice.Mid(strlen("OpenAL Soft On "));
				Ar.Logf(TEXT("%ls"), *DetectedDevice);
			}
			else
			{
				Ar.Logf(TEXT("Unknown Device"));
			}
		}
		else if (ParseCommand(&Cmd, TEXT("GETDEVICE")))
		{
			Ar.Logf(TEXT("%d"), CurrentDevice);
		}
		else if (ParseCommand(&Cmd, TEXT("SETDEVICE")))
		{
			INT DriverNum = appAtoi(Cmd);
			BOOL ResetAudioDeviceGuid = FALSE;

			if (DriverNum == -1)
			{
				DriverNum = 0;
				ResetAudioDeviceGuid = TRUE;
			}

			if (DriverNum >= 0 && DriverNum < DetectedDevices.Num())
			{
				PreferredDevice = ResetAudioDeviceGuid ? TEXT("") : DetectedDevices(DriverNum);
				SaveConfig();
				Ar.Logf(TEXT("RESTART"));
			}
			else
			{
				Ar.Logf(TEXT("Unknown Device"));
			}
		}

		return 1;
	}

	return FALSE;
	unguard;
}

#if !UNREAL_TOURNAMENT_OLDUNREAL
#if DNF
UBOOL UALAudioSubsystem::PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch, UBOOL IsMonitoredSound)
#else
UBOOL UALAudioSubsystem::PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch)
#endif
{
	FVector Velocity = Actor ? Actor->Velocity : FVector(0,0,0);
	FLOAT   Priority = 1.f;
	UBOOL   World    = IdToSlot(Id) != SLOT_Interface;
#if DNF
	return PlaySound( Actor, Id, Sound, Location, Velocity, Volume, Radius, Pitch, Priority, World, IsMonitoredSound);
#else
	return PlaySound( Actor, Id, Sound, Location, Velocity, Volume, Radius, Pitch, Priority, World);
#endif
}
#endif

#if DNF
UBOOL UALAudioSubsystem::PlaySound(AActor* Actor, INT Id, USound* Sound, FVector Location, FVector Velocity, FLOAT Volume, FLOAT Radius, FLOAT Pitch, FLOAT Priority, UBOOL World, UBOOL IsMonitoredSound)
#else
UBOOL UALAudioSubsystem::PlaySound(AActor* Actor, INT Id, USound* Sound, FVector Location, FVector Velocity, FLOAT Volume, FLOAT Radius, FLOAT Pitch, FLOAT Priority, UBOOL World)
#endif
{
	INT Index = INDEX_NONE; // Put it up front so I can unguardf it. --han
	guard(UALAudioSubsystem::PlaySound);

	if (!Sound || !Viewport || !Viewport->Actor)
		return 0;

	// stijn: TODO: Use FSoundId instead of bit shifting crap...
	INT Slot = IdToSlot(Id);

	// Special case: stop playing a sound. (see: Engine.Decoration.Timer)
	if (Volume == 0)
	{
		if (Slot != SLOT_None)
			StopSoundId(Id);
		return 0;
	}

	// Compute priority.
	FLOAT PriorityMultiplier = Priority;
	Priority = SoundPriority(Location, GetCameraLocation(), Volume, Radius, Slot, PriorityMultiplier);
	FLOAT BestPriority = Priority;

	guard(FindChannel);
	for (INT i = 0; i < EffectsChannels; i++)
	{
		if ((PlayingSounds[i].Actor == Actor) && (PlayingSounds[i].SoundFlags & SF_Loop) && (PlayingSounds[i].Sound == Sound)) // never play already looping sound from same actor twice!
			return 0;
		if ((Slot != SLOT_None) && (PlayingSounds[i].Actor == Actor) && (PlayingSounds[i].Id & ~1) == (Id & ~1))
		{
			if (Id & 1)
				return 0;
			else
			{
				Index = i;
				break;
			}
		}
		else if (PlayingSounds[i].Priority <= BestPriority)
		{
			Index = i;
			BestPriority = PlayingSounds[i].Priority;
		}
	}
	unguard;

	// If no sound, or its priority is overruled, stop it.
	if (Index == -1)
		return 0;

	// If already playing, stop it.
	StopSound(Index);

	ALAudioSoundHandle* AudioHandle = NULL;
	guard(Handle);
#if 0
	// Crude hack.
	if (Sound == (USound*)-1) // Shouldn't that be Sound->Handle? Galaxy has the same shit.
		return 0; // nah: Loki's source and Galaxy return 1 here!
	else if (Sound->Handle == (void*)-1)
		appErrorf(TEXT("Sound handle and not Sound should be checked for INDEX_NONE"));
#else
	if (Sound == (USound*)-1 || Sound->Handle == (void*)-1)
		return 0;
#endif

	AudioHandle = (ALAudioSoundHandle*)Sound->Handle;
	if (!AudioHandle)
	{
		RegisterSound(Sound);
		AudioHandle = (ALAudioSoundHandle*)Sound->Handle;
		if (!AudioHandle)
			return 0;
		check(AudioHandle != (void*)-1);
	}
	unguard;


	DWORD SoundFlags = 0;
	guard(SoundFlags);
	INT Slot = IdToSlot(Id);
	if ( bPlayingAmbientSound )
		SoundFlags |= SF_AmbientSound;
	if ( AudioHandle->bLoopingSample )
		SoundFlags |= SF_LoopingSource;
	if ( Slot == SLOT_Interface )
		SoundFlags |= SF_No3D;
	if ( Slot == SLOT_Interface )
		SoundFlags |= SF_NoFilter;
	if ( UseSpeechVolume && (Slot == SLOT_Talk) ) 
		SoundFlags |= SF_Speech;
	if ( Actor )
	{
		AActor* Camera = GetCameraActor();
		if ( Camera && (Actor == Camera || (Camera->IsA(APawn::StaticClass()) && Actor == ((APawn*)Camera)->Weapon)) )
			SoundFlags |= SF_No3D;

		AZoneInfo* Zone = GetRegion( Actor, Slot==SLOT_Talk||Slot==SLOT_Pain).Zone;
		if ( Zone && Zone->bWaterZone )
			SoundFlags |= SF_WaterEmission;
	}
	unguard;

	//check(Index<EffectsChannels);

	guard(UpdatePlayingSound);
	ALAudioSoundInstance& Playing = PlayingSounds[Index];

	// Default state
	Playing.Sound = Sound;
	Playing.Actor = Actor;
	Playing.Priority = Priority;
	Playing.Id = Id;
	Playing.Volume = Volume;
	Playing.SoundFlags = SoundFlags;
	Playing.PriorityMultiplier = PriorityMultiplier;
#if DNF
	Playing.IsMonitoredSound = IsMonitoredSound;
#endif

	// Modify state with necessary API calls.
	Playing.Init();
	Playing.SetRadius(Radius);
	Playing.SetPitch(Pitch);
	Playing.SetDopplerFactor(1.0);
	Playing.SetLocation( Actor ? Actor->Location : Location);
	Playing.SetVelocity(Velocity);
	Playing.UpdateEmission( LastRenderCoords, (Viewport->Actor->ShowFlags & SHOW_PlayerCtrl) != 0);
	Playing.UpdateVolume(this);
	if ( EffectSet )
		Playing.SetEFX( iEffectSlot, AL_FILTER_NULL);
	if ( GFilterExtensionLoaded )
		Playing.UpdateAttenuation( AttenuationFactor(Playing)); //No DeltaTime: instant transition.

	if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: PlaySound alSourcef for sound %ls setting error: %ls"), *FObjectFullName(Sound), appFromAnsi(alGetString(error)));

	guard(Play);
	alSourcePlay(Playing.SourceID);
	unguard;

	unguard;



	return 1;
	unguardf((TEXT("(Actor=%s,Id=%i,Sound=%s,Location=(%f,%f,%f),Volume=%f,Radius=%f,Pitch=%f)|(Index=%i)"), *FObjectFullName(Actor), Id, *FObjectFullName(Sound), Location.X, Location.Y, Location.Z, Volume, Radius, Pitch, Index));
}

void UALAudioSubsystem::RenderAudioGeometry(FSceneNode* Frame)
{
	// Do nothing.
}

#ifdef EFX
//
// Retrieves EFX Effects.
//
// Basic control flow:
//   Initializes Info with default values (Out of old ALAudio.EFXZone)
//   Returns false if Zone==NULL.
//   Tries to call QueryEffects() UnrealScript event on Zone.
//   (Non    227): If calling fails returns false, return true otherwise.
//   (Unreal 227): If no preset in AZoneInfo, try calling.
//
UBOOL UALAudioSubsystem::QueryEffects(FEFXEffects& Effects, AZoneInfo* Zone, AActor* ViewActor)
{
	guard(UALAudioSubsystem::QueryEffects);

	struct eventQueryEffects_Parms
	{
		FEFXEffects Effects;
		AActor* ViewActor;
	} Parms;

	// Set ViewActor for Event.
	Parms.ViewActor = ViewActor;

	// Set default values (Taken out of old ALAudio.EFXZone).
	Parms.Effects.Version = 1;
	Parms.Effects.ReverbPreset = ARRAY_COUNT(g_ReverbPreset) - 1; // RP_None
	Parms.Effects.AirAbsorptionGainHF = 0.994f;
	Parms.Effects.DecayHFRatio = 0.83f;
	Parms.Effects.DecayLFRatio = 1.0;
	Parms.Effects.DecayTime = 1.49f;
	Parms.Effects.Density = 1.0;
	Parms.Effects.Diffusion = 1.0;
	Parms.Effects.EchoDepth = 0.0;
	Parms.Effects.EchoTime = 0.25;
	Parms.Effects.Gain = 0.32f;
	Parms.Effects.GainHF = 0.89f;
	Parms.Effects.GainLF = 0.0;
	Parms.Effects.HFReference = 5000.0;
	Parms.Effects.LFReference = 250.0;
	Parms.Effects.LateReverbDelay = 0.011f;
	Parms.Effects.LateReverbGain = 1.26f;
	Parms.Effects.RoomRolloffFactor = 0.0;
	Parms.Effects.bUserDefined = 0;
	Parms.Effects.bDecayHFLimit = 1;

#if ENGINE_VERSION==227
	if (Zone->EFXAmbients != ARRAY_COUNT(g_ReverbPreset) - 1) // If none check for userdefined method.
	{
		Effects = Parms.Effects; // Ugly struct copy.
		Effects.ReverbPreset = Zone->EFXAmbients;
		return 1;
	}
#endif

	UBOOL Called = Zone ? ProcessScript(Zone, ALAUDIO_QueryEffects, &Parms) : 0; // Call UnrealScript event if Zone is not NULL.

	// Clamp.
	if (Parms.Effects.bUserDefined)
	{
		Parms.Effects.AirAbsorptionGainHF = Clamp(Parms.Effects.AirAbsorptionGainHF, 0.892f, 1.f);
		Parms.Effects.DecayHFRatio = Clamp(Parms.Effects.DecayHFRatio, 0.1f, 20.f);
		Parms.Effects.DecayLFRatio = Clamp(Parms.Effects.DecayLFRatio, 0.1f, 20.f);
		Parms.Effects.DecayTime = Clamp(Parms.Effects.DecayTime, 0.1f, 20.f);
		Parms.Effects.Density = Clamp(Parms.Effects.Density, 0.f, 1.f);
		Parms.Effects.Diffusion = Clamp(Parms.Effects.Diffusion, 0.f, 1.f);
		Parms.Effects.EchoDepth = Clamp(Parms.Effects.EchoDepth, 0.f, 1.f);
		Parms.Effects.EchoTime = Clamp(Parms.Effects.EchoTime, 0.075f, 0.25f);
		Parms.Effects.Gain = Clamp(Parms.Effects.Gain, 0.f, 1.f);
		Parms.Effects.GainHF = Clamp(Parms.Effects.GainHF, 0.f, 1.f);
		Parms.Effects.GainLF = Clamp(Parms.Effects.GainLF, 0.f, 1.f);
		Parms.Effects.HFReference = Clamp(Parms.Effects.HFReference, 1000.f, 20000.f);
		Parms.Effects.LFReference = Clamp(Parms.Effects.LFReference, 20.f, 1000.f);
		Parms.Effects.LateReverbDelay = Clamp(Parms.Effects.LateReverbDelay, 0.f, 0.1f);
		Parms.Effects.LateReverbGain = Clamp(Parms.Effects.LateReverbGain, 0.1f, 10.f);
		Parms.Effects.RoomRolloffFactor = Clamp(Parms.Effects.RoomRolloffFactor, 0.0f, 10.f);
		Parms.Effects.bDecayHFLimit = Clamp(Parms.Effects.bDecayHFLimit, (BITFIELD)AL_FALSE, (BITFIELD)AL_TRUE);
	}

	Effects = Parms.Effects; // Ugly struct copy.

	if (Called)
		return 1;

	// We got EFX data out of the Zone, everything is fine.
	return 0;

	unguardf((TEXT("(Zone=%s, ViewActor=%s)"), *FObjectFullName(Zone), *FObjectFullName(ViewActor)));
}
#endif

void UALAudioSubsystem::StopSound(INT Index)
{
	guard(UALAudioSubsystem::StopSound);
	PlayingSounds[Index].Stop();
	unguard;
}

#if DNF
void UALAudioSubsystem::StopSoundBySlot(AActor* Actor, INT Id)
{
	INT Index = -1;
	for (INT i = 0; i < EffectsChannels; i++)
	{
		ALAudioSoundInstance& Playing = PlayingSounds[i];
		if (((Playing.Id & ~1) == (Id & ~1)) && (Playing.Actor == Actor))
		{
			Index = i;
			break;
		}
	}
	if (Index == -1)
		return;

	StopSound(Index);
}
#endif

void UALAudioSubsystem::Update( FPointRegion ListenerRegion, FCoords& ListenerCoords)
{
	ELevelTick TickType         = LEVELTICK_All;
	FLOAT      DeltaTime        = UpdateTime();
	AActor*    ListenerActor    = GetCameraActor();
	FVector    ListenerVelocity = ListenerActor->Velocity;

#if ENGINE_VERSION==227
	// Higor: FIXME SMIRFSTCH
	ListenerRegion = Viewport->Actor->CameraRegion;
#else
	if ( ListenerActor && ListenerActor->GetLevel() && ListenerActor->GetLevel()->Model )
		ListenerRegion = ListenerActor->GetLevel()->Model->PointRegion( ListenerActor->Level, ListenerCoords.Origin);
#endif

	Update( TickType, DeltaTime, ListenerRegion, ListenerCoords, ListenerVelocity, ListenerActor);
}


void UALAudioSubsystem::Update( ELevelTick TickType, FLOAT DeltaTime, FPointRegion ListenerRegion, FCoords& ListenerCoords, FVector ListenerVelocity, AActor* ListenerActor)
{
	guard(UALAudioSubsystem::Update);
	if ( !ListenerActor )
		ListenerActor = Viewport->Actor;
	if ( !Viewport || !Viewport->Actor || !context_id )
		return;

#if DNF
	MusicUpdate();
#endif

	// check for device alive
	int connected = 0;
	alcGetIntegerv(Device, ALC_CONNECTED, 1, &connected);
	LONG Counter = DeviceChangeCounter.GetValue();
	if(!connected || LastDeviceChangeCounter != Counter)
	{
		LastDeviceChangeCounter = Counter;

#ifdef ALC_SOFT_reopen_device
		if (alcIsExtensionPresent(Device, "ALC_SOFT_reopen_device") != ALC_TRUE)
			GWarn->Logf(TEXT("ALAudio: reopen: ALC_SOFT_reopen_device not present"));
		else
		{
			LPALCREOPENDEVICESOFT alcReopenDeviceSOFT = (LPALCREOPENDEVICESOFT)alcGetProcAddress(Device, "alcReopenDeviceSOFT");

			if (!alcReopenDeviceSOFT)
				GWarn->Logf(TEXT("ALAudio: alcReopenDeviceSOFT is NULL"));
			else
			{
				ALCint AttrList[32];
				appMemzero(AttrList, sizeof(AttrList));
				InitAttrList(&AttrList[0]);

				ALCboolean Reopen = ALC_FALSE;
				
				if ( PreferredDevice != TEXT(""))
					Reopen = alcReopenDeviceSOFT(Device, appToAnsi(*PreferredDevice), AttrList);
				if (Reopen != ALC_TRUE)
					Reopen = alcReopenDeviceSOFT(Device, nullptr, AttrList);
				if (Reopen != ALC_TRUE)
					GWarn->Logf(TEXT("ALAudio: reopen error: %s"), appFromAnsi(alGetString(alcGetError(Device))));
				else
				{
					/*LPALCRESETDEVICESOFT alcResetDeviceSOFT = (LPALCRESETDEVICESOFT)alcGetProcAddress(Device, "alcResetDeviceSOFT");
					if (!alcResetDeviceSOFT)
						GWarn->Logf(TEXT("ALAudio: alcResetDeviceSOFT is NULL"));
					else if (alcResetDeviceSOFT(Device, AttrList) != ALC_TRUE)
						GWarn->Logf(TEXT("ALAudio: reset (after reopen) error: %s"), appFromAnsi(alGetString(alcGetError(Device))));
					else*/
					{
						AudioLog(NAME_Init, TEXT("ALAudio: We are using OpenAL device:  %s (%s)"), appFromAnsi(alcGetString(Device, ALC_DEVICE_SPECIFIER)), appFromAnsi(alcGetString(Device, ALC_ALL_DEVICES_SPECIFIER)));

						// restart music if any
						if (Viewport->Actor->Song)
						{
							if (Viewport->Actor->Song->Handle)
								StopMusic(Viewport->Actor->Song);
							if (!Viewport->Actor->Song->Handle)
							{
								RegisterMusic(Viewport->Actor->Song);
								guard(StartMusic);
								if (Viewport->Actor->Song && Viewport->Actor->Song->Handle)
									StartMusic((ALAudioMusicHandle*)Viewport->Actor->Song->Handle);
								unguard;
							}
						}
					}
				}
			}
		}
#endif
	}

	// Get Render
	__Render = Viewport->GetOuterUClient()->Engine->Render;

	// Update listener
	alDopplerFactor(DopplerFactor);
	ALfloat ListenerPosition[3], ListenerVelocity[3], ListenerDirection[6];
	TransformCoordinates(ListenerPosition, ListenerCoords.Origin); //using the head location instead of the player location
	TransformCoordinates(ListenerVelocity, ListenerActor->Velocity);
	TransformCoordinates(ListenerDirection, ListenerCoords.ZAxis);
	TransformCoordinates(&ListenerDirection[3], -ListenerCoords.YAxis);
	alListenerfv(AL_ORIENTATION, ListenerDirection);
	alListenerfv(AL_POSITION, ListenerPosition);
	alListenerfv(AL_VELOCITY, ListenerVelocity);
	LastRenderCoords = ListenerCoords;
	LastViewerPos = ListenerActor->Location;

	ALAudioSoundInstance::UpdateParams UpdateParams;
	UpdateParams.ListenerActor = ListenerActor;
	UpdateParams.AudioSub = this;
	UpdateParams.DeltaTime = DeltaTime;
	UpdateParams.Realtime = Viewport->IsRealtime() && Viewport->Actor->Level->Pauser == TEXT("");
	UpdateParams.LightRealtime = (Viewport->Actor->ShowFlags & SHOW_PlayerCtrl) != 0;

	guard(UpdateSounds);
	alcSuspendContext(context_id);
	INT Updated = 0;
	for ( INT i=0; i<EffectsChannels; i++)
		Updated += PlayingSounds[i].Update( UpdateParams);
	alcProcessContext(context_id);
	unguard;

	// See if any new ambient sounds need to be started.
	guard(StartAmbience);
	if ( UpdateParams.Realtime )
	{
		for ( INT i=0 ; i<Viewport->Actor->GetLevel()->Actors.Num() ; i++)
		{
			AActor* Actor = Viewport->Actor->GetLevel()->Actors(i);
			if (Actor && Actor->AmbientSound && FDistSquared(ListenerCoords.Origin, Actor->Location) <= Square(Actor->WorldSoundRadius()))
			{
				FSoundId Id(Actor, SLOT_Ambient, false, true);
				INT j;
				for ( j=0 ; j<EffectsChannels ; j++)
				{
					ALAudioSoundInstance& Playing = PlayingSounds[j];
					if ( (Playing.Actor == Actor) && (Playing.Id == Id.Id) )
					{
						if ( Playing.Sound != Actor->AmbientSound )
						{
							if ( Playing.SoundFlags & SF_AmbientSound ) // Replacing automatic ambient sound
							{
								StopSound(j);
								j = EffectsChannels;
							}
							else // Ambient sound being overriden by script PlaySound with SLOT_Ambient
							{
								//TODO: Move to abstraction
								FLOAT SoundTime;
								alGetSourcef( Playing.SourceID, AL_SAMPLE_OFFSET, &SoundTime);
								ALAudioSoundHandle* AudioHandle = (ALAudioSoundHandle*)Playing.Sound->Handle;
								SoundTime /= AudioHandle->Rate;
								// Ambient sound override about to end, play next sound now
								if ( SoundTime + DeltaTime > AudioHandle->Duration )
								{
									StopSound(j);
									j = EffectsChannels;
								}
							}
						}
						break;
					}
				}

				if (j == EffectsChannels)
				{
					bPlayingAmbientSound = TRUE;
					PlaySound
					(
						Actor, 
						Id.Id, 
						Actor->AmbientSound,
						Actor->Location,
						Actor->Velocity,
						AMBIENT_FACTOR * ((FLOAT)Actor->SoundVolume / 255.0), 
						Actor->WorldSoundRadius(), 
						Actor->SoundPitch / 64.0,
					#if DNF
						0.75f,
					#else
						Actor->SoundPriority ? static_cast<FLOAT>(Actor->SoundPriority) / 16.f : 0.75f,
					#endif
						TRUE
					#if DNF
						, FALSE
					#endif
					);
					bPlayingAmbientSound = FALSE;
				}
			}
		}
	}
	unguard;

	//Music Code
#if !RUNE_CLASSIC
	guard(DigitalMusic);
	if (UseDigitalMusic && Viewport->Actor)
	{
		ALfloat FMusicVolume = ((FLOAT)MusicVolume / 255.f);
		if (Transiting)
		{
			//debugf(TEXT("Transiting"));
			if (MFrom && MFrom->Handle)
			{
				ALAudioMusicHandle *MusicHandle = (ALAudioMusicHandle*)MFrom->Handle;
				ALfloat MFromVol = 0.f;
				alGetSourcef(MusicHandle->musicsource, AL_GAIN, &MFromVol);
				//debugf(TEXT("MFrom %s Current MFromVol %f equals 0 = %i"),MFrom->GetName(),MFromVol,approximatelyEqualFloat(MFromVol,0.f,0.05f));
				if (MFromVol > 0.01f)
				{
					//debugf(TEXT("MFrom %s MFromVol %f > 0.f"),MFrom->GetName(),MFromVol);
					if (Viewport->Actor->Transition == MTRAN_Instant)
					{
						MFromVol = FMusicVolume;
						//debugf(TEXT("MFrom MTRAN_Instant %s MFromVol %f"),MFrom->GetName(),MFromVol);
					}
					else
						MFromVol = FMusicVolume - (FMusicVolume / (MusicFadeOutTime / (appSecondsAudio() - TransitionStartTime)));

					//debugf(TEXT("MFrom %s MFromVol %f MusicFadeOutTime %f appSecondsAudio() %f, TransitionStartTime %f"),MFrom->GetName(),MFromVol,MusicFadeOutTime,appSecondsAudio(),TransitionStartTime);
					MFromVol = Clamp(MFromVol, 0.f, (GIsEditor ? 1.f : FMusicVolume));

					//debugf(TEXT("MFrom MFromVol %f"),MFromVol);
					alSourcef(MusicHandle->musicsource, AL_GAIN, MFromVol);

					if (MFromVol <= 0.01f) // check again and stop if necessary, needed for crossfading.
					{
						//debugf(TEXT(" MFromVol <= 0.01f StopMusic %s"),MFrom->GetName());
						if (MTo && MTo != MFrom)
							StopMusic(MFrom);
						MFrom = NULL;
						if (!CrossFading)
							TransitionStartTime = appSecondsAudio(); //Update TransitionTime for FadeIn
					}
				}
				else
				{
					//debugf(TEXT("MFrom && MFrom->Handle else StopMusic %s"),MFrom->GetName());
					if (MTo && MTo != MFrom)
						StopMusic(MFrom);
					MFrom = NULL;
					if (!CrossFading)
						TransitionStartTime = appSecondsAudio(); //Update TransitionTime for FadeIn
				}
			}
			if (MTo && MTo->Handle)
			{
				ALAudioMusicHandle *MusicHandle = (ALAudioMusicHandle*)MTo->Handle;
				ALfloat MToVol = 0.f;

				//debugf(TEXT("MTo %s CrossFading %i"),MTo->GetName(),CrossFading);

				if (MFrom && !CrossFading && ((TransitionStartTime - appSecondsAudio()) > 0.01f))
				{
					//debugf(TEXT("Not finished fading out..."));
					return; // not finished fading out
				}
				else if (CrossFading && !((MusicFadeInTime > 0.f) && (appSecondsAudio() - TransitionStartTime) >= (MusicFadeOutTime - MusicFadeInTime)))
				{
					//debugf(TEXT("Don't start Crossfading yet..."));
					return; // don't start yet.
				}

				if (!MusicHandle->IsPlaying)
				{
					MusicHandle->SongSection = Viewport->Actor->SongSection;
					if (!StartMusic(MusicHandle))
					{
						//fatal!
						GWarn->Logf(TEXT("ALAudio: Unable to start music %s"), MTo->GetName());
						MTo = NULL;
						CrossFading = FALSE;
						Transiting = FALSE;
					}
					if (MusicFadeInTime>0)
					{
						if (CrossFading)
						{
							MToVol = FMusicVolume / (MusicFadeInTime / (appSecondsAudio() - (TransitionStartTime + MusicFadeOutTime - MusicFadeInTime)));
						}
						else if (MusicFadeInTime >0)
							MToVol = FMusicVolume / (MusicFadeInTime / (appSecondsAudio() - TransitionStartTime));
					}
					else
						MToVol = (GIsEditor ? 1.f : FMusicVolume);
					alSourcef(MusicHandle->musicsource, AL_GAIN, MToVol); // be sure to set initial volume.
				}
				alGetSourcef(MusicHandle->musicsource, AL_GAIN, &MToVol);
				//debugf(TEXT("MTo Current Volume: %f == %f = %i"),MToVol,(GIsEditor ? 1.f : ((FLOAT)MusicVolume/255)),approximatelyEqualFloat(MToVol,( GIsEditor ? 1.f : FMusicVolume),0.01));

				if (!approximatelyEqualFloat(MToVol, (GIsEditor ? 1.f : FMusicVolume), 0.05f))
				{
					if (MusicFadeInTime>0)
					{
						if (CrossFading)
						{
							MToVol = FMusicVolume / (MusicFadeInTime / (appSecondsAudio() - (TransitionStartTime + MusicFadeOutTime - MusicFadeInTime)));
						}
						else if (MusicFadeInTime >0)
							MToVol = FMusicVolume / (MusicFadeInTime / (appSecondsAudio() - TransitionStartTime));
					}
					else MToVol = (GIsEditor ? 1.f : FMusicVolume);

					MToVol = Clamp(MToVol, 0.f, (GIsEditor ? 1.f : FMusicVolume));
					//debugf(TEXT("MTo %s MToVol %f MusicFadeInTime %f appSecondsAudio() %f, TransitionStartTime %f"),MTo->GetName(),MToVol,MusicFadeInTime,appSecondsAudio(),TransitionStartTime);

					//debugf(TEXT("New Volume: %f"),MToVol);
					if (MToVol == (GIsEditor ? 1.f : FMusicVolume))
					{
						//debugf(TEXT("MToVol %f (FLOAT)MusicVolume/255)) %f"),MToVol,((FLOAT)MusicVolume/255));

						if (MFrom && MFrom->Handle) //still running Music, shouldn't happen, unregister!
						{
							//debugf(TEXT("MToVol==(GIsEditor ? 1.f : FMusicVolume) StopMusic %s"),MFrom->GetName());
							if (MFrom != MTo) //only unregister when transiting to a different song, not when transiting from one subtrack to another.
								StopMusic(MFrom);
							MFrom = NULL;
						}
						MFrom = MTo;
						MTo = NULL;
						CrossFading = FALSE;
						Transiting = FALSE;
					}
				}
				else
				{
					if (MFrom && MFrom->Handle) //still running Music, shouldn't happen, unregister!
					{
						//debugf(TEXT("approximatelyEqualFloat StopMusic %s"),MFrom->GetName());
						if (MFrom != MTo) //only unregister when transiting to a different song, not when transiting from one subtrack to another.
							StopMusic(MFrom);
						MFrom = NULL;
					}
					//debugf(TEXT("MTo stop"));
					MFrom = MTo;
					MTo = NULL;
					Transiting = FALSE;
				}
			}
			if (!MFrom && !MTo)
			{
				//debugf(TEXT("Nothing to play, stopping music"));
				// Nothing to play. Clean up.
				for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
					if (MusicIt->Handle)
						StopMusic(*MusicIt);
				Transiting = FALSE;
			}
		}
		else if (Viewport->Actor->Song && Viewport->Actor->Song->Handle)
		{
			ALAudioMusicHandle *MusicHandle = (ALAudioMusicHandle*)Viewport->Actor->Song->Handle;
			if (MusicHandle)
			{
				if (MusicHandle->BufferError != AL_NO_ERROR)
				{
					GWarn->Logf(TEXT("ALAudio: Musicbuffer error: %s"), appFromAnsi(alGetString(MusicHandle->BufferError))); // catch error here since can't print out from bufferthread directly.
					StopMusic(Viewport->Actor->Song);
				}
				alSourcef(MusicHandle->musicsource, AL_GAIN, (GIsEditor ? 1.f : FMusicVolume));

				// Stereo Angle
				Angles[0] = MusicStereoAngle * PI / 180;
				Angles[1] = -Angles[0];
				alSourcefv(MusicHandle->musicsource, AL_STEREO_ANGLES, Angles);

				if (MusicHandle->IsPlaying && !MusicHandle->IsOgg)
				{
					xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_MIX, MusicStereoMix); //update stereo mix

					if ((MusicHandle->SongSection != Viewport->Actor->SongSection) && (Viewport->Actor->Transition == MTRAN_None)) // update subtrack switching if done without transition.
					{
						AudioLog(NAME_DevMusic, TEXT("ALAudio: Switching to SongSection %i"), Viewport->Actor->SongSection);
						MusicHandle->SongSection = Viewport->Actor->SongSection;
						xmp_set_position(MusicHandle->xmpcontext, MusicHandle->SongSection);
					}

				}
			}
		}

		if (Viewport->Actor->Transition != MTRAN_None && !Transiting)
		{
			if (Viewport->Actor->Song)
				AudioLog(NAME_DevMusic, TEXT("ALAudio: Transiting to %s, SongSection %i with MTRAN %i"), *FObjectFullName(Viewport->Actor->Song), Viewport->Actor->SongSection, Viewport->Actor->Transition);
			else
				AudioLog(NAME_DevMusic, TEXT("ALAudio: Transiting to None with MTRAN %i"), Viewport->Actor->Transition);
			CrossFading = FALSE;

			switch (Viewport->Actor->Transition)
			{
			case MTRAN_Instant:
				MusicFadeInTime = 0.f;
				MusicFadeOutTime = 0.1f;
				break;
			case MTRAN_Segue:
				if (0)
				{
					MusicFadeInTime = 4.f;
					MusicFadeOutTime = 3.f;
				}
				else
				{
					CrossFading = TRUE;
					MusicFadeInTime = 7.f;
					MusicFadeOutTime = 3.f;
				}
				break;
			case MTRAN_Fade:
				MusicFadeInTime = 0.f;
				MusicFadeOutTime = 3.f;
				break;
			case MTRAN_FastFade:
				MusicFadeInTime = 0.f;
				MusicFadeOutTime = 1.f;
				break;
			case MTRAN_SlowFade:
				MusicFadeInTime = 0.f;
				MusicFadeOutTime = 5.f;
				break;
			default:
				MusicFadeInTime = 0.f;
				MusicFadeOutTime = 0.f;
			}

			/*
			if (MFrom)
			{
			debugf(TEXT("MFrom %s"),MFrom->GetName());
			if (MFrom->Handle)
			debugf(TEXT("MFrom->Handle %s"),MFrom->GetName());
			}
			if (MTo)
			{
			debugf(TEXT("MTo %s"),MTo->GetName());
			if (MTo->Handle)
			debugf(TEXT("MTo->Handle %s"),MTo->GetName());
			}
			if (Viewport->Actor->Song)
			{
			debugf(TEXT("Viewport->Actor->Song %s"),Viewport->Actor->Song->GetName());
			if (!Viewport->Actor->Song->Handle)
			RegisterMusic(Viewport->Actor->Song);
			if (Viewport->Actor->Song->Handle)
			debugf(TEXT("Viewport->Actor->Song->Handle %s"),Viewport->Actor->Song->GetName());
			}
			*/
			if (MTo)
				MFrom = MTo;
			MTo = NULL;
			Transiting = TRUE;
			TransitionStartTime = appSecondsAudio();
			Viewport->Actor->Transition = MTRAN_None;
		}
		if (MTo != Viewport->Actor->Song && Viewport->Actor->SongSection != 255)
		{
			MTo = Viewport->Actor->Song;
			//debugf(TEXT("Viewport->Actor->SongSection!=255"));
		}
		if (Viewport->Actor->Song && !Viewport->Actor->Song->Handle)
		{
			//debugf(TEXT("Viewport->Actor->Song && !Viewport->Actor->Song->Handle"));
			RegisterMusic(Viewport->Actor->Song);
			if (Viewport->Actor->Song && !Viewport->Actor->Song->Handle) // need to check for Viewport->Actor->Song again in case it was nulled due to being invalid format.
				MTo = NULL; // Failed to register.
		}
	}
	unguard;
#endif // !RUNE_CLASSIC.

#if RUNE_CLASSIC
	// Update certain raw things like MusicVolume.
	UpdateRaw();
#endif

	/*------------------------------------------------------------------------------------
	Reverb implementation for Zoneinfo
	------------------------------------------------------------------------------------*/
#ifdef EFX
	if ( UseReverb && GEffectsExtensionLoaded )
	{
		guard(UseReverb);

		alGetError();


		if (!ListenerRegion.Zone || OldAssignedZone == ListenerRegion.Zone || SoundVolume == 0)
			goto EFXDone;
		FLOAT MasterGain = (FLOAT)(ListenerRegion.Zone->MasterGain / 255.f);

		OldAssignedZone = ListenerRegion.Zone;
		ReverbZone = ListenerRegion.Zone->bReverbZone;
		RaytraceZone = ListenerRegion.Zone->bRaytraceReverb;
		speedofsound = ListenerRegion.Zone->SpeedOfSound;
		ReverbHFDamp = Clamp(ListenerRegion.Zone->CutoffHz, 0, OutputRate);//max samp rate
		zonenumber = ListenerRegion.ZoneNumber;
		Ambient = ARRAY_COUNT(g_ReverbPreset) - 1;

		FEFXEffects Effects;
		UBOOL RetrievedEffects = QueryEffects(Effects, ListenerRegion.Zone, Viewport->Actor);

		if (RetrievedEffects)
			Ambient = Effects.ReverbPreset;

		ALuint iEffect = 0;

		//debugf(TEXT("OldReverbVolume %f,(FLOAT)(Region.Zone->MasterGain/255.0f) %f, OldReverbIntensity %f, DopplerFactor %f"),OldReverbVolume,(FLOAT)(Region.Zone->MasterGain/255.0f),OldReverbIntensity,DopplerFactor);

#if DNF
		bSpecialZone = RetrievedEffects;
#else
		bSpecialZone = (RetrievedEffects && !Viewport->Actor->bShowMenu);
#endif

		// Zone has changed and Effect is in place , so remove all EFX effects to be ready for new effect.
		// Higor: this causes effect cutoff when going from one reverb zone to another
		// So we only do this when the next zone doesn't want an effect.
		if ( EffectSet && !ReverbZone && !bSpecialZone ) 
		{
			//debugf(TEXT("Zonechange, removing old EFX effect"));
			alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
			EffectSet = 0;
		}
		if (ReverbZone || bSpecialZone) // maybe needs to be adjusted more to the original reverb model - Smirftsch
		{
			// Create EFX objects...
			//debugf(TEXT("setting ReverbZone"));
			EFXEAXREVERBPROPERTIES efxReverb = {0};

			alGenEffects(1, &iEffect);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: alGenEffects create error: %s"), appFromAnsi(alGetString(error)));
				goto Error;
			}

			//debugf(TEXT("alIsAuxiliaryEffectSlot %i"),alIsAuxiliaryEffectSlot(iEffectSlot));

			// Send automatic adjustments
			alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, AL_TRUE);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: alAuxiliaryEffectSloti create error: %s"), appFromAnsi(alGetString(error)));
				goto Error;
			}

			// Change some arbitrary reverb parameter
			if (Effects.bUserDefined)
			{
				efxReverb.flAirAbsorptionGainHF = Effects.AirAbsorptionGainHF;
				efxReverb.flDecayHFRatio = Effects.DecayHFRatio;
				efxReverb.flDecayLFRatio = Effects.DecayLFRatio;
				efxReverb.flDecayTime = Effects.DecayTime;
				efxReverb.flDensity = Effects.Density;
				efxReverb.flDiffusion = Effects.Diffusion;
				efxReverb.flEchoDepth = Effects.EchoDepth;
				efxReverb.flEchoTime = Effects.EchoTime;
				efxReverb.flGain = Effects.Gain;
				efxReverb.flGainHF = Effects.GainHF;
				efxReverb.flHFReference = Effects.HFReference;
				efxReverb.flLateReverbDelay = Effects.LateReverbDelay;
				efxReverb.flLateReverbGain = Effects.LateReverbGain;
				efxReverb.flLFReference = Effects.LFReference;
				efxReverb.flRoomRolloffFactor = Effects.RoomRolloffFactor;
				efxReverb.iDecayHFLimit = Effects.bDecayHFLimit;
				/*
				// yet unused?
				efxReverb.flModulationDepth		=	Effects.lModulationDepth;
				efxReverb.flModulationTime		=	Effects.lModulationTime;
				efxReverb.flReflectionsDelay	=	Effects.lReflectionsDelay;
				efxReverb.flReflectionsGain		=	Effects.lReflectionsGain;
				efxReverb.flReflectionsPan[3]	=	Effects.lReflectionsPan[3];
				efxReverb.flLateReverbPan[3]	=	Effects.lLateReverbPan[3];
				*/
			}
			else if (EmulateOldReverb && (Ambient == (ARRAY_COUNT(g_ReverbPreset) - 1)))
			{
				MaxDelay = 0.f;
				MaxGain = 0.f;
				MinDelay = 0.f;
				AvgDelay = 0.f;
				T = 0.f;

				//Calculate reverb time from six comb filters
				INT i = 0;

				for (i = 0; i<ARRAY_COUNT(ListenerRegion.Zone->Delay); i++)
				{
					DelayTime[i] = Clamp(ListenerRegion.Zone->Delay[i] / 500.0f, 0.001f, 0.340f);
					DelayGain[i] = Clamp(ListenerRegion.Zone->Gain[i] / 255.0f, 0.001f, 0.999f);
					if ((DelayTime[i] * DelayGain[i])>(MaxDelay*MaxGain))
					{
						MaxDelay = DelayTime[i];
						MaxGain = DelayGain[i];
						//Reverb->Time=(float)((-60.0f*MaxDelay)/(20.0f*log10(MaxGain)));
					}
					if (DelayTime[i]<MinDelay)
						MinDelay = DelayTime[i];
					AvgDelay += (DelayTime[i] / 6.0f);
					T += DelayTime[i];
				}
				RoomSize = 3.0f*(AvgDelay - MinDelay) / (MaxDelay - MinDelay);

				if (RoomSize<0.4f)
				{
					efxReverb = *GetEFXReverb(110);
					//debugf(TEXT("Ambient: 110"));
				}
				else if (RoomSize<0.8f)
				{
					efxReverb = *GetEFXReverb(26);
					//debugf(TEXT("Ambient: 26"));
				}
				else if (RoomSize<1.2f)
				{
					efxReverb = *GetEFXReverb(44);
					//debugf(TEXT("Ambient: 44"));
				}
				else if (RoomSize<1.6f)
				{
					efxReverb = *GetEFXReverb(27);
					//debugf(TEXT("Ambient: 27"));
				}
				else if (RoomSize<2.0f)
				{
					efxReverb = *GetEFXReverb(45);
					//debugf(TEXT("Ambient: 45"));
				}
				else if (RoomSize<2.4f)
				{
					efxReverb = *GetEFXReverb(28);
					//debugf(TEXT("Ambient: 28"));
				}
				else if (RoomSize<4.f)
				{
					efxReverb = *GetEFXReverb(29);
					//debugf(TEXT("Ambient: 29"));
				}
				else if (RoomSize<6.0f)
				{
					efxReverb = *GetEFXReverb(21);
					//debugf(TEXT("Ambient: 21"));
				}
				else if (RoomSize<8.0f)
				{
					efxReverb = *GetEFXReverb(90);
					//debugf(TEXT("Ambient: 90"));
				}
				else
				{
					efxReverb = *GetEFXReverb(18);
					//debugf(TEXT("Ambient: 18"));
				}
				//debugf(TEXT("OpenAL: Roomsize: %f MasterGain %f, efxReverb.flGain %f, OldReverbIntensity %f"),RoomSize, MasterGain, efxReverb.flGain, OldReverbIntensity);
				efxReverb.flGain = Clamp((MasterGain * efxReverb.flGain * OldReverbIntensity), 0.001f, 0.999f);
			}
			else
			{
			    efxReverb = *GetEFXReverb(Ambient);
			    //debugf(TEXT("Ambient: %i"),Ambient);
			    //debugf(TEXT("OpenAL: Ambient %i MasterGain %f, efxReverb.flGain %f, ReverbIntensity %f"),Ambient, MasterGain, efxReverb.flGain, ReverbIntensity);
				efxReverb.flGain = Clamp((MasterGain * efxReverb.flGain * ReverbIntensity), 0.001f, 0.999f);
			}
			//debugf(TEXT("Region.Zone %s ambients %i"), *FObjectFullName(Region.Zone), Ambient);
			alEffecti(iEffect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
			alEffectf(iEffect, AL_REVERB_DENSITY, efxReverb.flDensity);
			alEffectf(iEffect, AL_REVERB_GAIN, efxReverb.flGain);
			alEffectf(iEffect, AL_REVERB_GAINHF, efxReverb.flGainHF);
			alEffectf(iEffect, AL_REVERB_DECAY_TIME, efxReverb.flDecayTime);
			alEffectf(iEffect, AL_REVERB_DECAY_HFRATIO, efxReverb.flDecayHFRatio);
			alEffectf(iEffect, AL_REVERB_REFLECTIONS_GAIN, efxReverb.flReflectionsGain);
			alEffectf(iEffect, AL_REVERB_REFLECTIONS_DELAY, efxReverb.flReflectionsDelay);
			alEffectf(iEffect, AL_REVERB_LATE_REVERB_GAIN, efxReverb.flLateReverbGain);
			alEffectf(iEffect, AL_REVERB_LATE_REVERB_DELAY, efxReverb.flLateReverbDelay);
			alEffectf(iEffect, AL_REVERB_AIR_ABSORPTION_GAINHF, efxReverb.flAirAbsorptionGainHF);
			alEffectf(iEffect, AL_REVERB_ROOM_ROLLOFF_FACTOR, efxReverb.flRoomRolloffFactor);
			alEffecti(iEffect, AL_REVERB_DECAY_HFLIMIT, efxReverb.iDecayHFLimit);

            /*
			debugf(TEXT("AL_REVERB_DENSITY%f"),efxReverb.flDensity);
			debugf(TEXT("AL_REVERB_GAIN %f"),efxReverb.flGain);
			debugf(TEXT("AL_REVERB_GAINHF %f"),efxReverb.flGainHF);
			debugf(TEXT("AL_REVERB_DECAY_TIME %f"),efxReverb.flDecayTime);
			debugf(TEXT("AL_REVERB_DECAY_HFRATIO %f"),efxReverb.flDecayHFRatio);
			debugf(TEXT("AL_REVERB_REFLECTIONS_GAIN %f"),efxReverb.flReflectionsGain);
			debugf(TEXT("AL_REVERB_REFLECTIONS_DELAY %f"),efxReverb.flReflectionsDelay);
			debugf(TEXT("AL_REVERB_LATE_REVERB_GAIN %f"),efxReverb.flLateReverbGain);
			debugf(TEXT("AL_REVERB_LATE_REVERB_DELAY %f"),efxReverb.flLateReverbDelay);
			debugf(TEXT("AL_REVERB_AIR_ABSORPTION_GAINHF %f"),efxReverb.flAirAbsorptionGainHF);
			debugf(TEXT("AL_REVERB_ROOM_ROLLOFF_FACTOR %f"),efxReverb.flRoomRolloffFactor);
			debugf(TEXT("AL_REVERB_DECAY_HFLIMIT %f"),efxReverb.iDecayHFLimit);
			*/

			// Check error status
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: EFX reverb parameter change error: %s"), appFromAnsi(alGetString(error)));
				goto Error;
			}

			// Re-load effect into slot
			if (alIsAuxiliaryEffectSlot(iEffectSlot))
			{
				alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_EFFECT, iEffect);
				// Check error status
				if ((error = alGetError()) != AL_NO_ERROR)
				{
					GWarn->Logf(TEXT("ALAudio: alAuxiliaryEffectSloti effectslot error: %s"), appFromAnsi(alGetString(error)));
					goto Error;
				}
			}
			EffectSet = 1; // Effect has been set;

			if (alIsEffect(iEffect))
				alDeleteEffects(1, &iEffect);

			if (!EffectSet)
			{
			Error:
				GWarn->Logf(TEXT("ALAudio: Error in EFX, unsetting any effects."));
				if (alIsAuxiliaryEffectSlot(iEffectSlot))
					alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
				if (alIsEffect(iEffect))
					alDeleteEffects(1, &iEffect);
				EffectSet = 0;
			}
		}
		unguard;
	}
EFXDone:;
#endif
	unguard;
}
void UALAudioSubsystem::PostRender(FSceneNode* Frame)
{
	guard(UALAudioSubsystem::PostRender);

	Frame->Viewport->Canvas->Color = FColor(255, 255, 255);
	if (AudioStats)
	{
		Frame->Viewport->Canvas->CurX = 0;
		Frame->Viewport->Canvas->CurY = 16;
		Frame->Viewport->Canvas->WrappedPrintf
			(Frame->Viewport->Canvas->SmallFont, 0, TEXT("ALAudioSubsystem Statistics:"));
		INT i = 0;
		for (i = 0; i<EffectsChannels; i++)
		{
			// Current Sound.
			Frame->Viewport->Canvas->CurX = 10;
			Frame->Viewport->Canvas->CurY = 24 + 8 * i;
			Frame->Viewport->Canvas->WrappedPrintf
				(Frame->Viewport->Canvas->SmallFont, 0, TEXT("Channel %i: %s"),
				i, *PlayingSounds[i].GetSoundInformation(DetailStats));
		}
#if !RUNE_CLASSIC
		INT MusicNum = 0;
		for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
		{
			if (MusicIt && MusicIt->Handle)
			{
				ALAudioMusicHandle* MusicHandle = (ALAudioMusicHandle*)MusicIt->Handle;

				INT Offset = 0, MusicLen = 0, Volume = MusicVolume;
				if (MusicHandle->IsOgg)
				{
					Offset = ov_time_tell(MusicHandle->OggStream);
					MusicLen = ov_time_total(MusicHandle->OggStream, -1);
				}
				else
				{
					if (MusicHandle->IsPlaying)
					{
						xmp_get_frame_info(MusicHandle->xmpcontext, &xmpfi);
						Offset = xmpfi.time / 1000;
						MusicLen = xmpfi.total_time / 1000;
						Volume = xmpfi.volume;
					}
				}
				Frame->Viewport->Canvas->CurX = 10;
				Frame->Viewport->Canvas->CurY = 24 + 8 * (i + 1 + MusicNum);
				if (DetailStats)
				{
					Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->SmallFont, 0, TEXT("MusicChannel %i: Title: %s - Vol: %i"), MusicNum, *MusicHandle->MusicTitle, Volume);
					Frame->Viewport->Canvas->CurY = 24 + 8 * (i + 2 + MusicNum);
					Frame->Viewport->Canvas->CurX = 10;
					Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->SmallFont, 0, TEXT("Offset: %i TotalLength : %i Type : %s"), Offset, MusicLen, *MusicHandle->MusicType);
				}
				else Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->SmallFont, 0, TEXT("MusicChannel %i: Title: %s"), MusicNum, *MusicHandle->MusicTitle);
				MusicNum++;
			}

		}
#endif // !RUNE_CLASSIC
	}
	unguard;
}

/*-----------------------------------------------------------------------------
Destroy & Shutdown.
-----------------------------------------------------------------------------*/

void UALAudioSubsystem::NoteDestroy( AActor* Actor)
{
	guard(UALAudioSubsystem::NoteDestroy);
	check(Actor);
	check(Actor->IsValid());

	// Stop referencing actor.
	for ( INT i=0; i<EffectsChannels; i++)
	{
		if (PlayingSounds[i].Actor == Actor)
		{
			if ( PlayingSounds[i].SoundFlags & SF_Loop )
			{
				// Stop ambient sound when actor dies.
				StopSound(i);
			}
			else
			{
				// Unbind regular sounds from actors.
				PlayingSounds[i].Actor = NULL;
			}
		}
	}

	unguard;
}

inline INT IdToBitmask( INT Id) 
{
	return 1 << IdToSlot(Id);
}

void UALAudioSubsystem::NoteDestroy( AActor* Actor, DWORD SlotStopMask)
{
	guard(UALAudioSubsystem::NoteDestroy469);
	check(Actor);
	check(Actor->IsValid());

	// Stop or referencing actor, may stop the sound as well.
	for (INT i = 0; i<EffectsChannels; i++)
	{
		ALAudioSoundInstance& Playing = PlayingSounds[i];
		if ( Playing.Actor == Actor )
		{
			if ( (SlotStopMask & (1 << IdToSlot(Playing.Id))) )
				Playing.Stop();
			else
				Playing.Actor = nullptr;
		}
	}

	unguard;
}

void UALAudioSubsystem::Destroy()
{
	guard(UALAudioSubsystem::Destroy);

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(WIN32)
	TimerEnd();
#endif

	if (Initialized)
	{
		// Unregister all music
#if !RUNE_CLASSIC
		for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
			if (MusicIt->Handle)
				StopMusic(*MusicIt);
		MFrom = NULL;
		MTo = NULL;
#endif

		// Unhook.
		EndBuffering = TRUE; //Make sure to have no thread left buffering!!
#if __GNUG__
		// stijn: @Smirftsch, I do not understand why you had this pthread_exit
		// call here. This is totally unsafe. It shuts down the main game
		// thread, but leaves other threads running. On my Linux machine, I
		// still had two pulseaudio threads running in the background while my
		// main game thread was gone. This turned my ut-bin into a zombie
		// process.
		
//		pthread_exit(NULL);
#endif
		USound::Audio = NULL;
#if !RUNE_CLASSIC
		UMusic::Audio = NULL;
#endif

		// Shut down viewport.
		SetViewport(NULL);

		AudioLog(NAME_Exit, TEXT("ALAudio: subsystem shut down."));

#ifdef EFX
		// Remove alDeleteAuxiliaryEffectSlot
		if ( GEffectsExtensionLoaded && alIsAuxiliaryEffectSlot(iEffectSlot))
			alDeleteAuxiliaryEffectSlots(1, &iEffectSlot);
#endif

		// Shutdown ALUT / ALURE
#ifdef ALURE
		alureShutdownDevice();
#else
		alutExit();
#endif

		// Shutdown soundsystem.
		if (context_id)
		{
			alcMakeContextCurrent(NULL);
			Device = alcGetContextsDevice(context_id);
			if (Device)
			{
				alcDestroyContext(context_id);
				alcCloseDevice(Device);				
			}
			context_id = NULL;
		}
		AudioLog(NAME_Exit, TEXT("ALAudio: Audio subsystem shut down."));

		// han: Set Initialized to zero?
	}
	Super::Destroy();

#ifdef EFX
	unguardf((TEXT("(Initialized=%i,EffectsExtensionLoaded=%i)"), Initialized, GEffectsExtensionLoaded));
#else
	unguardf((TEXT("(Initialized=%i)"), Initialized));
#endif
}

void UALAudioSubsystem::ShutdownAfterError()
{
	guard(UALAudioSubsystem::ShutdownAfterError);

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(WIN32)
	TimerEnd();
#endif

	// Unregister all music
#if !RUNE_CLASSIC && !UNREAL_TOURNAMENT_UTPG
	for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
		if (MusicIt->Handle)
			StopMusic(*MusicIt);
	MFrom = NULL;
	MTo = NULL;
#endif

	// Unhook.
	EndBuffering = TRUE; //Make sure to have no thread left buffering!!
	USound::Audio = NULL;
#if !RUNE_CLASSIC
	UMusic::Audio = NULL;
#endif

	// Safely shut down.
	AudioLog(NAME_Exit, TEXT("ALAudio subsystem shut down."));

	try
	{

#ifdef EFX
		// Remove alDeleteAuxiliaryEffectSlot
		if ( GEffectsExtensionLoaded && alIsAuxiliaryEffectSlot(iEffectSlot) )
			alDeleteAuxiliaryEffectSlots(1, &iEffectSlot);
#endif

		if (context_id)
		{
			ALCcontext* Context = alcGetCurrentContext();
			Device = alcGetContextsDevice(Context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(Context);
			alcCloseDevice(Device);
			context_id = NULL;
		}

		// Shutdown ALUT / ALURE
#ifdef ALURE
		alureShutdownDevice();
#else
		alutExit();
#endif
	}
	catch(...)
	{
		
	}

	AudioLog(NAME_Exit, TEXT("UALAudioSubsystem::ShutdownAfterError"));
	Super::ShutdownAfterError();

#ifdef EFX
	unguardf((TEXT("(Initialized=%i,EffectsExtensionLoaded=%i)"), Initialized, GEffectsExtensionLoaded));
#else
	unguardf((TEXT("(Initialized=%i)"), Initialized));
#endif
}

//
// Stop Sound support.
//
void UALAudioSubsystem::StopAllSound() // Rune.
{
	guard(UALAudioSubsystem::StopAllSound);
	check(ARRAY_COUNT(PlayingSounds) >= EffectsChannels); // More sanity checks. --han

	for (INT i = 0; i<EffectsChannels; i++)
		StopSound(i);

	unguard;
}

//
// I don't exactly know how this is supposed to work, but I implemented a scheme which sounds resonable for me. --han
//
//  * Stop everything if neither Actor nor Sound is specified.
//  * If just Actor is set, stop all sounds associated with this Actor.
//  * If just Sound is set, stop it for all Actors.
//  * If both Actor and Sound is set, require both to match.
//
UBOOL UALAudioSubsystem::StopSound(AActor* Actor, USound *Sound) // Rune Classic.
{
	guard(UALAudioSubsystem::StopSound);
	check(ARRAY_COUNT(PlayingSounds) >= EffectsChannels); // More sanity checks. --han
	debugf(NAME_DevAudio, TEXT("StopSound(Actor=%s,Sound=%)"), Actor->GetName(), Sound->GetName());

	INT Count = 0;
	for (INT i = 0; i<EffectsChannels; i++)
		if ((Actor == NULL || Actor == PlayingSounds[i].Actor) && (Sound == NULL || Sound == PlayingSounds[i].Sound))
			StopSound(i), Count++;

	return Count;
	unguardf((TEXT("(Actor=%s,Sound=%s)"), *FObjectFullName(Actor), *FObjectFullName(Sound)));
}
void UALAudioSubsystem::StopSoundId(INT Id) // DeusEx, Rune and Undying, NOT Rune Classic.
{
	guard(UALAudioSubsystem::StopSoundId);
	INT Index = INDEX_NONE;

	for (INT i = 0; i<EffectsChannels; i++)
	{
#if defined(UNDYING) // Undying ignores force play flag.
		if (PlayingSounds[i].Id == Id)
#else
		if ((PlayingSounds[i].Id&~1) == (Id&~1))
#endif
		{
			Index = i;
			break;
		}
	}
	if (Index != INDEX_NONE)
		StopSound(Index);

	unguardf((TEXT("(Id=%i)"), Id));
}

//
// Additional SoundId related interfaces.
//
UBOOL UALAudioSubsystem::SoundIdActive(INT Id) // Undying.
{
	guard(UALAudioSubsystem::SoundIdActive);
	check(sizeof(PlayingSounds) <= EffectsChannels); // More sanity checks. --han

	for (INT i = 0; i<EffectsChannels; i++)
#if defined(UNDYING) // Undying ignores force play flag.
		if (PlayingSounds[i].Id == Id)
#else
		if ((PlayingSounds[i].Id&~1) == (Id&~1))
#endif
			return 1;
	return 0;

	unguardf((TEXT("(Id=%i)"), Id));
}

// Non unicode verification.
#if defined(LEGACY_YELL_IF_UNICODE) && (defined(_UNICODE) || defined(UNICODE) )
#error "Your active configuration is set to UNICODE!"
#endif

/*-----------------------------------------------------------------------------

	Subsystem utils.

-----------------------------------------------------------------------------*/

AActor* UALAudioSubsystem::GetCameraActor()
{
	guard(UALAudioSubsystem::GetCameraActor);
	check(Viewport);
	check(Viewport->Actor);
#if RUNE || RUNE_CLASSIC //RUNE
	return Viewport->Actor;
#else
	return (Viewport->Actor->ViewTarget ? Viewport->Actor->ViewTarget : Viewport->Actor);
#endif
	unguard;
}

FVector UALAudioSubsystem::GetCameraLocation()
{
	guard(UALAudioSubsystem::GetCameraLocation);
#if RUNE || RUNE_CLASSIC //RUNE
	check(Viewport);
	check(Viewport->Actor);
	return Viewport->Actor->ViewLocation;
#else
	return LastRenderCoords.Origin + (GetCameraActor()->Location - LastViewerPos);
#endif
	unguard;
}

FLOAT UALAudioSubsystem::SoundPriority( FVector Location, FVector CameraLocation, FLOAT Volume, FLOAT Radius, INT Slot, FLOAT PriorityMultiplier)
{
	guard(UALAudioSubsystem::SoundPriority);
	FLOAT Priority = Volume;
	// stijn: we should use World instead of the slot for UT469
	if (Slot == SLOT_Interface)
		Priority *= 2.0;
	else if (Radius > 0.f)
		Priority *= 1.0f - FDist( Location, CameraLocation) / Radius;
	Priority *= PriorityMultiplier;
	return Priority;
	unguard;
}

FLOAT UALAudioSubsystem::AttenuationFactor( ALAudioSoundInstance& Playing)
{
	guard(UALAudioSubsystem::AttenuationFactor);

	// TODO: MOVE TO ABSTRACTION

	if ( !Playing.Actor )
		return Playing.AttenuationFactor;

	if ( !bSoundAttenuate || (Playing.SoundFlags & SF_NoFilter) )
		return 1.0;

	AActor* Listener = (Viewport && Viewport->Actor) ? GetCameraActor() : NULL;

	//Attenuate by visibility
	FLOAT Factor = 1.0;
	if ( Listener && Playing.Actor->Region.Zone && Playing.Actor->Region.ZoneNumber != 0 )
	{
		if ( Playing.Actor->IsBrush() )
		{
			//TODO
		}
		else if ( Listener && !FastLineCheck( Listener->GetLevel()->Model, GetCameraLocation(), Playing.Location))
			Factor = 0.8f;
	}

	// Use Water zone's ViewFog to see how much sound must be attenuated
	// Designed like this to not affect DM-Pyramid's NitrogenZone.
	// Use Head region for listener and pawns emitting talk/pain sounds.

	// Attenuate by emitter water zone
	if ( Playing.SoundFlags & SF_WaterEmission )
	{
		INT Slot = Playing.GetSlot();
		AZoneInfo* Zone = GetRegion( Playing.Actor, Slot==SLOT_Talk||Slot==SLOT_Pain ).Zone;
		if ( Zone )
			Factor /= 1.0f + Max(Zone->ViewFog.X + Zone->ViewFog.Y + Zone->ViewFog.Z, 0.f) * 1.5;
	}

	//Attenuate by listener water zone
	if ( Listener )
	{
		AZoneInfo* Zone = GetRegion( Listener, 1).Zone;
		if ( Zone && Zone->bWaterZone )
			Factor /= 1.0f + Max(Zone->ViewFog.X + Zone->ViewFog.Y + Zone->ViewFog.Z, 0.f) * 3.0;
	}

	return Factor;

	unguard;
}

FLOAT UALAudioSubsystem::UpdateTime()
{
	FLOAT DeltaTime;
	FTIME NewTime;
#if defined(LEGACY_TIME) || defined(UNREAL_TOURNAMENT)
	NewTime = appSecondsAudio();
#else
	// Han: Either use DOUBLE for FTime games too or make consequent use of FTime (TransitionStartTime, etc.)
	NewTime = appSeconds();
#endif
	DeltaTime = Clamp( (FLOAT)(NewTime-LastTime), 0.f, 0.4f);
	LastTime = NewTime;
	return DeltaTime;
}


/*-----------------------------------------------------------------------------
Rune Classic Raw Streaming.

Todo:
* Figure out which alSource* stuff needs to be set!
-----------------------------------------------------------------------------*/

#if RUNE_CLASSIC

#if 1
#define rawf debugf
#else
#define rawf
#endif

#define DESIRED_RAW_POOL_SIZE 8 // Could actually be sth. like 32 until we figure out how to not get so much data at once.

void UALAudioSubsystem::InitRaw()
{
	guard(UALAudioSubsystem::InitRaw);
	IsRawStreaming = IsRawError = 0;
	RawStreamStartTime = 0.f;
	ALuint RawSource = 0;
	unguard;
}

void UALAudioSubsystem::UpdateRaw()
{
	guard(UALAudioSubsystem::UpdateRaw);
	if (!IsRawStreaming)
		return;

	// Update MusicVolume.
	alSourcef(RawSource, AL_GAIN, MusicVolume / 255.f);
	unguard;
}

UBOOL UALAudioSubsystem::ResizeRawChunkBufferPool(INT Desired)
{
	guard(UALAudioSubsystem::ResizeRawChunkBufferPool);
	INT OldPoolCount = RawChunkBufferPool.Num();

	// Clear Error.
	alGetError();

	// Enlarge.
	if (RawChunkBufferPool.Num()<Desired)
	{
		guard(Enlarge);
		ALuint* Buffers = (ALuint*)appAlloca(Desired*sizeof(ALuint*));
		INT NumBuffers = Desired - RawChunkBufferPool.Num();
		alGenBuffers(NumBuffers, &Buffers[RawChunkBufferPool.Num()]);
		if (alGetError() != AL_NO_ERROR)
		{
			debugf(NAME_DevMusic, TEXT("Failed to create buffer for raw stream pool. "));
			return 0;
		}
		for (INT i = RawChunkBufferPool.Num(); i<Desired; i++)
			RawChunkBufferPool.AddItem(Buffers[i]);
		rawf(NAME_DevMusic, TEXT("Generated %i chunk buffers."), NumBuffers);
		unguard;
	}
	// Shrink.
	else if (RawChunkBufferPool.Num()>Desired)
	{
		guard(Shrink);
		alDeleteBuffers(RawChunkBufferPool.Num() - Desired, &RawChunkBufferPool(Desired));
		RawChunkBufferPool.Remove(Desired, RawChunkBufferPool.Num() - Desired);
		if (alGetError() != AL_NO_ERROR)
		{
			debugf(NAME_DevMusic, TEXT("Encountered error while shrinking raw stream pool. "));
			return 0;
		}
		rawf(NAME_DevMusic, TEXT("Deleted %i chunk buffers."), OldPoolCount - RawChunkBufferPool.Num());
		unguard;
	}
	return 1;
	unguardf((TEXT("(Desired=%i)"), Desired));
}

INT UALAudioSubsystem::RegainChunkBufferPool()
{
	guard(UALAudioSubsystem::RegainChunkBufferPool);
	INT OldPoolSize = RawChunkBufferPool.Num();
	ALint BuffersProcessed;
	alGetSourcei(RawSource, AL_BUFFERS_PROCESSED, &BuffersProcessed);
	if (BuffersProcessed>0)
	{
		check(BuffersProcessed <= RawChunkBuffers.Num());
		for (INT i = 0; i<BuffersProcessed; i++)
			RawChunkBufferPool.AddItem(RawChunkBuffers(i));

		// Unqueue.
		alSourceUnqueueBuffers(RawSource, BuffersProcessed, &RawChunkBuffers(0));
		RawChunkBuffers.Remove(0, BuffersProcessed);
	}
	rawf(NAME_DevMusic, TEXT("Regained %i chunk buffers."), RawChunkBufferPool.Num() - OldPoolSize);
	return BuffersProcessed;
	unguard;
}

void UALAudioSubsystem::RawSamples(FLOAT* Data, INT NumSamples, INT NumChannels, INT SampleRate)
{
	guard(UALAudioSubsystem::RawSamples);
	rawf(NAME_DevMusic, TEXT("RawSamples(Data=0x%08x,NumSamples=%i,NumChannels=%i,SampleRate=%i)"), Data, NumSamples, NumChannels, SampleRate);
	check(Data);
	check(NumSamples>0);
	check(SampleRate>0);
	if (IsRawError)
		return;
	if (NumChannels<1 || NumChannels>2)
	{
		debugf(NAME_DevMusic, TEXT("Only mono and stereo raw stream is supported (NumChannels=%i). "), NumChannels);
		return;
	}

	// Clear Error.
	alGetError();

	//new(RawChunkBuffers) FRawChunk( Data, NumSamples, NumChannels, SampleRate );

	// Get a chunk.
	ALuint ChunkBuffer = 0;
	if (RawChunkBufferPool.Num() || RegainChunkBufferPool()>0 || ResizeRawChunkBufferPool(DESIRED_RAW_POOL_SIZE))
		ChunkBuffer = RawChunkBufferPool.Pop();
	else
		return;
	RawChunkBuffers.AddItem(ChunkBuffer);

	// Clamp.
	guard(Buffer);
#if 1
	// Okay now finally works, but keep the stuff below in case we need that if float format is not preset.
	FLOAT* ResampledData = new FLOAT[NumSamples];
	for (INT i = 0; i<NumSamples; i++)
		ResampledData[i] = Clamp(Data[i], -1.f, +1.f);
	// Buffer.
	alBufferData(ChunkBuffer, NumChannels == 2 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32, Data, NumSamples*sizeof(FLOAT), SampleRate);
	delete ResampledData;
#else
	// Sample to SWORD and buffer. Smirftsch always says he likes the SWORD type...
	SWORD* ResampledData = new SWORD[NumSamples/**NumChannels*/]; //
	for (INT i = 0; i<NumSamples/**NumChannels*/; i++)
		ResampledData[i] = Clamp(Data[i], -1.f, +1.f)*32767.0;
	alBufferData(ChunkBuffer, NumChannels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, ResampledData, NumSamples*sizeof(SWORD)/*NumChannels*/, SampleRate);
	delete ResampledData;
#endif
	if (alGetError() != AL_NO_ERROR)
	{
		debugf(NAME_DevMusic, TEXT("Failed to buffer data for raw stream. "));
		return;
	}
	alSourceQueueBuffers(RawSource, 1, &ChunkBuffer);
	if (alGetError() != AL_NO_ERROR)
	{
		debugf(NAME_DevMusic, TEXT("Failed to create queue buffer for raw stream. "));
		return;
	}
	unguard;

	// Start (again) if needed.
	ALint SourceState = 0;
	alGetSourcei(RawSource, AL_SOURCE_STATE, &SourceState);
	if (SourceState != AL_PLAYING)
		alSourcePlay(RawSource);
	unguard;
}
// FIX-ME: Figure out what needs to be returned here so we don't get a shitload of Chunks at once, or copy them asside and buffer them once a threshold is reached or Update() is called.
FLOAT UALAudioSubsystem::GetRawTimeOffset(FLOAT InOffset) // Looks like this is used to determine whether we need new samples.
{
	guard(UALAudioSubsystem::GetRawTimeOffset);
	rawf(NAME_DevMusic, TEXT("GetRawTimeOffset(InOffset=%f)"), InOffset);
	//static FLOAT Bla=0.f;
	if (IsRawStreaming)
		return (appSecondsAudio() - RawStreamStartTime) + InOffset;
	//return Latency/1000.f+InOffset;
	//return (Bla-=100.f);
	else
		return 0.f;
	unguard;
}
void UALAudioSubsystem::StopRawStream()
{
	guard(UALAudioSubsystem::StopRawStream);
	rawf(NAME_DevMusic, TEXT("(RawChunkBuffers#=%i,RawChunkBufferPool#=%i,IsRawError=%i)"), RawChunkBuffers.Num(), RawChunkBufferPool.Num(), IsRawError);
	if (IsRawError)
		return;

	debugf(NAME_DevMusic, TEXT("Stopping raw stream."));
	check(IsRawStreaming);

	// Delete source first, otherwise unqueueing fails and thus deletion. --han
	alDeleteSources(1, &RawSource);
	RawSource = 0;

	// Unqueue all buffers and add to pool.
	/*if ( RawChunkBuffers.Num() )
	{
	alSourceUnqueueBuffers( RawSource, RawChunkBuffers.Num(), &RawChunkBuffers(0) );
	if ( alGetError()!=AL_NO_ERROR )
	{
	debugf( NAME_DevMusic, TEXT("Failed to unqueue remaining raw chunks. ") );

	// Just delete now.
	alDeleteBuffers( RawChunkBuffers.Num(), &RawChunkBuffers(0) );
	RawChunkBuffers.Empty();
	}
	else
	{
	while ( RawChunkBuffers.Num() )
	RawChunkBufferPool.AddItem(RawChunkBuffers.Pop());
	}
	}*/

	// Resize Pool to desired size.
	if (!ResizeRawChunkBufferPool(DESIRED_RAW_POOL_SIZE))
		debugf(NAME_DevMusic, TEXT("Failed to resize raw queue buffer pool to desired size."));

	// Exit if not RawStreaming.
	if (!IsRawStreaming)
		return;

	IsRawStreaming = 0;
	unguard;
}

void UALAudioSubsystem::BeginRawStream(INT NumChannels, INT SampleRate)
{
	guard(UALAudioSubsystem::BeginRawStream);
	if (IsRawError)
	{
		debugf(NAME_DevMusic, TEXT("Cannot begin raw stream due to by prior raw streaming errors."));
		return;
	}

	// Check for AL_EXT_float32 presense. TODO: Move this check somewhere else and cache the result. Or even do some OpenGLDrv style.
	UBOOL SUPPORTS_AL_EXT_float32 = alIsExtensionPresent("AL_EXT_float32"); // Playback seems to be all fucked up, but I have not yet an idea why.
	if (!SUPPORTS_AL_EXT_float32)
	{
		// In fact I could just resample to a required to exist OpenAL 1.1 format, but...
		debugf(NAME_DevMusic, TEXT("Cannot begin raw stream: AL_EXT_float32 extension is missing."));
		return;
	}

	debugf(NAME_DevMusic, TEXT("Beginning raw stream (%i:%i)"), NumChannels, SampleRate);
	check(!IsRawStreaming);
	check(RawChunkBuffers.Num() == 0);

	// Clear error.
	alGetError();

	// Generate source.
	alGenSources(1, &RawSource);
	if (alGetError() != AL_NO_ERROR)
	{
		debugf(NAME_DevMusic, TEXT("Failed to create source for raw stream. "));
		return;
	}

	// Enlarge or shrink Pool if required.
	if (!ResizeRawChunkBufferPool(DESIRED_RAW_POOL_SIZE))
	{
		IsRawError = 1;
		return;
	}

	RawStreamStartTime = appSecondsAudio();
	IsRawStreaming = 1;

	// A bit odd, but it sets MusicVolume.
	UpdateRaw();
	unguard;
}

#endif // RUNE_CLASSIC.
UBOOL					UALAudioSubsystem::EndBuffering     = FALSE;

/*-----------------------------------------------------------------------------
The End.
-----------------------------------------------------------------------------*/
