class ExultAudioAdapter : public Audio::Stream {
public:
    ExultAudioAdapter(OSystem* system, Audio::Mixer* mixer, ExultCore::Audio* exultAudioSystem);
    ~ExultAudioAdapter() override;

    bool init();
    void shutdown();

    // ScummVM Audio::Stream interface
    bool getSamples(int16 *stream, int length) override;

private:
    OSystem* _osystem;
    Audio::Mixer* _mixer;
    ExultCore::Audio* _exultAudioSystem;
    Audio::ChannelId _channelId = Audio::ChannelId::INVALID; // Registered ScummVM audio channel/stream
};

