//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Projectile shot from the AR2 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef	GRENADEAR2_H
#define	GRENADEAR2_H

#include "basegrenade_shared.h"

#define	MAX_AR2_NO_COLLIDE_TIME 0.2

class SmokeTrail;
class CWeaponAR2;
struct edict_t;
class CGrenadeAR2 : public CBaseGrenade
{
public:
	DECLARE_CLASS( CGrenadeAR2, CBaseGrenade );
	
	CHandle< SmokeTrail > m_hSmokeTrail;
	float				 m_fSpawnTime;
	float				m_fDangerRadius;
	Vector m_vecVelocity;
	
public:
	//void	SetVelocity(const Vector& velocity, const AngularImpulse& angVelocity);
	int m_iBounceCount;

private:
	bool m_isTouched;

	void		Spawn( void );
	//bool CreateVPhysics();
	//void VPhysicsUpdate(IPhysicsObject* pPhysics);
	void		Precache( void );
	void 		GrenadeAR2Touch( CBaseEntity *pOther );
	void		GrenadeAR2Think( void );
	void		Event_Killed( const CTakeDamageInfo &info );

public:
	void EXPORT				Detonate(void);
	CGrenadeAR2(void);
	

	DECLARE_DATADESC();
protected:
	bool	m_inSolid;
};
CBaseGrenade* GrenadeCreate(const Vector& position, const QAngle& angles, const Vector& velocity, const AngularImpulse& angVelocity, CBaseEntity* pOwner);

#endif	//GRENADEAR2_H
