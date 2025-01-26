#include "basehlcombatweapon.h"
#include "Sprite.h"
#include "npcevent.h"
#include "beam_shared.h"

class CWeaponM4;
static int g_LaserDot;

//-----------------------------------------------------------------------------
// CWeaponM4
//-----------------------------------------------------------------------------
class CWeaponM4 : public CHLMachineGun
{
	DECLARE_DATADESC();

public:
	DECLARE_CLASS(CWeaponM4, CHLMachineGun);

	CWeaponM4();
	~CWeaponM4();

	DECLARE_SERVERCLASS();

	/*virtual bool Holster(CHLMachineGun* pSwitchingTo) {
		m_active = false;
		return BaseClass::Holster(pSwitchingTo);
	}*/
	void   	DrawBeam();
	void			Precache();
	//void			AddViewKick();
	void			SecondaryAttack();
	void            PrimaryAttack();
	void            DrawHitmarker();
	int				GetMinBurst() { return 2; }
	int				GetMaxBurst() { return 5; }
	bool            Deploy(void);
	virtual void	Equip(CBaseCombatCharacter* pOwner);
	bool			Reload();
	//void GetShootPosition(CLaserDot* pLaserDot, Vector* pShootPosition);
	float			GetFireRate() { return 0.075f; } // RPS, 60sec/800 rounds = 0.075
	int				CapabilitiesGet() { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	int				WeaponRangeAttack2Condition(float flDot, float flDist);
	Activity		GetPrimaryAttackActivity();
	void	ItemPostFrame(void);
	void	CreateLaserPointer(void);
	void	UpdateLaserPosition(Vector vecMuzzlePos = vec3_origin, Vector vecEndPos = vec3_origin);
	Vector	GetLaserPosition(void);
	void	StartLaserEffects(void);
	void	StopLaserEffects(void);
	void	UpdateLaserEffects(void);

	// NPC RPG users cheat and directly set the laser pointer's origin
	void	UpdateNPCLaserPosition(const Vector& vecTarget);
	void	SetNPCLaserPosition(const Vector& vecTarget);
	const Vector& GetNPCLaserPosition(void);

	// Values which allow our "spread" to change from button input from player
	virtual const Vector& GetBulletSpread()
	{
		// Define "spread" parameters based on the "owner" and what they are doing
		static Vector plrDuckCone = VECTOR_CONE_2DEGREES;
		static Vector plrStandCone = VECTOR_CONE_3DEGREES;
		static Vector plrMoveCone = VECTOR_CONE_4DEGREES;
		static Vector npcCone = VECTOR_CONE_5DEGREES;
		static Vector plrRunCone = VECTOR_CONE_6DEGREES;
		static Vector plrJumpCone = VECTOR_CONE_9DEGREES;

		if (GetOwner() && GetOwner()->IsNPC())
			return npcCone;

		//static Vector cone;

		// We must know the player "owns" the weapon before different cones may be used
		CBasePlayer* pPlayer = ToBasePlayer(GetOwnerEntity());
		if (pPlayer->m_nButtons & IN_DUCK)
			return plrDuckCone;
		if (pPlayer->m_nButtons & IN_FORWARD)
			return plrMoveCone;
		if (pPlayer->m_nButtons & IN_BACK)
			return plrMoveCone;
		if (pPlayer->m_nButtons & IN_MOVERIGHT)
			return plrMoveCone;
		if (pPlayer->m_nButtons & IN_MOVELEFT)
			return plrMoveCone;
		if (pPlayer->m_nButtons & IN_JUMP)
			return plrJumpCone;
		if (pPlayer->m_nButtons & IN_SPEED)
			return plrRunCone;
		if (pPlayer->m_nButtons & IN_RUN)
			return plrRunCone;
		else
			return plrStandCone;
	}

	const WeaponProficiencyInfo_t* GetProficiencyValues();

	void FireNPCPrimaryAttack(CBaseCombatCharacter* pOperator, Vector& vecShootOrigin, Vector& vecShootDir);
	void Operator_ForceNPCFire(CBaseCombatCharacter* pOperator, bool bSecondary);
	void Operator_HandleAnimEvent(animevent_t* pEvent, CBaseCombatCharacter* pOperator);

protected:
	/*CNetworkVector(m_targetPosition);
	CNetworkVector(m_worldPosition);
	CNetworkVar(int, m_active);
	CNetworkVar(int, m_viewmodelIndex);*/
	bool				m_bInitialStateUpdate;
	//CHandle<CLaserDot>	m_hLaserDot;
	//CHandle<CSprite>	m_hLaserMuzzleSprite;
	//CHandle<CBeam>		m_hLaserBeam;
public:
	DECLARE_ACTTABLE();
};
