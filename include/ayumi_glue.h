#pragma once

#include <stdint.h>
#include <ayumi.h>

// define this to use ayumi as SSG emulation engine
#define PC98_SSG_USE_AYUMI
#define PC98_OPNA_SSG_CLOCK ((55466 * 72) / 2)

#ifdef __cplusplus
extern "C" {
#endif

struct ayumi_ctx_t {
    // ayumi context
    struct ayumi   ayumi;
    // shadow AY register storage
    uint8_t regs[16];
    // volume
    double  volume[2];
};

void     ayumi_reg_write(struct ayumi_ctx_t* ctx, int reg, uint8_t data);
uint8_t  ayumi_reg_read (struct ayumi_ctx_t* ctx, int reg);
uint32_t ayumi_render_audio_float(struct ayumi_ctx_t* ctx, float* out, uint32_t frames);
uint32_t ayumi_mix_audio_int32(struct ayumi_ctx_t* ctx, int32_t* out, uint32_t frames);

// STATIC ayumi context (FIXME!)
extern struct ayumi_ctx_t ayumi_ctx;

#ifdef __cplusplus
}
#endif
