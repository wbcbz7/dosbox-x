#include    "np2glue.h"
//#include	"compiler.h"
//#include	"pccore.h"
//#include	"iocore.h"
//#include	"cbuscore.h"
#include	"board26k.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include    "ayumi_glue.h"

static void IOOUTCALL opn_o188(UINT port, REG8 dat) {

	opn.addr = dat;
	opn.data = dat;
	(void)port;
}

static void IOOUTCALL opn_o18a(UINT port, REG8 dat) {

	UINT	addr;

	opn.data = dat;
	addr = opn.addr;
	S98_put(NORMAL2608, addr, dat);
	if (addr < 0x10) {
		if (addr != 0x0e) {
#ifdef PC98_SSG_USE_AYUMI
            ayumi_reg_write(&ayumi_ctx, addr, dat);
#else
            psggen_setreg(&psg1, addr, dat);
#endif
		}
	}
	else if (addr < 0x100) {
		if (addr < 0x30) {
			if (addr == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(dat & 0x0f, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
				if (addr == 0x27) {
					opnch[2].extop = dat & 0xc0;
				}
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(0, addr, dat);
		}
		opn.reg[addr] = dat;
	}
	(void)port;
}

static REG8 IOINPCALL opn_i188(UINT port) {

	(void)port;
	return(fmtimer.status);
}

static REG8 IOINPCALL opn_i18a(UINT port) {

	UINT	addr;

	addr = opn.addr;
	if (addr == 0x0e) {
		return(fmboard_getjoy(&psg1));
	}
	else if (addr < 0x10) {
#ifdef PC98_SSG_USE_AYUMI
        return ayumi_reg_read(&ayumi_ctx, addr);
#else
		return(psggen_getreg(&psg1, addr));
#endif
	}
	(void)port;
	return(opn.data);
}


// ----

static const IOOUT opn_o[4] = {
			opn_o188,	opn_o18a,	NULL,		NULL};

static const IOINP opn_i[4] = {
			opn_i188,	opn_i18a,	NULL,		NULL};


void board26k_reset(const NP2CFG *pConfig) {

	opngen_setcfg(3, 0);
	fmtimer_reset(pConfig->snd26opt & 0xc0);
	soundrom_loadex(pConfig->snd26opt & 7, OEMTEXT("26"));
	opn.base = (pConfig->snd26opt & 0x10)?0x000:0x100;
}

void board26k_bind(void) {

	fmboard_fmrestore(0, 0);
#ifndef PC98_SSG_USE_AYUMI
	psggen_restore(&psg1);
#endif
//	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
//	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	cbuscore_attachsndex(0x188 - opn.base, opn_o, opn_i);
}

