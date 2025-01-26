//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "npcevent.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "game.h"
#include "in_buttons.h"
#include "ai_memory.h"
#include "soundent.h"
#include "rumble_shared.h"
#include "gamestats.h"
#include "Sprite.h"
#include "beam_shared.h"
#include "weapon_m4.h"
#include "weapon_rpg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_plr_dmg_smg1_grenade;
#define	M4_BEAM_SPRITE		"effects/laser1_noz.vmt"
#define	M4_LASER_SPRITE	"sprites/redglow1.vmt"


IMPLEMENT_SERVERCLASS_ST(CWeaponM4, DT_WeaponM4)
/*SendPropVector(SENDINFO_NAME(m_targetPosition, m_targetPosition), -1, SPROP_COORD),
SendPropVector(SENDINFO_NAME(m_worldPosition, m_worldPosition), -1, SPROP_COORD),
SendPropInt(SENDINFO(m_active), 1, SPROP_UNSIGNED),
SendPropModelIndex(SENDINFO(m_viewmodelIndex)),*/
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_m4, CWeaponM4);
PRECACHE_WEAPON_REGISTER(weapon_m4);

BEGIN_DATADESC(CWeaponM4)
/*DEFINE_FIELD(m_active, FIELD_INTEGER),
DEFINE_FIELD(m_viewmodelIndex, FIELD_INTEGER),
DEFINE_FIELD(m_targetPosition, FIELD_POSITION_VECTOR),
DEFINE_FIELD(m_worldPosition, FIELD_POSITION_VECTOR),*/
DEFINE_FIELD(m_bInitialStateUpdate, FIELD_BOOLEAN),
END_DATADESC()

acttable_t CWeaponM4::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_SMG1,			true },
	{ ACT_RELOAD,					ACT_RELOAD_SMG1,				true },
	{ ACT_IDLE,						ACT_IDLE_SMG1,					true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG1,			true },

	{ ACT_WALK,						ACT_WALK_RIFLE,					true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true  },

	// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_SMG1_RELAXED,			false }, // Never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_SMG1_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_SMG1,			false }, // Always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_RIFLE_RELAXED,			false }, // Never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_RIFLE_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_RIFLE,				false }, // Always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_RIFLE_RELAXED,			false }, // Never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_RIFLE_STIMULATED,		false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_RIFLE,				false }, // Always aims

	// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_SMG1_RELAXED,			false }, // Never aims
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_RIFLE_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_SMG1,			false }, // Always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_RIFLE_RELAXED,			false }, // Never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_RIFLE_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_RIFLE,				false }, // Always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_RIFLE_RELAXED,			false }, // Never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_RIFLE_STIMULATED,	false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_RIFLE,				false }, // Always aims
	// End readiness activities

	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		true },
	{ ACT_RUN,						ACT_RUN_RIFLE,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_RIFLE,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_SMG1,	true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_SMG1_LOW,		true },
	{ ACT_COVER_LOW,				ACT_COVER_SMG1_LOW,				false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_SMG1_LOW,			false },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_SMG1_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SMG1,		true },
};

IMPLEMENT_ACTTABLE(CWeaponM4);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponM4::CWeaponM4()
{
	 m_bInitialStateUpdate = false;
	m_fMinRange1 = 0; // No minimum range
}

CWeaponM4::~CWeaponM4()
{
}

/*void CWeaponM4::CreateLaserPointer(void)
{
	if (m_hLaserDot != NULL)
		return;

	m_hLaserDot = CLaserDot::Create(GetAbsOrigin(), GetOwnerEntity());
	m_hLaserDot->TurnOff();

	UpdateLaserPosition();
}*/

bool CWeaponM4::Deploy(void)
{
	m_bInitialStateUpdate = true;
	return BaseClass::Deploy();
}

void CWeaponM4::ItemPostFrame(void) {
	BaseClass::ItemPostFrame();

	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;
	/*if ((m_bInitialStateUpdate) && (GetActivity() != ACT_VM_DRAW))
	{

	}
	if (!m_active) {
		m_active = true;
	}
	else {
		DrawBeam();
	}*/
}

/*void CWeaponM4::DrawBeam()
{
	m_active = true;
	Vector start, angles, right, up, forward;
	trace_t tr;

	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	m_viewmodelIndex = pPlayer->entindex();
	CBaseViewModel* vm = pPlayer->GetViewModel();
	if (vm) 
		m_viewmodelIndex = vm->entindex();
	pPlayer->EyeVectors(&forward, &right, &up);
	start = pPlayer->Weapon_ShootPosition();
	Vector aimDir = pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector end = start + forward * 4096;

	VectorVectors(aimDir, right, up);
	UTIL_TraceLine(start, end, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr);
	end = m_worldPosition = tr.endpos;

	m_targetPosition = start + forward * 25;
	
	CBaseEntity* pHit = tr.m_pEnt;
	if (tr.DidHitWorld()) {

	}
}*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponM4::Precache()
{
	BaseClass::Precache();

	PrecacheModel("sprites/redglow1.vmt");
	PrecacheModel(M4_LASER_SPRITE);
	g_LaserDot = PrecacheModel(M4_BEAM_SPRITE);
}

//-----------------------------------------------------------------------------
// Purpose: Give this weapon longer range when wielded by an ally NPC.
//-----------------------------------------------------------------------------
void CWeaponM4::Equip(CBaseCombatCharacter* pOwner)
{
	if (pOwner->Classify() == CLASS_PLAYER_ALLY)
		m_fMaxRange1 = 3000;
	else
		m_fMaxRange1 = 1400;

	BaseClass::Equip(pOwner);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponM4::FireNPCPrimaryAttack(CBaseCombatCharacter* pOperator, Vector& vecShootOrigin, Vector& vecShootDir)
{
	// FIXME: Use the returned number of bullets to account for >10hz firerate
	WeaponSoundRealtime(SINGLE_NPC);

	CSoundEnt::InsertSound(SOUND_COMBAT | SOUND_CONTEXT_GUNFIRE, pOperator->GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pOperator, SOUNDENT_CHANNEL_WEAPON, pOperator->GetEnemy());
	pOperator->FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2, entindex(), 0);

	pOperator->DoMuzzleFlash();
	m_iClip1--;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponM4::Operator_ForceNPCFire(CBaseCombatCharacter* pOperator, bool bSecondary)
{
	// Ensure we have enough rounds in the magazine
	m_iClip1++;

	Vector vecShootOrigin, vecShootDir;
	QAngle angShootDir;
	GetAttachment(LookupAttachment("muzzle"), vecShootOrigin, angShootDir);
	AngleVectors(angShootDir, &vecShootDir);
	FireNPCPrimaryAttack(pOperator, vecShootOrigin, vecShootDir);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponM4::Operator_HandleAnimEvent(animevent_t* pEvent, CBaseCombatCharacter* pOperator)
{
	switch (pEvent->event)
	{
	case EVENT_WEAPON_SMG1:
	{
		Vector vecShootOrigin, vecShootDir;
		QAngle angDiscard;

		// Support old style attachment point firing
		if ((pEvent->options == NULL) || (pEvent->options[0] == '\0') || (!pOperator->GetAttachment(pEvent->options, vecShootOrigin, angDiscard)))
			vecShootOrigin = pOperator->Weapon_ShootPosition();

		CAI_BaseNPC* npc = pOperator->MyNPCPointer();
		ASSERT(npc != NULL);
		vecShootDir = npc->GetActualShootTrajectory(vecShootOrigin);

		FireNPCPrimaryAttack(pOperator, vecShootOrigin, vecShootDir);
	}
	break;

	default:
		BaseClass::Operator_HandleAnimEvent(pEvent, pOperator);
		break;
	}
}

void CWeaponM4::DrawHitmarker()
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	trace_t tr;
	if (pPlayer)
	{
		
#ifndef CLIENT_DLL
				CSingleUserRecipientFilter filter(pPlayer);
				UserMessageBegin(filter, "ShowHitmarker");
				WRITE_BYTE(1);
				MessageEnd();
#endif
	}
}

void CWeaponM4::PrimaryAttack(void) {
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());
	if (pOwner)
	{
		trace_t tr;
		Vector vecStart, vecStop, vecDir;

		// Get the angles
		AngleVectors(pOwner->EyeAngles(), &vecDir);

		// Get the vectors
		vecStart = pOwner->Weapon_ShootPosition();
		vecStop = vecStart + vecDir * MAX_TRACE_LENGTH;
		// Do the TraceLine
		UTIL_TraceLine(vecStart, vecStop, MASK_ALL, pOwner, COLLISION_GROUP_NONE, &tr);

		
			if (tr.m_pEnt->IsNPC()) {
				EmitSound("Hit.Hitmarker");
				DrawHitmarker();
			}
			else {
				StopSound("Hit.Hitmarker");
			}
		
	}

	BaseClass::PrimaryAttack();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponM4::GetPrimaryAttackActivity()
{
	//if (m_nShotsFired < 2)
		return ACT_VM_PRIMARYATTACK;

	//if (m_nShotsFired < 3)
		//return ACT_VM_RECOIL1;

	//if (m_nShotsFired < 4)
		//return ACT_VM_RECOIL2;

	//return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponM4::Reload()
{
	bool fRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (fRet)
		WeaponSound(RELOAD);

	return fRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
/*void CWeaponM4::AddViewKick()
{
#define EASY_DAMPEN			2.3f
#define MAX_VERTICAL_KICK	17.0f // Degrees
#define SLIDE_LIMIT			0.11f // Seconds

	// Get the view kick
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	DoMachineGunKick(pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT);
}*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponM4::SecondaryAttack()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flDot -
//          flDist -
// Output : int
//-----------------------------------------------------------------------------
int CWeaponM4::WeaponRangeAttack2Condition(float flDot, float flDist)
{
	return COND_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t* CWeaponM4::GetProficiencyValues()
{
	static WeaponProficiencyInfo_t proficiencyTable[] =
	{
		{ 7.0,		0.75	},
		{ 5.00,		0.75	},
		{ 10.0 / 3.0, 0.75	},
		{ 5.0 / 3.0,	0.75	},
		{ 1.00,		1.0		},
	};

	COMPILE_TIME_ASSERT(ARRAYSIZE(proficiencyTable) == WEAPON_PROFICIENCY_PERFECT + 1);

	return proficiencyTable;
}

/*CLaserDot::CLaserDot(void)
{
	m_hTargetEnt = NULL;
	m_bIsOn = true;
	g_LaserDotList.Insert(this);
}

CLaserDot::~CLaserDot(void)
{
	g_LaserDotList.Remove(this);
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
// Output : CLaserDot
//-----------------------------------------------------------------------------
CLaserDot* CLaserDot::Create(const Vector& origin, CBaseEntity* pOwner, bool bVisibleDot)
{
	CLaserDot* pLaserDot = (CLaserDot*)CBaseEntity::Create("env_laserdot", origin, QAngle(0, 0, 0));

	if (pLaserDot == NULL)
		return NULL;

	pLaserDot->m_bVisibleLaserDot = bVisibleDot;
	pLaserDot->SetMoveType(MOVETYPE_NONE);
	pLaserDot->AddSolidFlags(FSOLID_NOT_SOLID);
	pLaserDot->AddEffects(EF_NOSHADOW);
	UTIL_SetSize(pLaserDot, vec3_origin, vec3_origin);

	//Create the graphic
	pLaserDot->SpriteInit("sprites/redglow1.vmt", origin);

	pLaserDot->SetName(AllocPooledString("TEST"));

	pLaserDot->SetTransparency(kRenderGlow, 255, 255, 255, 255, kRenderFxNoDissipation);
	pLaserDot->SetScale(0.5f);

	pLaserDot->SetOwnerEntity(pOwner);

	pLaserDot->SetContextThink(&CLaserDot::LaserThink, gpGlobals->curtime + 0.1f, g_pLaserDotThink);
	pLaserDot->SetSimulatedEveryTick(true);

	if (!bVisibleDot)
	{
		pLaserDot->MakeInvisible();
	}

	return pLaserDot;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLaserDot::LaserThink(void)
{
	SetNextThink(gpGlobals->curtime + 0.05f, g_pLaserDotThink);

	if (GetOwnerEntity() == NULL)
		return;

	Vector	viewDir = GetAbsOrigin() - GetOwnerEntity()->GetAbsOrigin();
	float	dist = VectorNormalize(viewDir);

	float	scale = RemapVal(dist, 32, 1024, 0.01f, 0.5f);
	float	scaleOffs = random->RandomFloat(-scale * 0.25f, scale * 0.25f);

	scale = clamp(scale + scaleOffs, 0.1f, 32.0f);

	SetScale(scale);
}

void CLaserDot::SetLaserPosition(const Vector& origin, const Vector& normal)
{
	SetAbsOrigin(origin);
	m_vecSurfaceNormal = normal;
}

Vector CLaserDot::GetChasePosition()
{
	return GetAbsOrigin() - m_vecSurfaceNormal * 10;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLaserDot::TurnOn(void)
{
	m_bIsOn = true;
	if (m_bVisibleLaserDot)
	{
		BaseClass::TurnOn();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLaserDot::TurnOff(void)
{
	m_bIsOn = false;
	if (m_bVisibleLaserDot)
	{
		BaseClass::TurnOff();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CLaserDot::MakeInvisible(void)
{
	BaseClass::TurnOff();
}
*/