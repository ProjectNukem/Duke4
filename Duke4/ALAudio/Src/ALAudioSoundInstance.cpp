/*=============================================================================
	ALAudioSoundInstance.cpp: ALAudio abstraction of a playing sound.
	
	Revision history:
	* Created by Fernando Velazquez (Higor)
=============================================================================*/

#include "ALAudio.h"
#ifndef warnf
#define warnf GWarn->Logf
#endif

static void TransformCoordinates( ALfloat Dest[3], const FVector &Source)
{
	Dest[0] = Source.X;
	Dest[1] = Source.Y;
#if DNF
	Dest[2] = Source.Z;
#else
	Dest[2] = -Source.Z;
#endif
}

static FLOAT MOD( FLOAT Value, FLOAT Limit)
{
	return Value - (FLOAT)appFloor(Value / Limit) * Limit;
}


void ALAudioSoundInstance::Init()
{
	// Initial values must match API defaults
	Radius            = 3.40282e+38f; //std::numeric_limits<float>::max();
	Pitch             = 1.0;
	DopplerFactor     = 1.0;
	AttenuationFactor = 1.0;
	Location          = FVector(0,0,0);
	Velocity          = FVector(0,0,0);
	LastAudioOffset   = 0;
	LipsynchTimer     = 0.0;


	alGenSources( 1, &SourceID);
#if 1
	//Ugly hack, find out why we get an OpenAL invalid value
	alSourcei( SourceID, AL_BUFFER, 0);
	alGetError();
#endif
	alSourcei( SourceID, AL_BUFFER, GetHandle()->ID);

	if (IsAmbient())
	{
	}
	else
	{
		// The higher the value the higher the distance based attenuation.
		alSourcef( SourceID, AL_ROLLOFF_FACTOR, 1.1f);
	}

	// Disable spatialization
	if ( SoundFlags & SF_No3D )
		alSourcei( SourceID, AL_SOURCE_SPATIALIZE_SOFT, AL_FALSE);

#if DNF
	// Disable HRTF
	if ( SoundFlags & SF_No3D )
	{
		alSourcei( SourceID, AL_DIRECT_CHANNELS_SOFT, AL_REMIX_UNMATCHED_SOFT );
		alSourcei( SourceID, AL_SOURCE_RELATIVE, AL_TRUE );
	}
#endif

	// Enable loop
	if ( SoundFlags & SF_Loop )
		alSourcei( SourceID, AL_LOOPING, AL_TRUE);

	PriorityMultiplier = 1.f;
}

INT ALAudioSoundInstance::Update( const ALAudioSoundInstance::UpdateParams& Params)
{
	if ( !IsPlaying() )
	{
		// Cleanup this instance.
		if ( SourceID )
			Stop();
		return 0;
	}

	// Update Actor related vars
	if ( Actor )
	{
#if DNF
		if ( IsMonitoredSound )
			UpdateMonitoredSound();
#endif

#if USE_LIPSYNCH
		// DeusX lipsynch
		if ( Slot == SLOT_Talk )
		{
			// Handle lipsynch.
			if ( GOpenALSOFT && Actor->bIsPawn && ((APawn*)Actor)->bIsSpeaking && ((LipsynchTimer+=DeltaTime) > 1.f/LIPSYNCH_FREQUENCY) )
			{
				// Do the magic!
				Lipsynch(Playing);
				LipsynchTimer = MOD(LipsynchTimer, 1.f/LIPSYNCH_FREQUENCY);
			}
		}
#endif
	}


	// Validate automatically played Ambient Sound.
	if ( SoundFlags & SF_AmbientSound )
	{
		if ( !Actor || Actor->bDeleteMe || Actor->AmbientSound != Sound || !Params.Realtime )
		{
			Stop();
			return 0;
		}
		Radius = Actor->WorldSoundRadius();
	}

	// Stop looping non-ambient sound if necessary
	if ( !(SoundFlags & SF_AmbientSound) && (SoundFlags & SF_LoopingSource) )
	{
		// Special hack for looping Weapon Sounds like GESBioRifle to prevent infinite loops. Check if Fire/AltFire is still pressed, otherwise stop the loop.
#if DNF
		APawn* Pawn = Cast<APawn>(Actor);
		if ( Pawn && Pawn->Weapon && !Pawn->bFire && !Pawn->bAltFire )
		{
			Stop();
			return 0;
		}
#elif RUNE || RUNE_CLASSIC
		APlayerPawn* Player = Params.Viewport->Actor;
		if ( Player->Weapon && !Player->bFire && !Player->bAltFire )
		{
			Stop();
			return 0;
		}
#else
		// New hack
		if ( Actor )
		{
			APawn*   Pawn   = Cast<APawn>(Actor);
			AWeapon* Weapon = Cast<AWeapon>(Actor);
			if ( Pawn )
				Weapon = Pawn->Weapon;
			else if ( Weapon )
				Pawn = Cast<APawn>(Weapon->Owner);

			// A weapon (or it's owner) is emitting this looping sound
			// The pawn owner no longer exists or no longer firing
			if ( Weapon
				&& (!Pawn || (!Pawn->bFire && !Pawn->bAltFire)) )
			{
				SoundFlags &= (~SF_LoopingSource);
				if ( Sound == Weapon->CockingSound
					|| Sound == Weapon->FireSound
					|| Sound == Weapon->AltFireSound )
				{
					Stop();
					return 0;
				}
			}
		}
#endif
	}

	// Update from emitter Actor (if present) and stop looping sound if out of range
	UpdateEmission( Params.AudioSub->LastRenderCoords, Params.LightRealtime);
	if ( SoundFlags & SF_Loop )
	{
		if ( (GetSlot() != SLOT_Interface) && FDistSquared( Params.AudioSub->LastRenderCoords.Origin, Location) > Square(Radius) )
		{
			Stop();
			return 0;
		}
	}

	// Process Audio Subsystem volume changes.
	UpdateVolume(Params.AudioSub);

	// Update this sound instance's priority.
	Priority = Params.AudioSub->SoundPriority( Location, Params.AudioSub->LastRenderCoords.Origin, Volume, Radius, GetSlot(), PriorityMultiplier);

	ProcessLoop(); // TODO: Find a way to let OpenAL properly loop things

	if ( GFilterExtensionLoaded )
		UpdateAttenuation( Params.AudioSub->AttenuationFactor(*this), Params.DeltaTime);

	return 1;
}


bool ALAudioSoundInstance::IsPlaying()
{
	ALint state = AL_STOPPED;
	if ( SourceID && alIsSource(SourceID) )
		alGetSourcei( SourceID, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}


void ALAudioSoundInstance::Stop()
{
	guard(ALAudioSoundInstance::Stop);
	if ( SourceID && alIsSource(SourceID) )
	{
		alSourcei( SourceID, AL_LOOPING, FALSE); //TODO: Track state
		if ( IsPlaying() )
			alSourceStop(SourceID);
		alDeleteSources( 1, &SourceID);
		SourceID = 0;
	}
#if DNF
	if ( Actor && IsMonitoredSound )
		Actor->MonitorSoundLevel = 0.f;
#endif
	Sound = nullptr;
	Actor = nullptr;
	Id = 0;
	Priority = 0.0f;
	Volume = 0.0f;
	Radius = 0.0f;
	SoundFlags = 0;
#if DNF
	IsMonitoredSound = FALSE;
#endif
	unguard;
}

bool ALAudioSoundInstance::IsAmbient()
{
	return GetSlot() == SLOT_Ambient && (!Actor || Sound != Actor->AmbientSound || Actor->bStatic || !Actor->bMovable);
}

void ALAudioSoundInstance::SetRadius( FLOAT NewRadius)
{
	if ( Radius != NewRadius )
	{
		Radius = NewRadius;
		alSourcef( SourceID, AL_REFERENCE_DISTANCE, 0.1*Radius);
		alSourcef( SourceID, AL_MAX_DISTANCE, Radius);

		if (IsAmbient())
			alSourcef( SourceID, AL_SOURCE_RADIUS, Radius*0.5);
	}
}

void ALAudioSoundInstance::SetPitch( FLOAT NewPitch)
{
	if ( Pitch != NewPitch )
	{
		Pitch = NewPitch;
		alSourcef( SourceID, AL_PITCH, Pitch);
	}
}

void ALAudioSoundInstance::SetDopplerFactor( FLOAT NewDopplerFactor)
{
	INT Slot = GetSlot();
	if ( (SoundFlags & SF_No3D) || (Slot == SLOT_Interface) )
		NewDopplerFactor = 0;
	else if ( Slot == SLOT_Misc || Slot == SLOT_Talk )
		NewDopplerFactor *= 0.5;

	if ( DopplerFactor != NewDopplerFactor )
	{
		DopplerFactor = NewDopplerFactor;
		alSourcef( SourceID, AL_DOPPLER_FACTOR, NewDopplerFactor);
	}
}

void ALAudioSoundInstance::SetVolume( FLOAT NewVolume, FLOAT NewMaxVolume)
{
//	NewVolume = Clamp<FLOAT>( NewVolume, 0, NewMaxVolume);
	alSourcef( SourceID, AL_GAIN, NewVolume);
	alSourcef( SourceID, AL_MAX_GAIN, NewMaxVolume);
}

void ALAudioSoundInstance::SetLocation( FVector NewLocation)
{
	if ( Location != NewLocation )
	{
		Location = NewLocation;
		ALfloat alPosition[3];
		TransformCoordinates( alPosition, Location);
		alSourcefv( SourceID, AL_POSITION, alPosition);
	}
}

void ALAudioSoundInstance::SetVelocity( FVector NewVelocity)
{
	if ( Velocity != NewVelocity )
	{
		Velocity = NewVelocity;
		ALfloat alVelocity[3];
		TransformCoordinates( alVelocity, Velocity);
		alSourcefv( SourceID, AL_VELOCITY, alVelocity);
	}
}

void ALAudioSoundInstance::SetEFX( ALuint NewEffectSlot, INT FilterType)
{
	guard(ALAudioSoundInstance::SetEFX);
#ifdef EFX
	if ( !GEffectsExtensionLoaded )
		return;

	// Set the EFX effect slot on the Aux0 line on the source(s)
	if ( SourceID && alIsSource(SourceID) )
	{
		// EFX implementation doesnt match original reverb fully (yet?). Smirftsch
		alSource3i( SourceID, AL_AUXILIARY_SEND_FILTER, NewEffectSlot, 0, FilterType); 

		// Check error status
		ALenum Error = alGetError();
		if ( Error != AL_NO_ERROR)
			GWarn->Logf(TEXT("ALAudio: EFX set slot to source error: %s [%i]"), appFromAnsi(alGetString(Error)), Error);
	}
#endif
	unguard;
}

void ALAudioSoundInstance::UpdateEmission( const FCoords& ListenerCoords, UBOOL LightRealtime)
{
	// Nothing to update from
	if ( !Actor )
		return;

	INT Slot = GetSlot();


	// Update source's position and velocity.
	SetLocation(Actor->Location);
	SetVelocity(Actor->Velocity);

	// Update Water emission flag (voices are emitted from HeadRegion)
	const FPointRegion& Region = GetRegion( Actor, (Slot==SLOT_Talk || Slot==SLOT_Pain) );
	if ( Region.Zone && Region.Zone->bWaterZone )
		SoundFlags |= SF_WaterEmission;
	else
		SoundFlags &= ~SF_WaterEmission;


	// Update Ambient Sound volume and radius
	if ( Slot == SLOT_Ambient )
	{
		FLOAT Brightness = 2.0 * AMBIENT_FACTOR * (FLOAT)Actor->SoundVolume / 255.0;
		if ( UALAudioSubsystem::__Render && (Actor->LightType != LT_None) )
		{
			FPlane Color;
			Brightness *= (FLOAT)Actor->LightBrightness / 255.0;
			FLOAT BrightnessScale = 1.0f;
			UALAudioSubsystem::__Render->GlobalLighting( LightRealtime, Actor, BrightnessScale, Color);
			Brightness *= BrightnessScale;
		}
		Volume = Brightness;
		Radius = Actor->WorldSoundRadius();
		SetRadius( Radius );
		SetPitch( Actor->SoundPitch / 64.0);
	}
}

#if DNF
void ALAudioSoundInstance::UpdateMonitoredSound(INT SampleSize)
{
	guard(UALAudioSubsystem::UpdateMonitoredSound);
	ALint Error;

	// Get buffer data.
	ALAudioSoundHandle* AudioHandle = (ALAudioSoundHandle*)Sound->Handle;

	ALfloat* SampleData = NULL;
	ALint NumChannels = 0;
	ALint NumSamples = 0;
	ALint SampleRate = 0;
	ALint SampleIndex = 0;
	ALint SampleBits = 0;
    ALfloat MaxVolume = -1000.f;

	guard(QueryInfo);
	alGetError();
	alGetBufferi(AudioHandle->ID, AL_CHANNELS, &NumChannels); if ((Error = alGetError()) != AL_NO_ERROR)
	{
	    warnf(TEXT("Failed to query number of channels (%s)."), appFromAnsi(alGetString(Error)));
	    goto Cleanup;
    }
	alGetBufferi(AudioHandle->ID, AL_SAMPLE_LENGTH_SOFT, &NumSamples);	if ((Error = alGetError()) != AL_NO_ERROR)
    {
        warnf(TEXT("Failed to query number of samples (%s)."), appFromAnsi(alGetString(Error)));
        goto Cleanup;
    }
	alGetBufferi(AudioHandle->ID, AL_FREQUENCY, &SampleRate); if ((Error = alGetError()) != AL_NO_ERROR)
    {
        warnf(TEXT("Failed to query sample rate (%s)."), appFromAnsi(alGetString(Error)));
        goto Cleanup;
    }
	alGetBufferi(AudioHandle->ID, AL_BITS, &SampleBits); if ((Error = alGetError()) != AL_NO_ERROR)
	{
		warnf(TEXT("Failed to query sample bits (%s)."), appFromAnsi(alGetString(Error)));
		goto Cleanup;
	}
	alGetSourcei(SourceID, AL_SAMPLE_OFFSET, &SampleIndex);	if ((Error = alGetError()) != AL_NO_ERROR)
    {
        warnf(TEXT("Failed to query source offset (%s)."), appFromAnsi(alGetString(Error)));
        goto Cleanup;
    }
	unguard;

	// Determine if we have enough samples (at least for now).
	if (NumSamples< SampleSize)
	{
	    warnf(TEXT("Skipping monitored sound update for too small sample (%i samples)."), NumSamples);
	    goto Cleanup;
    }

	// Determine sample position.
	SampleIndex = Max(SampleIndex + 512 - SampleSize / 2, 0);
	SampleIndex = Min(SampleIndex, NumSamples - SampleSize - 1);

	// Create new buffer.
	SampleData = new ALfloat[SampleSize];

	// Download sample data.
	guard(Download);
	Sound->Data.Load();

	switch (SampleBits)
	{
	case CHAR_BIT:
		for (int i = 0; i < SampleSize; i++)
			SampleData[i] = ((SBYTE*)&Sound->Data(0))[SampleIndex + i] / (ALfloat)(INT8_MAX + 1);
		break;
	case CHAR_BIT * sizeof(SWORD):
		for (int i = 0; i < SampleSize; i++)
			SampleData[i] = ((SWORD*)&Sound->Data(0))[SampleIndex + i] / (ALfloat)(INT16_MAX + 1);
		break;
	case CHAR_BIT * sizeof(ALfloat):
		appMemcpy(SampleData, (ALfloat*)&Sound->Data(0) + SampleIndex, sizeof SampleData);
		break;
	default:
		warnf(TEXT("Unknown sample bits."));
		goto Cleanup;
	}

	unguard;

	// Calculate max volume.
	guard(CalcAvgVolume);
	for (INT i = 0; i< SampleSize; i++)
	{
		MaxVolume = Max(MaxVolume, SampleData[i]);
	}
	unguard;

	// Update monitored sound level.
	if (fabs(Actor->MonitorSoundLevel - MaxVolume) > 0.07)
		Actor->MonitorSoundLevel = (Actor->MonitorSoundLevel + MaxVolume) * 0.5f;

	// And the last bit.
Cleanup:
	if (SampleData)
		delete[] SampleData;

	unguard;
}
#endif

void ALAudioSoundInstance::UpdateVolume( UALAudioSubsystem* AudioSub)
{
	guard(ALAudioSoundInstance::UpdateVolume);

	//OpenAL:
	/*
	distance = min(distance, AL_MAX_DISTANCE) // avoid negative gain
	gain = (1  AL_ROLLOFF_FACTOR * (distance   AL_REFERENCE_DISTANCE) / (AL_MAX_DISTANCE  AL_REFERENCE_DISTANCE))
	*/

	// Adjust Volume
	FLOAT AdjustedVolume = Min<FLOAT>( Volume, 4);
	if ( AdjustedVolume > 1 )
	{
		if ( GetSlot() == SLOT_Interface )
			AdjustedVolume = appSqrt(AdjustedVolume);
		else
			AdjustedVolume = appSqrt(1.f + (AdjustedVolume - 1.f) * 0.2f);
	}

	// Update volume
	FLOAT ConfiguredAudioVolume = 0.5f * ((SoundFlags & SF_Speech) ? AudioSub->SpeechVolume : AudioSub->SoundVolume);
	FLOAT AudioVolume    = AdjustedVolume * ConfiguredAudioVolume / 255.f; //GAIN
	FLOAT MaxAudioVolume = 2.0f           * ConfiguredAudioVolume / 255.f; //MAX_GAIN

	// Scale Viewport sounds
	if ( GIsEditor && (SoundFlags & SF_No3D) ) 
	{
		AudioVolume *= AudioSub->ViewportVolumeIntensity;
		MaxAudioVolume *= AudioSub->ViewportVolumeIntensity;
	}

	// Slightly amplify when both emitter and listener are underwater
	if ( SoundFlags & SF_WaterEmission )
	{
		AZoneInfo* Zone = GetRegion( AudioSub->GetCameraActor(), 1).Zone;
		if ( Zone && Zone->bWaterZone )
			AudioVolume *= 1.25;
	}

	// Allow maximum volume to be a bit higher.
	SetVolume( AudioVolume, MaxAudioVolume);

	unguard;
}

void ALAudioSoundInstance::UpdateAttenuation( FLOAT NewAttenuationFactor, FLOAT DeltaTime)
{
	FLOAT OldAttenuationFactor = AttenuationFactor;

	// No transition
	if ( DeltaTime < 0 )
		AttenuationFactor = NewAttenuationFactor;
	else if ( AttenuationFactor != NewAttenuationFactor )
	{
		FLOAT Dir = NewAttenuationFactor < AttenuationFactor ? -1.f : 1.f;
		FLOAT DeltaAttenuation = Clamp( Abs(NewAttenuationFactor-AttenuationFactor), 0.f, DeltaTime);
		AttenuationFactor += DeltaAttenuation * Dir * 0.2f;
	}

	if ( AttenuationFactor != OldAttenuationFactor )
	{
		ALAudioSoundHandle* AudioHandle = GetHandle();
		if ( AttenuationFactor == 1.f )
			alFilteri( AudioHandle->Filter, AL_FILTER_TYPE, AL_FILTER_NULL); // TODO: Filter should be instance based!!!
		else
		{
			alFilteri( AudioHandle->Filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf( AudioHandle->Filter, AL_LOWPASS_GAIN, 0.5f + 0.5f * AttenuationFactor );
			alFilterf( AudioHandle->Filter, AL_LOWPASS_GAINHF, 0.2f + 0.8f * AttenuationFactor );
		}
		alSourcei( SourceID, AL_DIRECT_FILTER, AudioHandle->Filter);
	}
}

void ALAudioSoundInstance::ProcessLoop()
{
	if ( SoundFlags & SF_LoopingSource )
	{
		ALAudioSoundHandle* AudioHandle = GetHandle();
		if ( AudioHandle->bLoopingSample && (AudioHandle->LoopEnd>0 || AudioHandle->LoopStart>0) )
		{
			ALint ALOffset;
			alGetSourcei( SourceID, AL_SAMPLE_OFFSET, &ALOffset);

			// OpenAL has already looped this internally, move to LoopStart if we haven't already
			if ( LastAudioOffset > ALOffset )
			{
				if ( AudioHandle->LoopStart > ALOffset )
				{
					ALOffset = AudioHandle->LoopStart;
					alSourcei( SourceID, AL_SAMPLE_OFFSET, ALOffset);
				}
			}
			// Handle loop here if OpenAL hasn't done it
			else if ( ALOffset >= AudioHandle->LoopEnd )
			{
				ALOffset = AudioHandle->LoopStart;
				alSourcei( SourceID, AL_SAMPLE_OFFSET, ALOffset);
			}
			LastAudioOffset = ALOffset;
		}
	}
}


FString ALAudioSoundInstance::GetSoundInformation( UBOOL Detail)
{
	if ( Detail )
	{
		if ( !SourceID )
			return TEXT("None ...");

		return FString::Printf(	TEXT("%s - Vol: %05.2f Pitch: %05.2f Radius: %07.2f Priority: %05.2f Actor:%s")
			, *FObjectFullName(Sound)
			, Volume, Pitch, Radius, Priority
			, *FObjectFullName(Actor));
	}
	else
	{
		if ( !SourceID )
			return TEXT("None");

		return FObjectFullName(Sound);
	}
}
