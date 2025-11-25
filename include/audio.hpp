#pragma once

namespace audio
{
  class Audio
  {
  public:
		Audio() = delete;
		Audio(const Audio&) = delete;
		~Audio() = default;

		static void initialize();
		static void finalize();
  };

  class Sound
  {
  public:
    Sound(const char* filePath, bool stream = false);
    ~Sound();

    void play();
    void stop();
    void pause();
    bool isPlaying() const;

    void setVolume(float volume);
    void setPan(float pan);
    void setPitch(float pitch);
    void setLooping(bool looping);

    void* m_sound;
  };
}
