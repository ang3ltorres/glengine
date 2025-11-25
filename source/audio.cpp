#include "audio.hpp"

#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

using namespace audio;

static ma_engine g_engine;

void Audio::initialize()
{
  ma_engine_init(NULL, &g_engine);
}

void Audio::finalize()
{
  ma_engine_uninit(&g_engine);
}

Sound::Sound(const char* filePath, bool stream)
{
  ma_sound* sound = new ma_sound();

  ma_uint32 flags = stream ? ma_sound_flags::MA_SOUND_FLAG_STREAM : 0;
  ma_result result = ma_sound_init_from_file(&g_engine, filePath, flags, NULL, NULL, sound);

  if (result != MA_SUCCESS)
  {
    printf("Failed to load sound: %s (Result: %d)\n", filePath, result);
    delete sound;
    m_sound = nullptr;
    return;
  }
  m_sound = sound;
}

Sound::~Sound()
{
  if (m_sound)
  {
    ma_sound_uninit((ma_sound*)m_sound);
    delete (ma_sound*)m_sound;
  }
}

void Sound::play()
{
  if (m_sound)
  {
    if (ma_sound_at_end((ma_sound*)m_sound))
      ma_sound_seek_to_pcm_frame((ma_sound*)m_sound, 0);

    ma_sound_start((ma_sound*)m_sound);
  }
}

void Sound::stop()
{
  if (m_sound)
  {
    ma_sound_stop((ma_sound*)m_sound);
    ma_sound_seek_to_pcm_frame((ma_sound*)m_sound, 0);
  }
}

void Sound::pause()
{
  if (m_sound)
    ma_sound_stop((ma_sound*)m_sound);
}

bool Sound::isPlaying() const
{
  return m_sound && ma_sound_is_playing((ma_sound*)m_sound);
}

void Sound::setVolume(float volume)
{
  if (m_sound)
    ma_sound_set_volume((ma_sound*)m_sound, volume);
}

void Sound::setPan(float pan)
{
  if (m_sound)
    ma_sound_set_pan((ma_sound*)m_sound, pan);
}

void Sound::setPitch(float pitch)
{
  if (m_sound)
    ma_sound_set_pitch((ma_sound*)m_sound, pitch);
}

void Sound::setLooping(bool looping)
{
  if (m_sound)
    ma_sound_set_looping((ma_sound*)m_sound, looping);
}
