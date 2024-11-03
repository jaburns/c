#pragma once

#define AUDIO_SAMPLE_RATE 44100

typedef struct {
    f32* samples;
    size_t num_samples;  // total for both channels, aka length of samples array
} AudioClip;

enumdef(AudioPlayerMsgKind, u8){
    AUDIO_PLAYER_MSG_KIND_PLAY,
    AUDIO_PLAYER_MSG_KIND_STOP,
};

typedef struct {
    AudioClip* clip;
} AudioPlayerMsgPlay;

typedef struct {
    AudioPlayerMsgKind kind;
    union {
        AudioPlayerMsgPlay msg_play;
    };
} AudioPlayerMsg;

typedef struct {
    Channel msg_chan;  // of AudioPlayerMsg
    AudioClip* clip;
    size_t idx;
} AudioPlayer;

typedef void (*AudioCallbackFn)(AudioPlayer* player, u8* out_stream, i32 out_stream_byte_len);

internal void audio_clip_load(Arena* arena, AudioClip* clip, char* path);
internal void audio_clip_play(AudioPlayer* player, AudioClip* clip);

internal void audio_player_alloc_into(AudioPlayer* player, Arena* arena);
internal void audio_player_stream_callback(AudioPlayer* player, u8* out_stream, i32 out_stream_byte_len);