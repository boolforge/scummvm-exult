    debug(1, "ExultAudioAdapter: Initialization complete. Registered with ScummVM mixer on channel %d.", (int)_channelId);
    return true;
}

void ExultAudioAdapter::shutdown() {
    debug(1, "ExultAudioAdapter: shutdown() called.");

    // Stop and unregister the stream from ScummVM's mixer.
    if (_channelId != Audio::ChannelId::INVALID && _mixer) {
        _mixer->stopChannel(_channelId);
        _channelId = Audio::ChannelId::INVALID;
        debug(1, "ExultAudioAdapter: Unregistered stream from ScummVM mixer.");
    }

    // Potentially tell Exult's audio system to shut down or revert from ScummVM mode.
    // Note: ExultCore::Audio::Destroy() is static and deletes the singleton.
    // We should not call it here if ExultEngine might be reinitialized later.
    // Instead, call specific cleanup methods if they exist.
    if (_exultAudioSystem) {
        // _exultAudioSystem->stop_music(); // Example
        // _exultAudioSystem->stop_sound_effects(); // Example
        // _exultAudioSystem->setScummVMMode(false, 0, 0); // Revert mode if appropriate
        // For now, Exult's Audio::Destroy() will handle its internal cleanup when ExultEngine is destroyed.
        debug(1, "ExultAudioAdapter: Exult audio system cleanup (if any specific calls needed) would be here.");
    }
}

// ScummVM Audio::Stream interface implementation
bool ExultAudioAdapter::getSamples(int16 *stream, int length) {
    if (!_exultAudioSystem || !_exultAudioSystem->is_audio_enabled()) {
        // Fill with silence if Exult audio is not ready or explicitly disabled in Exult
        memset(stream, 0, length * sizeof(int16)); // length is number of int16 samples
        return true;
    }
    // Request mixed audio data from ExultCore::Audio.
    // `length` is the total number of int16 samples ScummVM's mixer expects.
    int samples_read = _exultAudioSystem->getMixedAudio(stream, length);

    // If ExultCore::Audio provided fewer samples than requested, fill the remainder with silence.
    if (samples_read < length) {
        // Should not happen if getMixedAudio is implemented correctly to fill 'length' samples
        // or if it returns the actual number of samples it could fill (which should be 'length' or 0).
        // If it returns 0 (e.g. error or no data), the whole buffer should be silent.
        // If it returns N < length, that means it partially filled.
        debug(2, "ExultAudioAdapter::getSamples: ExultCore::Audio::getMixedAudio returned %d samples, expected %d. Filling remainder with silence.", samples_read, length);
        memset(stream + samples_read, 0, (length - samples_read) * sizeof(int16));
    } else if (samples_read > length) {
        // This would be an error in ExultCore::Audio::getMixedAudio implementation.
        error("ExultAudioAdapter::getSamples: ExultCore::Audio::getMixedAudio returned %d samples, but only %d were requested!", samples_read, length);
        // Potentially truncate or handle, but this indicates a deeper issue. For now, ScummVM mixer will only use `length`.
    }
    // If samples_read == length, buffer is correctly filled.
    // If samples_read == 0, buffer (hopefully already zeroed by getMixedAudio or here) is silent.

    return true; // Always return true as we handle silence filling.
}

} // namespace Exult
} // namespace ScummVM


