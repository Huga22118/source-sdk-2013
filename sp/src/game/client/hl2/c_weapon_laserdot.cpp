#include "cbase.h"
#include "c_basehlcombatweapon.h"
#include "beamdraw.h"
#include "c_weapon__stubs.h"
#include "clienteffectprecachesystem.h"
#include "tier0/memdbgon.h"

CLIENTEFFECT_REGISTER_BEGIN(PrecacheEffectLDMFOUR)
CLIENTEFFECT_MATERIAL("sprites/redglow1.vmt")
CLIENTEFFECT_REGISTER_END()

class C_LDMFOUR : public CDefaultClientRenderable {
public:
	C_LDMFOUR();
	void Update(C_BaseEntity *pOwner);
	matrix3x4_t z;
	const matrix3x4_t& RenderableToWorldTransform() { return z; }
	virtual const Vector& GetRenderOrigin() { return m_worldPosition; }
	virtual const QAngle& GetRenderAngles() { return vec3_angle; }
	virtual bool ShouldDraw(void) { return true; }
	virtual bool IsTransparent(void) { return true; }
	virtual bool ShouldReceiveProjectedTextures(int flags) { return false; }
	virtual int DrawModel(int flags);
	virtual void GetRenderBounds(Vector& mins, Vector& maxs) 
	{ mins.Init(-32, -32, -32); maxs.Init(32, 32, 32); }

	C_BaseEntity* m_pOwner;
	Vector m_targetPosition;
	Vector m_worldPosition;
	int m_active;
	int m_viewmodelIndex;
};

class CWeaponM4 : public C_HLMachineGun {
	DECLARE_CLASS(CWeaponM4, C_HLMachineGun);
public:
	CWeaponM4() {}
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
	void OnDataChanged(DataUpdateType_t updateType) {
		BaseClass::OnDataChanged(updateType);
		m_LaserDot.Update(this);
		}
private:
	CWeaponM4(const CWeaponM4&);
	C_LDMFOUR m_LaserDot;
};

STUB_WEAPON_CLASS_IMPLEMENT(weapon_m4, CWeaponM4);
IMPLEMENT_CLIENTCLASS_DT(CWeaponM4, DT_WeaponM4, CWeaponM4)
RecvPropVector(RECVINFO_NAME(m_LaserDot.m_targetPosition, m_targetPosition)),
RecvPropVector(RECVINFO_NAME(m_LaserDot.m_worldPosition, m_worldPosition)),
RecvPropInt(RECVINFO_NAME(m_LaserDot.m_active, m_active)),
RecvPropInt(RECVINFO_NAME(m_LaserDot.m_viewmodelIndex, m_viewmodelIndex)),
END_RECV_TABLE()

C_LDMFOUR::C_LDMFOUR() {
	m_pOwner = NULL;
	m_hRenderHandle = INVALID_CLIENT_RENDER_HANDLE;
}

void C_LDMFOUR::Update(C_BaseEntity* pOwner) {
	m_pOwner = pOwner;
	if (m_active) {
		if (m_hRenderHandle == INVALID_CLIENT_RENDER_HANDLE) 
			ClientLeafSystem()->AddRenderable(this, RENDER_GROUP_TRANSLUCENT_ENTITY);
		else
			ClientLeafSystem()->RenderableChanged(m_hRenderHandle);
		
	}
	else if (!m_active && m_hRenderHandle != INVALID_CLIENT_RENDER_HANDLE) {
		ClientLeafSystem()->RemoveRenderable(m_hRenderHandle);
		m_hRenderHandle = INVALID_CLIENT_RENDER_HANDLE;
	}
}

int C_LDMFOUR::DrawModel(int flags) {
	Vector points[3];
	QAngle tmpAngle;
	if (!m_active) {
		return 0;
	}
	C_BaseEntity *pEnt = cl_entitylist->GetEnt(m_viewmodelIndex);
	if (!pEnt) {
		return 0;
	}

	pEnt->GetAttachment(1, points[0], tmpAngle);
	points[1] = 0.5 * (m_targetPosition + points[0]);
	points[1].z += 4 * sin(gpGlobals->curtime * 11) + 5 * cos(gpGlobals->curtime * 13);
	points[2] = m_worldPosition;

	IMaterial* pMat = materials->FindMaterial("sprites/redglow1.vmt", TEXTURE_GROUP_CLIENT_EFFECTS);
	Vector color;
	color.Init(0, 0, 1);
}