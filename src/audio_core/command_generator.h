// Copyright 2020 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <array>
#include "audio_core/common.h"
#include "audio_core/voice_context.h"
#include "common/common_funcs.h"
#include "common/common_types.h"

namespace Core::Memory {
class Memory;
}

namespace AudioCore {
class MixContext;
class SplitterContext;
class ServerSplitterDestinationData;
class ServerMixInfo;
using MixVolumeBuffer = std::array<float, AudioCommon::MAX_MIX_BUFFERS>;
class CommandGenerator {
public:
    explicit CommandGenerator(AudioCommon::AudioRendererParameter& worker_params,
                              VoiceContext& voice_context, MixContext& mix_context,
                              SplitterContext& splitter_context, Core::Memory::Memory& memory);
    ~CommandGenerator();

    void ClearMixBuffers();
    void GenerateVoiceCommands();
    void GenerateVoiceCommand(ServerVoiceInfo& voice_info);
    void GenerateSubMixCommands();
    void GenerateFinalMixCommands();
    void PreCommand();
    void PostCommand();

    s32* GetChannelMixBuffer(s32 channel);
    const s32* GetChannelMixBuffer(s32 channel) const;
    s32* GetMixBuffer(std::size_t index);
    const s32* GetMixBuffer(std::size_t index) const;
    std::size_t GetMixChannelBufferOffset(s32 channel) const;

private:
    void GenerateDataSourceCommand(ServerVoiceInfo& voice_info, VoiceState& dsp_state, s32 channel);
    void GenerateBiquadFilterCommandForVoice(ServerVoiceInfo& voice_info, VoiceState& dsp_state,
                                             s32 mix_buffer_count, s32 channel);
    void GenerateVolumeRampCommand(float last_volume, float current_volume, s32 channel,
                                   s32 node_id);
    void GenerateVoiceMixCommand(const MixVolumeBuffer& mix_volumes,
                                 const MixVolumeBuffer& last_mix_volumes, VoiceState& dsp_state,
                                 s32 mix_buffer_offset, s32 mix_buffer_count, s32 voice_index,
                                 s32 node_id);
    void GenerateSubMixCommand(ServerMixInfo& mix_info);
    void GenerateMixCommands(ServerMixInfo& mix_info);
    void GenerateMixCommand(std::size_t output_offset, std::size_t input_offset, float volume,
                            s32 node_id);
    void GenerateFinalMixCommand();
    void GenerateBiquadFilterCommand(s32 mix_buffer, const BiquadFilterParameter& params,
                                     std::array<s64, 2>& state, std::size_t input_offset,
                                     std::size_t output_offset, s32 sample_count, s32 node_id);
    void GenerateDepopPrepareCommand(VoiceState& dsp_state);
    ServerSplitterDestinationData* GetDestinationData(s32 splitter_id, s32 index);

    // DSP Code
    s32 DecodePcm16(ServerVoiceInfo& voice_info, VoiceState& dsp_state, s32 sample_count,
                    s32 channel, std::size_t mix_offset);
    s32 DecodeAdpcm(ServerVoiceInfo& voice_info, VoiceState& dsp_state, s32 sample_count,
                    s32 channel, std::size_t mix_offset);
    void DecodeFromWaveBuffers(ServerVoiceInfo& voice_info, s32* output, VoiceState& dsp_state,
                               s32 channel, s32 target_sample_rate, s32 sample_count, s32 node_id);
    void Resample(s32* output, s32* input, s32 pitch, s32& fraction, s32 sample_count);

    AudioCommon::AudioRendererParameter& worker_params;
    VoiceContext& voice_context;
    MixContext& mix_context;
    SplitterContext& splitter_context;
    Core::Memory::Memory& memory;
    std::vector<s32> mix_buffer{};
    std::vector<s32> sample_buffer{};
    bool dumping_frame{false};
};
} // namespace AudioCore
