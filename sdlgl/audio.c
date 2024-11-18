#include "inc.h"

internal void audio_clip_load(Arena* arena, AudioClip* clip, char* path) {
    ZeroStruct(clip);

    ArenaTemp scratch = scratch_acquire(&arena, 1);

    FILE* fp = fopen(path, "rb");
    if (!fp) Panic("Failed to open audio file %s", path);

    OggVorbis_File vorbis;
    if (ov_open_callbacks(fp, &vorbis, NULL, 0, OV_CALLBACKS_DEFAULT) != 0) {
        Panic("Invalid ogg file");
    }
    i64 total_sample_count = 2 * ov_pcm_total(&vorbis, -1);

    vorbis_info* vi = ov_info(&vorbis, -1);
    i32 channels = vi->channels;
    if (channels != 1 && channels != 2) {
        Panic("Unsupported number of channels. Only mono and stereo are supported.");
    }

    bool must_resample = vi->rate != AUDIO_SAMPLE_RATE;

    Arena* buffer_arena = must_resample ? scratch.arena : arena;
    f32* buffer = arena_alloc(buffer_arena, total_sample_count * sizeof(f32));

    i32 j = 0;
    while (1) {
        f32** pcm;
        i32 section = 0;
        long samples = ov_read_float(&vorbis, &pcm, 1024, &section);
        if (channels == 1) {
            for (i32 i = 0; i < samples; ++i) {
                buffer[j++] = pcm[0][i];
                buffer[j++] = pcm[0][i];
            }
        } else {
            for (i32 i = 0; i < samples; ++i) {
                buffer[j++] = pcm[0][i];
                buffer[j++] = pcm[1][i];
            }
        }
        if (samples <= 0)
            break;
    }

    if (must_resample) {
        f32 ratio = (f32)AUDIO_SAMPLE_RATE / (f32)vi->rate;
        i32 og_sample_count = total_sample_count;
        total_sample_count = (i32)(og_sample_count * ratio);
        f32* resampled_buffer = arena_alloc(arena, total_sample_count * sizeof(f32));

        for (i32 i = 0; i < total_sample_count; ++i) {
            f32 src_index = i / ratio;
            i32 idx_floor = (i32)src_index;
            f32 frac = src_index - idx_floor;
            i32 idx_ceil = (idx_floor + 1 < og_sample_count) ? idx_floor + 1 : idx_floor;
            resampled_buffer[i] = buffer[idx_floor] * (1.0f - frac) + buffer[idx_ceil] * frac;
        }

        buffer = resampled_buffer;
    }

    clip->samples = buffer;
    clip->num_samples = total_sample_count;

    ov_clear(&vorbis);

    scratch_release(scratch);
}

internal void audio_clip_play(AudioPlayer* player, AudioClip* clip) {
    AudioPlayerMsg msg = {
        .kind = AUDIO_PLAYER_MSG_KIND_PLAY,
        .msg_play = (AudioPlayerMsgPlay){
            .clip = clip,
        },
    };
    channel_push(&player->msg_chan, &msg);
}

internal AudioPlayer audio_player_alloc(Arena* arena) {
    Channel msg_chan = channel_alloc(arena, sizeof(AudioPlayerMsg), 256);
    return (AudioPlayer){
        .msg_chan = msg_chan,
    };
}

#define AUDIO_MASTER_VOLUME 0  // .25

internal void audio_player_stream_callback(AudioPlayer* player, u8* out_stream, i32 out_stream_byte_len) {
    foreach (ChannelIter, it, &player->msg_chan) {
        AudioPlayerMsg* msg = it.item;

        switch (msg->kind) {
            case AUDIO_PLAYER_MSG_KIND_PLAY: {
                player->clip = msg->msg_play.clip;
                player->idx = 0;
                break;
            }
            case AUDIO_PLAYER_MSG_KIND_STOP: {
                break;
            }
        }
    }

    f32* out = (f32*)out_stream;
    i32 length = out_stream_byte_len / sizeof(f32);

    for (i32 i = 0; i < length; i += 2) {
        if (player->clip == NULL || player->idx >= player->clip->num_samples) {
            out[i] = 0.f;
            out[i + 1] = 0.f;
            continue;
        }
        out[i] = AUDIO_MASTER_VOLUME * player->clip->samples[player->idx++];
        out[i + 1] = AUDIO_MASTER_VOLUME * player->clip->samples[player->idx++];
    }
}