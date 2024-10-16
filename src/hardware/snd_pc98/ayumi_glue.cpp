#include <ayumi_glue.h>

// STATIC ayumi context (FIXME!)
ayumi_ctx_t ayumi_ctx;

void ayumi_reg_write(ayumi_ctx_t* ctx, int reg, uint8_t data)
{
    if ((ctx == nullptr) || (reg >= 16)) return;
    ctx->regs[reg] = data;

    // update ayumi context according to register write
    // it's actaully fine to update all registers (apart for possible performance issues)
    // since only write to R13 have side effect of resetting envelope phase
    
    // process tone
    ayumi_set_tone(&ctx->ayumi, 0, ctx->regs[0] | (ctx->regs[1] << 8));
    ayumi_set_tone(&ctx->ayumi, 1, ctx->regs[2] | (ctx->regs[3] << 8));
    ayumi_set_tone(&ctx->ayumi, 2, ctx->regs[4] | (ctx->regs[5] << 8));

    // process noise
    ayumi_set_noise(&ctx->ayumi, ctx->regs[6]);

    // process volume and mixer
    ayumi_set_volume(&ctx->ayumi, 0, ctx->regs[8] & 15);
    ayumi_set_volume(&ctx->ayumi, 1, ctx->regs[9] & 15);
    ayumi_set_volume(&ctx->ayumi, 2, ctx->regs[10] & 15);

    ayumi_set_mixer(&ctx->ayumi, 0, (ctx->regs[7] >> 0) & 0x1, (ctx->regs[7] >> 3) & 0x1, ctx->regs[8] & 0x10);
    ayumi_set_mixer(&ctx->ayumi, 1, (ctx->regs[7] >> 1) & 0x1, (ctx->regs[7] >> 4) & 0x1, ctx->regs[9] & 0x10);
    ayumi_set_mixer(&ctx->ayumi, 2, (ctx->regs[7] >> 2) & 0x1, (ctx->regs[7] >> 5) & 0x1, ctx->regs[10] & 0x10);

    // process envelope 
    ayumi_set_envelope(&ctx->ayumi, ctx->regs[11] | (ctx->regs[12] << 8));

    // process envelope shape
    if (reg == 13) ayumi_set_envelope_shape(&ctx->ayumi, ctx->regs[13]);
}

uint8_t ayumi_reg_read(ayumi_ctx_t* ctx, int reg)
{
    return ((ctx == nullptr) || (reg >= 16)) ? 0xFF : ctx->regs[reg]; // standard open bus behavior of YM2149F
}

uint32_t ayumi_render_audio_float(ayumi_ctx_t* ctx, float* out, uint32_t frames)
{
    if (ctx == nullptr) return 0;

    for(uint32_t i = 0; i < frames; i++) {
        ayumi_process(&ctx->ayumi);
        ayumi_remove_dc(&ctx->ayumi);

        *(out + 0) = ctx->ayumi.left;
        *(out + 1) = ctx->ayumi.right;
        out += 2;
    }
    return frames;
}

uint32_t ayumi_mix_audio_int32(ayumi_ctx_t* ctx, int32_t* out, uint32_t frames)
{
    if(ctx == nullptr) return 0;

    for(uint32_t i = 0; i < frames; i++) {
        ayumi_process(&ctx->ayumi);
        ayumi_remove_dc(&ctx->ayumi);

        *(out + 0) += ctx->ayumi.left  * ctx->volume[0];
        *(out + 1) += ctx->ayumi.right * ctx->volume[1];
        out += 2;
    }
    return frames;
}

