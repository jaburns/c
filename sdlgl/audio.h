#pragma once

#define AUDIO_SAMPLE_RATE 44100

structdef(AudioClip) {
    f32*   samples;
    size_t num_samples;  // total for both channels, aka length of samples array
};

enumdef(AudioPlayerMsgKind, u8){
    AUDIO_PLAYER_MSG_KIND_PLAY,
    AUDIO_PLAYER_MSG_KIND_STOP,
};

structdef(AudioPlayerMsgPlay) {
    AudioClip* clip;
};

structdef(AudioPlayerMsg) {
    AudioPlayerMsgKind kind;
    union {
        AudioPlayerMsgPlay msg_play;
    };
};

structdef(AudioPlayer) {
    Channel    msg_chan;  // of AudioPlayerMsg
    AudioClip* clip;
    size_t     idx;
};

typedef void (*AudioCallbackFn)(AudioPlayer* player, u8* out_stream, i32 out_stream_byte_len);

internal void audio_clip_load(Arena* arena, AudioClip* clip, char* path);
internal void audio_clip_play(AudioPlayer* player, AudioClip* clip);

internal AudioPlayer audio_player_alloc(Arena* arena);
internal void        audio_player_stream_callback(AudioPlayer* player, u8* out_stream, i32 out_stream_byte_len);