#ifndef __NVKM_SW_NV50_H__
#define __NVKM_SW_NV50_H__

#include <engine/sw.h>

struct nv50_sw_oclass {
	struct nouveau_oclass base;
	struct nouveau_oclass *cclass;
	struct nouveau_oclass *sclass;
};

struct nv50_sw_priv {
	struct nouveau_sw base;
};

int  nv50_sw_ctor(struct nouveau_object *, struct nouveau_object *,
			struct nouveau_oclass *, void *, u32,
			struct nouveau_object **);

struct nv50_sw_cclass {
	struct nouveau_oclass base;
	int (*vblank)(struct nvkm_notify *);
};

struct nv50_sw_chan {
	struct nouveau_sw_chan base;
	struct {
		struct nvkm_notify notify[4];
		u32 channel;
		u32 ctxdma;
		u64 offset;
		u32 value;
	} vblank;
};

int  nv50_sw_context_ctor(struct nouveau_object *,
				struct nouveau_object *,
				struct nouveau_oclass *, void *, u32,
				struct nouveau_object **);
void nv50_sw_context_dtor(struct nouveau_object *);

int nv50_sw_mthd_vblsem_value(struct nouveau_object *, u32, void *, u32);
int nv50_sw_mthd_vblsem_release(struct nouveau_object *, u32, void *, u32);
int nv50_sw_mthd_flip(struct nouveau_object *, u32, void *, u32);

#endif