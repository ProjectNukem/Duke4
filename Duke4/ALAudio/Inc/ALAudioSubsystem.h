/*=============================================================================
ALAudioSubsystem.cpp: Unreal OpenAL Audio interface object.
Copyright 2015 Oldunreal.

Revision history:
* Old initial version by Joseph I. Valenzuela (based on AudioSubsystem.cpp)
* Heavily modified and completely rewritten by Jochen 'Smirftsch' Görnitz
* Porting and porting help for other UEngine1 games, code cleanups, efx refactoring and lipsynch by Sebastian 'Hanfling' Kaufel
=============================================================================*/

#define AL_ALEXT_PROTOTYPES 1

#include <stdio.h>
#if MACOSX
# include "al.h"
# include "alc.h"
# include "efx.h"
# include "alext.h"
#else
# include <AL/al.h>
# include <AL/alc.h>
# include <AL/efx.h>
# include <AL/alext.h>
#endif

#ifdef ALURE
# define ALURE_STATIC_LIBRARY
# if MACOSX
#  include "alure.h"
# else
#  include "AL/alure.h"
# endif
#else
# include <AL/alut.h>
#endif

#include <math.h>
#include "ALAudioTypes.h"
#include "EFX-Util.h"
// libxmp music
#include "xmp.h"
// ogg music
#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"

#define TRUE 1
#define FALSE 0
#define PlayBuffer 1

#ifdef __GNUG__
#include <unistd.h>
#endif

#if DNF
/*-----------------------------------------------------------------------------
	MSS MP3 Integration:
-----------------------------------------------------------------------------*/
#include "../../Galaxy/Src/Mss.h"
#pragma comment(lib,"../../Galaxy/Src/Mss32.lib")
#endif

/*------------------------------------------------------------------------------------
Helpers
------------------------------------------------------------------------------------*/

// Constants.
#define MAX_EFFECTS_CHANNELS 256
#define EFFECT_FACTOR        1.0
#define AMBIENT_FACTOR       0.7
#define MUSIC_BUFFERS		 4
#define MUSIC_BUFFER_SIZE	 16384
#define SOUND_BUFFER_SIZE	 4096
#define CPP_PROPERTY_LOCAL(name) name, CPP_PROPERTY(name)

// Use 30 Hz as in stock Deus Ex for now, make it configurable later, bla bla yak yak.
#define LIPSYNCH_FREQUENCY 30.f

// Utility Macros.
#define safecall(f) \
{ \
	guard(f); \
	INT Error=f; \
	if( Error==0 ) \
		debugf( NAME_Warning, TEXT("%s failed: %i"), TEXT(#f), Error ); \
	unguard; \
}
#define silentcall(f) \
{ \
	guard(f); \
	f; \
	unguard; \
}

#ifndef FASTCALL
# ifdef _WIN32
#  define FASTCALL	__fastcall
# else
#  define FASTCALL __attribute__((fastcall))
# endif
#endif

typedef struct _memory_ogg_file
{
	BYTE* curPtr;
	BYTE* filePtr;
	SIZE_T fileSize;
} memory_ogg_file;


/*------------------------------------------------------------------------------------
OpenAL handles.
------------------------------------------------------------------------------------*/

extern UBOOL GFilterExtensionLoaded;
extern UBOOL GEffectsExtensionLoaded;
extern UBOOL GOpenALSOFT;

#if !AL_ALEXT_PROTOTYPES
	// Effect objects
	extern LPALGENEFFECTS alGenEffects;
	extern LPALDELETEEFFECTS alDeleteEffects;
	extern LPALISEFFECT alIsEffect;
	extern LPALEFFECTI alEffecti;
	extern LPALEFFECTIV alEffectiv;
	extern LPALEFFECTF alEffectf;
	extern LPALEFFECTFV alEffectfv;
	extern LPALGETEFFECTI alGetEffecti;
	extern LPALGETEFFECTIV alGetEffectiv;
	extern LPALGETEFFECTF alGetEffectf;
	extern LPALGETEFFECTFV alGetEffectfv;

	// Filter objects
	extern LPALGENFILTERS alGenFilters;
	extern LPALDELETEFILTERS alDeleteFilters;
	extern LPALISFILTER alIsFilter;
	extern LPALFILTERI alFilteri;
	extern LPALFILTERIV alFilteriv;
	extern LPALFILTERF alFilterf;
	extern LPALFILTERFV alFilterfv;
	extern LPALGETFILTERI alGetFilteri;
	extern LPALGETFILTERIV alGetFilteriv;
	extern LPALGETFILTERF alGetFilterf;
	extern LPALGETFILTERFV alGetFilterfv;

	// Auxiliary slot object
	extern LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
	extern LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
	extern LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
	extern LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
	extern LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
	extern LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
	extern LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
	extern LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
	extern LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
	extern LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
	extern LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;
#endif

#if !AL_ALEXT_PROTOTYPES
	// OpenALSoft specific extensions.
	extern LPALBUFFERSAMPLESSOFT alBufferSamplesSOFT;
	extern LPALBUFFERSUBSAMPLESSOFT alBufferSubSamplesSOFT;
	extern LPALISBUFFERFORMATSUPPORTEDSOFT alIsBufferFormatSupportedSOFT;
	extern LPALGETBUFFERSAMPLESSOFT alGetBufferSamplesSOFT;
#endif




/*------------------------------------------------------------------------------------
PlayingSounds and the associated data type are used in the queue of playing sounds.
------------------------------------------------------------------------------------*/

enum ESoundFlags
{
	SF_AmbientSound    = 0x00000001, //Automatically played ambient sound
	SF_LoopingSource   = 0x00000002,
	SF_Speech          = 0x00000004,
	SF_No3D            = 0x00000008,
	SF_NoFilter        = 0x00000010,
	SF_WaterEmission   = 0x00000020,

	SF_Loop            = SF_AmbientSound | SF_LoopingSource,
};

struct ALAudioSoundInstance
{
	AActor *Actor;
	USound *Sound;
	FLOAT Priority;
	INT Id;
	ALuint SourceID;
	FLOAT Volume;
	FLOAT MaxVolume;
//protected: // Higor: Used to move gradually stuff to abstraction, remove in release builds.
	FLOAT Radius;
	FLOAT Pitch;
	FLOAT DopplerFactor;
	FLOAT AttenuationFactor;
	FVector Location;
	FVector Velocity;
//public:
	DWORD SoundFlags;
	INT LastAudioOffset;
	FLOAT LipsynchTimer;
	FLOAT PriorityMultiplier;
#if DNF
	UBOOL IsMonitoredSound;
#endif

	struct UpdateParams
	{
		AActor* ListenerActor;
		class UALAudioSubsystem* AudioSub;
		FLOAT DeltaTime;
		UBOOL Realtime;
		UBOOL LightRealtime;
	};

	// Initialize default state.
	void Init();

	// Tick
	INT Update( const ALAudioSoundInstance::UpdateParams& Params);

	// Sound control
	bool IsPlaying();
	void Stop();

	// Modify sound state.
	void SetRadius( FLOAT NewRadius);
	void SetPitch( FLOAT NewPitch);
	void SetDopplerFactor( FLOAT NewDopplerFactor);
	void SetVolume( FLOAT NewVolume, FLOAT NewMaxVolume);
	void SetLocation( FVector NewLocation);
	void SetVelocity( FVector NewVelocity);
	void SetEFX( ALuint NewEffectSlot, INT FilterType);

	// High level routines (should be moved back to the subsystem)
	void UpdateEmission( const FCoords& ListenerCoords, UBOOL LightRealtime);
	void UpdateVolume( UALAudioSubsystem* AudioSub);
	void UpdateAttenuation( FLOAT NewAttenuationFactor, FLOAT DeltaTime=-1);
#if DNF
	void UpdateMonitoredSound( INT SampleSize=1024 );
#endif

	FString GetSoundInformation( UBOOL Detail);

	bool IsAmbient();

	// Inlines
	struct ALAudioSoundHandle* GetHandle() const;
	INT GetSlot() const;

private:
	void ProcessLoop();
};

struct ALAudioSoundHandle
{
	BYTE bLoopingSample;
	INT LoopStart, LoopEnd;
	ALuint ID;
	ALuint Filter;
	FLOAT Rate;
	FLOAT Duration;

	ALAudioSoundHandle()
		: bLoopingSample(0)
		, ID(0)
		, Filter(0)
		// Han: Initialize LoopStart and LoopEnd instead of relying on a weired function
		//      to set them and rely upon noone forgets to check bLoopingSample
		, LoopStart(0)
		, LoopEnd(0)
		, Rate(0)
		, Duration(0)
	{}
};
struct ALAudioMusicHandle
{
	xmp_context xmpcontext;
	void*	xmpBuffer;
	UBOOL   IsPlaying, EndPlaying, IsStarting, IsOgg;
	ALuint	musicbuffers[MUSIC_BUFFERS];
	ALuint	musicsource, format;
	ALint   BufferError;
	ALsizei	SampleRate; //no really need to set it here, but dislike having more statics.
	INT		SongSection;
	FString MusicType, MusicTitle;
	OggVorbis_File* OggStream;
	vorbis_info*    vorbisInfo;
	vorbis_comment* vorbisComment;
	memory_ogg_file MemOggFile;
	FRunnable* BufferingRunnable;
	FRunnableThread* BufferingThread;

	ALAudioMusicHandle()
		: xmpcontext(NULL)
		, xmpBuffer(NULL)
		, IsPlaying(0)
		, EndPlaying(0)
		, IsStarting(0)
		, IsOgg(0)
		, SongSection(0)
		, musicsource(0)
		, SampleRate(0)
		, BufferError(0)
		, format(AL_FORMAT_STEREO16)
		, MusicType(TEXT(""))
		, MusicTitle(TEXT(""))
		, BufferingRunnable(nullptr)
		, BufferingThread(nullptr)
	{}
};

#define bound(value,min,max) ((value)<(min)?(min):((value)>(max)?(max):(value)))

#ifdef EFX
// Struct used for querying EFX settings.
struct FEFXEffects
{
	INT Version;
	BYTE ReverbPreset; // Called Ambients in 227.
	FLOAT AirAbsorptionGainHF;
	FLOAT DecayHFRatio;
	FLOAT DecayLFRatio;
	FLOAT DecayTime;
	FLOAT Density;
	FLOAT Diffusion;
	FLOAT EchoDepth;
	FLOAT EchoTime;
	FLOAT Gain;
	FLOAT GainHF;
	FLOAT GainLF;
	FLOAT HFReference;
	FLOAT LFReference;
	FLOAT LateReverbDelay;
	FLOAT LateReverbGain;
	FLOAT RoomRolloffFactor;
	BITFIELD bUserDefined : 1 GCC_ALIGN(4);
	BITFIELD bDecayHFLimit : 1;
	INT Reserved[32] GCC_ALIGN(4);
};
#endif

/*------------------------------------------------------------------------------------
	UALAudioSubsystem.
------------------------------------------------------------------------------------*/

#if ENGINE_VERSION == 469 || ENGINE_VERSION == 470
# define UAudioSubsystemType UAudioSubsystemOldUnreal469
#else
# define UAudioSubsystemType UAudioSubsystem
#endif


//
// The Generic implementation of UAudioSubsystem.
//
class DLL_EXPORT_CLASS UALAudioSubsystem : public UAudioSubsystemType
#if defined(USE_UNEXT)
	, public IHumanHeadAudioSubsystem
#endif
{
	DECLARE_CLASS_AUDIO(UALAudioSubsystem, UAudioSubsystemType, CLASS_Config, ALAudio)

	// Configuration.
	BITFIELD		Initialized;
	UBOOL			UseDigitalMusic;
	UBOOL			ProbeDevicesOnly;
	UBOOL			AudioStats;
	UBOOL			DetailStats;
	UBOOL			EffectsStats;
#ifdef EFX
	UBOOL			UseReverb;
#endif
	UBOOL			UseAmbientPresets;
	UBOOL			bSoundAttenuate;
	UBOOL			UseHardwareChannels;
	UBOOL			UseSpeechVolume;
	INT				MusicPanSeparation;	// XMP default Pan separation
	INT				MusicStereoMix;		// XMP Stereo channel separation
	INT				MusicStereoAngle;
	INT				MusicAmplify;		// XMP Amplification factor: 0=Normal, 1=x2, 2=x4, 3=x8
	INT				Latency;
	INT				EffectsChannels;
	INT				FirstChannel;
	INT				LastChannel;
	BYTE			OutputRateNum;	// General output rate for OpenAL
	BYTE			SampleRateNum;	// SampleRate for XMP Music
	ALsizei			OutputRate;
	ALsizei			SampleRate;
	BYTE			MusicVolume;
	BYTE			SoundVolume;
	BYTE			SpeechVolume;
	BYTE			MusicInterpolation;	// XMP Interpolation type
	BYTE			MusicDsp;			// XMP dsp effects
	DWORD			OutputMode;
	FLOAT			ReverbFactor;
	FLOAT			DopplerFactor;
	UBOOL			UseAutoSampleRate;

	// for Alut and buffering
	ALsizei size;
	ALsizei length;

	// OpenAL Error
	ALint	error;

	// Device selector
	FStringNoInit PreferredDevice;
	TArray<FString> DetectedDevices;
	INT CurrentDevice; // -1 if default

	//HRTF
	BYTE	UseHRTF;

	// Environment configuration variables
	ALubyte szFnName[128];
	UBOOL ReverbZone;
	UBOOL RaytraceZone;
	UBOOL EffectSet;
	ALfloat speedofsound;
	FLOAT ReverbHFDamp;
	BYTE DelayCompare;
	BYTE fog;
	ALuint	Env;
	FLOAT ViewportVolumeIntensity;

#ifdef EFX
	UBOOL EmulateOldReverb;
	FLOAT OldReverbIntensity;
	FLOAT ReverbIntensity;
#endif

	// CombFilter to EFX
	FLOAT RoomSize;
	FLOAT MaxDelay;
	FLOAT MaxGain;
	FLOAT MinDelay;
	FLOAT AvgDelay;
	FLOAT DelayTime[6];
	FLOAT DelayGain[6];
	FLOAT DHFR1;
	FLOAT T;

	INT zonenumber;
	INT zonecompare;
	INT	zoneset;

	ALuint		sid;
	ALuint		bid;
	ALuint      iEffectSlot;

	//EAXZone stuff
	INT			unset;
	INT			unsetinit;
	UBOOL		bSpecialZone;

	ALfloat		intenvironmentsize;
	ALfloat		intenvironmentdiffusion;
	ALfloat		obstructionlfratio;
	ALint		obstruction;
	ALint		introom;
	ALfloat		airabsorbtionhf;
	ALfloat		Angles[2];

	//Music
	struct		xmp_frame_info xmpfi;
	struct		xmp_module_info xmpmi;
	ov_callbacks callbacks;
	static UBOOL EndBuffering;

	// Filter
	UBOOL LoadFilterExtension();

#ifdef EFX
	// EFX Helper
	UBOOL ConditionalLoadEffectsExtension();
	UBOOL QueryEffects(FEFXEffects& Effects, AZoneInfo* Zone, AActor* ViewActor);
#endif

	// Variables.
	UViewport*		Viewport;
	FTIME			LastTime;
#if !RUNE_CLASSIC
	UMusic*			CurrentMusic;
#endif
	BYTE			CurrentCDTrack;
	FLOAT			MusicFade;
	INT				FreeSlot;
	ALCdevice		*Device;
	ALCcontext		*context_id;
	AZoneInfo*		OldAssignedZone;
	BYTE			Ambient;
	BYTE			bPlayingAmbientSound;
	ALAudioSoundInstance PlayingSounds[MAX_EFFECTS_CHANNELS];

	// Listener state
	FCoords LastRenderCoords;
	FVector LastViewerPos;

	// OldUnreal 469 compatibility handling
	DWORD			CompatibilityFlags;

	// Music transition variables
	UBOOL Transiting, CrossFading;
	FLOAT TransitionStartTime, MusicFadeInTime, MusicFadeOutTime;
#if !RUNE_CLASSIC
	UMusic *MFrom, *MTo;
#endif

	LONG LastDeviceChangeCounter;
	FRunnable* DeviceMonitoringRunnable;
	FRunnableThread* DeviceMonitoringThread;
	FRunnable* UpdateBufferRunnable;
	FRunnableThread* UpdateBufferThread;


#if RUNE_CLASSIC
	// Rune Classic Raw Streaming variables.
	TArray<ALuint> RawChunkBuffers; // Buffers in use.
	TArray<ALuint> RawChunkBufferPool; // Pool for buffers not currently in use.
	UBOOL IsRawStreaming;
	FLOAT RawStreamStartTime;
	UBOOL IsRawError;
	ALuint RawSource;

	// Rune Classic Raw Streaming helper.
	void InitRaw();
	void UpdateRaw();
	INT RegainChunkBufferPool();
	UBOOL ResizeRawChunkBufferPool(INT Desired);

#endif

#if DNF
	// Music Functions and Members:
	HDIGDRIVER DigitalDriver;
	HMDIDRIVER MidiDriver;
	HDLSDEVICE DLSDevice;
	HSTREAM			CurrentStream;
	HSTREAM			PendingStream;
	TArray<FString>	LevelFilenames;
	double			CrossfadeStart;
	double			CrossfadeLength;
	FString CurrentLevelPrefix;
	TArray<FString>	NextJumpPattern;
	UBOOL			bInstant;
#endif

	// Static.
#if DNF
	static class URender* __Render;
#else
	static class URenderBase* __Render;
#endif

	// Constructor.
	UALAudioSubsystem();
	SC_MODIFIER void StaticConstructor(SC_PARAM);

	// FUnknown interface.
#if defined(USE_UNEXT)
	DWORD STDCALL QueryInterface(const FGuid& RefIID, void** InterfacePtr);
#endif

	// UObject interface.
	void Destroy();
	void PostEditChange();
	void ShutdownAfterError();

	// Configuration helpers.
	void SetDefaultValues();

	// UAudioSubsystem interface.
	void InitAttrList(ALCint* AttrList);
	UBOOL Init();
	void SetViewport(UViewport* Viewport);
	UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar); // Han: Removed default argument for KHG.
	void Update(FPointRegion Region, FCoords& Coords);
	void UnregisterSound(USound* Sound);
#if !RUNE_CLASSIC
	void RegisterMusic(UMusic* Music);
	void UnregisterMusic(UMusic* Music);
	void StopMusic(UMusic* Music);
#endif
	UBOOL StartMusic(ALAudioMusicHandle* MusicHandle);
#if DNF
	void MusicPlay(TCHAR* Prefix, TCHAR* Filename, UBOOL Instant = 0, FLOAT CrossfadeTime = 0.0f, UBOOL Push = 0);
	void MusicUpdate();
	void MusicComputeCrossfadeStart();
#endif
#if !UNREAL_TOURNAMENT_OLDUNREAL
	UBOOL PlaySound(AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch);
#if DNF
	UBOOL PlaySound(AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch, UBOOL IsMonitoredSound); // Duke Nukem Forever.
#else
	UBOOL PlaySound(AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch, INT _Unknown) // Rune Classic.
	{
		guard(UALAudioSubsystem::PlaySound);
		// Perform some logging to get an idea about the _Unknown parameter.
#if 0 // _Unknown seems to be usually 0. Investigate this later, but remove the logspam for now.
		debugf(NAME_DevAudio, TEXT("PlaySound(Actor=%s,Id=%i,Sound=%s,Location=(%f,%f,%f),Volume=%f,Radius=%f,Pitch=%f,_Unknown=%i)"), Actor->GetName(), Id, Sound->GetName(), Location.X, Location.Y, Location.Z, Volume, Radius, Pitch, _Unknown);
#endif
		return PlaySound(Actor, Id, Sound, Location, Volume, Radius, Pitch);
		unguard;
	}
#endif
#endif
	void NoteDestroy(AActor* Actor);
	void RegisterSound(USound* Sound);
	UBOOL GetLowQualitySetting() { return 0; }
	UViewport* GetViewport();
	void RenderAudioGeometry(FSceneNode* Frame);
	void PostRender(FSceneNode* Frame);
	void Flush();
	void StopSound(INT Index);

	// Magic interface.
	void Lipsynch(ALAudioSoundInstance& Playing);

	// Stop Sound support.
	void StopAllSound(); // Rune, Rune Classic.
	void StopSoundId(INT Id); // Deus Ex, Rune.
	UBOOL StopSound(AActor* Actor, USound *Sound); // Rune Classic.
	void StopSoundBySlot(AActor* Actor, INT Index); // Duke Nukem Forever.

	// Additional SoundId related interfaces.
	UBOOL SoundIdActive(INT Id);
	UBOOL SoundIDActive(INT Id) { return SoundIdActive(Id); }; // Undying.

	// DeusEx special stuff...
#if defined(INCLUDE_AUDIOSUBSYSTEM_SETINSTANT)
	void SetInstantSoundVolume(BYTE newSoundVolume)    { SoundVolume = newSoundVolume; }; // DEUS_EX CNN - instantly sets the sound volume
	void SetInstantSpeechVolume(BYTE newSpeechVolume)  { SpeechVolume = newSpeechVolume; }; // DEUS_EX CNN - instantly sets the speech volume
	void SetInstantMusicVolume(BYTE newMusicVolume)    { MusicVolume = newMusicVolume; }; // DEUS_EX CNN - instantly sets the music volume
#endif

	// Rune Classic Raw Streaming.
#if defined(RUNE_CLASSIC)
	void RawSamples(FLOAT* Data, INT NumSamples, INT NumChannels, INT SampleRate);
	FLOAT GetRawTimeOffset(FLOAT InOffset);
	void StopRawStream();
	void BeginRawStream(INT NumChannels, INT SampleRate);
#endif

#if ENGINE_VERSION==420 || RUNE_CLASSIC
	void CleanUp()
	{
		debugf(NAME_DevAudio, TEXT("CleanUp()"));
		Super::CleanUp();
	};
#endif

	// UAudioSubsytemOldUnreal469 interface.
#if DNF
	UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FVector Velocity, FLOAT Volume, FLOAT Radius, FLOAT Pitch, FLOAT Priority, UBOOL World, UBOOL IsMonitoredSound);
#else
	UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FVector Velocity, FLOAT Volume, FLOAT Radius, FLOAT Pitch, FLOAT Priority, UBOOL World);
#endif
	void Update( ELevelTick TickType, FLOAT DeltaTime, FPointRegion ListenerRegion, FCoords& ListenerCoords, FVector ListenerVelocity, AActor* ListenerActor);
	void NoteDestroy( AActor* Actor, DWORD SlotStopMask);
	void SetViewport(UViewport* InViewport, DWORD InCompatibilityFlags);
//	UBOOL StopSound( AActor* Actor, USound* Sound); //Defined above
//	void StopSoundId( INT Id); //Defined above

	// Names.
	static void RegisterNames();

	// Utils
	AActor* GetCameraActor(); //Not precise
	FVector GetCameraLocation(); //Not precise
	FLOAT SoundPriority( FVector Location, FVector CameraLocation, FLOAT Volume, FLOAT Radius, INT Slot, FLOAT PriorityMultiplier);
	FLOAT AttenuationFactor( ALAudioSoundInstance& Playing);
	FLOAT UpdateTime(); //Returns DeltaTime

	// Inlines.
	Sample* GetSound(USound* Sound)
	{
		check(Sound);
		if (!Sound->Handle)
			RegisterSound(Sound);
		return (Sample*)Sound->Handle;
	}

	// from alhelper, thanks Chris.
	ALsizei FramesToBytes(ALsizei size, ALenum channels, ALenum type)
	{
		switch (channels)
		{
		case AL_MONO_SOFT:    size *= 1; break;
		case AL_STEREO_SOFT:  size *= 2; break;
		case AL_REAR_SOFT:    size *= 2; break;
		case AL_QUAD_SOFT:    size *= 4; break;
		case AL_5POINT1_SOFT: size *= 6; break;
		case AL_6POINT1_SOFT: size *= 7; break;
		case AL_7POINT1_SOFT: size *= 8; break;
		}
		switch (type)
		{
		case AL_BYTE_SOFT:           size *= sizeof(ALbyte); break;
		case AL_UNSIGNED_BYTE_SOFT:  size *= sizeof(ALubyte); break;
		case AL_SHORT_SOFT:          size *= sizeof(ALshort); break;
		case AL_UNSIGNED_SHORT_SOFT: size *= sizeof(ALushort); break;
		case AL_INT_SOFT:            size *= sizeof(ALint); break;
		case AL_UNSIGNED_INT_SOFT:   size *= sizeof(ALuint); break;
		case AL_FLOAT_SOFT:          size *= sizeof(ALfloat); break;
		case AL_DOUBLE_SOFT:         size *= sizeof(ALdouble); break;
		}
		return size;
	}
	ALsizei BytesToFrames(ALsizei size, ALenum channels, ALenum type)
	{
		return size / FramesToBytes(1, channels, type);
	}
};


/*------------------------------------------------------------------------------------
	Inlines
------------------------------------------------------------------------------------*/

//
// Decode Slot from a sound Id
//
inline INT IdToSlot( INT Id)
{
	return (Id & 0b1110) >> 1;
}

//
// Get head/foot region, if applicable.
//
inline const FPointRegion& GetRegion( AActor* Actor, INT RegionType=0)
{
	APawn* P = Cast<APawn>(Actor);
	if ( P )
	{
		if ( RegionType > 0 )
			return P->HeadRegion;
		if ( RegionType < 0 )
			return P->FootRegion;
	}
	return Actor->Region;
}

inline ALAudioSoundHandle* ALAudioSoundInstance::GetHandle() const
{
	return Sound ? (ALAudioSoundHandle*)Sound->Handle : nullptr;
}

inline INT ALAudioSoundInstance::GetSlot() const
{
	return IdToSlot(Id);
}

