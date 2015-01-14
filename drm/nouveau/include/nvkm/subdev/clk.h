#ifndef __NOUVEAU_CLK_H__
#define __NOUVEAU_CLK_H__

#include <core/device.h>
#include <core/notify.h>
#include <core/subdev.h>

struct nouveau_pll_vals;
struct nvbios_pll;

enum nv_clk_src {
	nv_clk_src_crystal,
	nv_clk_src_href,

	nv_clk_src_hclk,
	nv_clk_src_hclkm3,
	nv_clk_src_hclkm3d2,
	nv_clk_src_hclkm2d3, /* NVAA */
	nv_clk_src_hclkm4, /* NVAA */
	nv_clk_src_cclk, /* NVAA */

	nv_clk_src_host,

	nv_clk_src_sppll0,
	nv_clk_src_sppll1,

	nv_clk_src_mpllsrcref,
	nv_clk_src_mpllsrc,
	nv_clk_src_mpll,
	nv_clk_src_mdiv,

	nv_clk_src_core,
	nv_clk_src_core_intm,
	nv_clk_src_shader,

	nv_clk_src_mem,

	nv_clk_src_gpc,
	nv_clk_src_rop,
	nv_clk_src_hubk01,
	nv_clk_src_hubk06,
	nv_clk_src_hubk07,
	nv_clk_src_copy,
	nv_clk_src_daemon,
	nv_clk_src_disp,
	nv_clk_src_vdec,

	nv_clk_src_dom6,

	nv_clk_src_max,
};

struct nouveau_cstate {
	struct list_head head;
	u8  voltage;
	u32 domain[nv_clk_src_max];
};

struct nouveau_pstate {
	struct list_head head;
	struct list_head list; /* c-states */
	struct nouveau_cstate base;
	u8 pstate;
	u8 fanspeed;
};

struct nouveau_clk {
	struct nouveau_subdev base;

	struct nouveau_domain *domains;
	struct nouveau_pstate bstate;

	struct list_head states;
	int state_nr;

	struct work_struct work;
	wait_queue_head_t wait;
	atomic_t waiting;

	struct nvkm_notify pwrsrc_ntfy;
	int pwrsrc;
	int pstate; /* current */
	int ustate_ac; /* user-requested (-1 disabled, -2 perfmon) */
	int ustate_dc; /* user-requested (-1 disabled, -2 perfmon) */
	int astate; /* perfmon adjustment (base) */
	int tstate; /* thermal adjustment (max-) */
	int dstate; /* display adjustment (min+) */

	bool allow_reclock;

	int  (*read)(struct nouveau_clk *, enum nv_clk_src);
	int  (*calc)(struct nouveau_clk *, struct nouveau_cstate *);
	int  (*prog)(struct nouveau_clk *);
	void (*tidy)(struct nouveau_clk *);

	/*XXX: die, these are here *only* to support the completely
	 *     bat-shit insane what-was-nouveau_hw.c code
	 */
	int (*pll_calc)(struct nouveau_clk *, struct nvbios_pll *,
			int clk, struct nouveau_pll_vals *pv);
	int (*pll_prog)(struct nouveau_clk *, u32 reg1,
			struct nouveau_pll_vals *pv);
};

static inline struct nouveau_clk *
nouveau_clk(void *obj)
{
	return (void *)nouveau_subdev(obj, NVDEV_SUBDEV_CLK);
}

struct nouveau_domain {
	enum nv_clk_src name;
	u8 bios; /* 0xff for none */
#define NVKM_CLK_DOM_FLAG_CORE 0x01
	u8 flags;
	const char *mname;
	int mdiv;
};

#define nouveau_clk_create(p,e,o,i,r,s,n,d)                                  \
	nouveau_clk_create_((p), (e), (o), (i), (r), (s), (n), sizeof(**d),  \
			      (void **)d)
#define nouveau_clk_destroy(p) ({                                            \
	struct nouveau_clk *clk = (p);                                       \
	_nouveau_clk_dtor(nv_object(clk));                                   \
})
#define nouveau_clk_init(p) ({                                               \
	struct nouveau_clk *clk = (p);                                       \
	_nouveau_clk_init(nv_object(clk));                                   \
})
#define nouveau_clk_fini(p,s) ({                                             \
	struct nouveau_clk *clk = (p);                                       \
	_nouveau_clk_fini(nv_object(clk), (s));                              \
})

int  nouveau_clk_create_(struct nouveau_object *, struct nouveau_object *,
			   struct nouveau_oclass *,
			   struct nouveau_domain *, struct nouveau_pstate *,
			   int, bool, int, void **);
void _nouveau_clk_dtor(struct nouveau_object *);
int  _nouveau_clk_init(struct nouveau_object *);
int  _nouveau_clk_fini(struct nouveau_object *, bool);

extern struct nouveau_oclass nv04_clk_oclass;
extern struct nouveau_oclass nv40_clk_oclass;
extern struct nouveau_oclass *nv50_clk_oclass;
extern struct nouveau_oclass *nv84_clk_oclass;
extern struct nouveau_oclass *nvaa_clk_oclass;
extern struct nouveau_oclass nva3_clk_oclass;
extern struct nouveau_oclass nvc0_clk_oclass;
extern struct nouveau_oclass nve0_clk_oclass;
extern struct nouveau_oclass gk20a_clk_oclass;

int nv04_clk_pll_set(struct nouveau_clk *, u32 type, u32 freq);
int nv04_clk_pll_calc(struct nouveau_clk *, struct nvbios_pll *,
			int clk, struct nouveau_pll_vals *);
int nv04_clk_pll_prog(struct nouveau_clk *, u32 reg1,
			struct nouveau_pll_vals *);
int nva3_clk_pll_calc(struct nouveau_clk *, struct nvbios_pll *,
			int clk, struct nouveau_pll_vals *);

int nouveau_clk_ustate(struct nouveau_clk *, int req, int pwr);
int nouveau_clk_astate(struct nouveau_clk *, int req, int rel, bool wait);
int nouveau_clk_dstate(struct nouveau_clk *, int req, int rel);
int nouveau_clk_tstate(struct nouveau_clk *, int req, int rel);

#endif
