//=============================================================================//
//
// Purpose:		Base class for Bullsquid and other predatory aliens.
//
//				
//				
//				
//
//=============================================================================//
#ifndef NPC_BASEPREDATOR_H
#define NPC_BASEPREDATOR_H

#include "ai_basenpc.h"
#include "ai_squadslot.h"

enum BossState
{
	BOSS_STATE_NORMAL,
	BOSS_STATE_RETREAT,
	BOSS_STATE_BERSERK
};

enum WanderState
{
	WANDER_STATE_NEVER = 0,
	WANDER_STATE_ALWAYS,
	WANDER_STATE_ALERT_ONLY,
	WANDER_STATE_IDLE_ONLY
};

//=========================================================
// monster-specific tasks
//=========================================================
enum
{
	TASK_PREDATOR_HOPTURN = LAST_SHARED_TASK,
	TASK_PREDATOR_EAT,
	TASK_PREDATOR_REGEN,
	TASK_PREDATOR_PLAY_EXCITE_ACT,
	TASK_PREDATOR_PLAY_EAT_ACT,
	TASK_PREDATOR_PLAY_SNIFF_ACT,
	TASK_PREDATOR_PLAY_INSPECT_ACT,
	TASK_PREDATOR_SPAWN,
	LAST_SHARED_PREDATOR_TASK
};

//=========================================================
// monster-specific schedule types
//=========================================================
enum
{
	SCHED_PREDATOR_HURTHOP = LAST_SHARED_SCHEDULE + 1,
	SCHED_PREDATOR_SEE_PREY,
	SCHED_PREDATOR_EAT,
	SCHED_PREDATOR_RUN_EAT,
	SCHED_PRED_SNIFF_AND_EAT,
	SCHED_PREDATOR_WALLOW,
	SCHED_PREDATOR_WANDER, // Similar to SCHED_PATROL_WALK, but with more interrupts for the [REDACTED]
	SCHED_PREDATOR_SPAWN,
	LAST_SHARED_SCHEDULE_PREDATOR
};

//-----------------------------------------------------------------------------
// Predator Conditions
//-----------------------------------------------------------------------------
enum
{
	COND_PREDATOR_SMELL_FOOD	= LAST_SHARED_CONDITION + 1,
	COND_NEW_BOSS_STATE,
	NEXT_CONDITION
};

// -----------------------------------------------
//	> Squad slots
// -----------------------------------------------
enum SquadSlot_T
{
	SQUAD_SLOT_FEED = LAST_SHARED_SQUADSLOT,
	SQUAD_SLOT_THREAT_DISPLAY
};


//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		PREDATOR_AE_SPIT		( 1 )
#define		PREDATOR_AE_BITE		( 2 )
#define		PREDATOR_AE_BLINK		( 3 )
#define		PREDATOR_AE_ROAR		( 4 )
#define		PREDATOR_AE_HOP			( 5 )
#define		PREDATOR_AE_THROW		( 6 )
#define		PREDATOR_AE_WHIP_SND	( 7 )
#define		PREDATOR_AE_TAILWHIP	( 8 )

class CNPC_BasePredator : public CAI_BaseNPC
{
	DECLARE_CLASS( CNPC_BasePredator, CAI_BaseNPC );
	DECLARE_DATADESC();

public:
	CNPC_BasePredator();
	void Activate( void );

	virtual bool IsBoss( void ) { return m_bIsBoss; } // Method returns whether or not this monster is a boss enemy

	virtual void GrowlSound( void ) {};
	virtual void RetreatModeSound( void ) {};
	virtual void BerserkModeSound( void ) {};

	float MaxYawSpeed ( void );

	virtual int RangeAttack1Conditions( float flDot, float flDist );
	virtual int MeleeAttack1Conditions( float flDot, float flDist );
	virtual int MeleeAttack2Conditions( float flDot, float flDist );

	virtual float GetMaxSpitWaitTime( void ) { return 0.0f; };
	virtual float GetMinSpitWaitTime( void ) { return 0.0f; };
	virtual float GetWhipDamage( void ) { return 0.0f; };
	virtual float GetSprintDistance( void ) { return 256.0f; };

	void Event_Killed(const CTakeDamageInfo &info);

	bool IsJumpLegal( const Vector &startPos, const Vector &apex, const Vector &endPos, float maxUp, float maxDown, float maxDist ) const; // For inheritance reasons, need to pass this through to base class

	mutable float	m_flJumpDist;

	bool FValidateHintType ( CAI_Hint *pHint );
	virtual void RemoveIgnoredConditions( void );
	Disposition_t IRelationType( CBaseEntity *pTarget );
	int OnTakeDamage_Alive( const CTakeDamageInfo &inputInfo );
	virtual void OnFed();

	virtual bool IsPrey( CBaseEntity* pTarget ) { return false; } // Override this method to choose an NPC to get excited about - like bullsquids with headcrabs
	virtual bool IsSameSpecies( CBaseEntity* pTarget ) { return Classify() == pTarget->Classify(); } // Is this NPC the same species as me?
	virtual bool ShouldInfight( CBaseEntity * pTarget ) { return false;  } // Is this NPC a rival or a potential mate that I should fight against?
	virtual bool ShouldFindMate();
	virtual bool CanMateWithTarget( CNPC_BasePredator * pTarget, bool receiving );

	virtual bool IsBaby() { return m_bIsBaby; };
	virtual void setIsBaby( bool bIsBaby ) { m_bIsBaby = bIsBaby; };

	int GetSoundInterests ( void );
	void RunAI ( void );
	virtual void OnListened ( void );

	virtual int SelectSchedule( void );
	virtual int SelectBossSchedule( void );
	virtual int TranslateSchedule( int scheduleType );

	bool ShouldAlwaysThink(); // Overriding for boss behavior
	bool FInViewCone ( Vector pOrigin );
	bool FVisible ( Vector vecOrigin );

	virtual void StartTask ( const Task_t *pTask );
	virtual void RunTask ( const Task_t *pTask );

	NPC_STATE SelectIdealState ( void );

	bool 			OverrideMove( float flInterval );			// Override to take total control of movement (return true if done so)
	bool			ShouldInvestigateSounds( void ) { return !m_bIsBaby || BaseClass::ShouldInvestigateSounds(); }	// 1upD - Adult predators always investigate sounds
	virtual bool	ShouldAvoidGoo( void ) { return m_tEzVariant != EZ_VARIANT_RAD; } // Don't avoid goo if the slime variant

	DEFINE_CUSTOM_AI;

protected:
	bool  m_fCanThreatDisplay;// this is so the squid only does the "I see a headcrab!" dance one time. 
	float m_flLastHurtTime;// we keep track of this, because if something hurts a squid, it will forget about its love of headcrabs for a while.
	float m_flNextSpitTime;// last time the bullsquid used the spit attack.
	float m_flHungryTime;// set this is a future time to stop the monster from eating for a while. 
	float m_flNextSpawnTime; // Next time the bullsquid can birth offspring

	// Spawning offspring
	bool  m_bSpawningEnabled;
	float m_iTimesFed;
	bool m_bReadyToSpawn; // Signals the predator NPC can now spawn offspring
	bool m_bIsBaby; // Is this a baby squid? (Or other baby monster)
	bool m_tEzVariant; //taz
	string_t m_iszMate; // Name of mate
	EHANDLE	m_hMate; // Current mate

	static string_t gm_iszGooPuddle;

	virtual void PredMsg( const tchar * pMsg ); // Print a message to the developer console
	float m_flNextPredMsgTime; // Don't print messages constantly

public:
	void			InputSpawnNPC( inputdata_t &inputdata );
	void			InputEnableSpawning( inputdata_t &inputdata );
	void			InputDisableSpawning( inputdata_t &inputdata );

	// Wander inputs
	void			InputSetWanderAlways( inputdata_t &inputdata );
	void			InputSetWanderNever( inputdata_t &inputdata );


// Boss stuff
	void			InputEnterNormalMode(inputdata_t &inputdata);
	void			InputEnterRetreatMode(inputdata_t &inputdata);
	void			InputEnterBerserkMode(inputdata_t &inputdata);

protected:
	bool  m_bIsBoss = true; // Key value to set whether or not this monster should act like the 'boss' enemy 
	BossState m_tBossState = BOSS_STATE_NORMAL; // State of the boss
	COutputEvent m_OnBossHealthReset; // Output for when the boss loses all of his health - allows you to script boss fight through Hammer!
	COutputEvent m_OnFed; // Output when a predator completes feeding
	COutputEvent m_OnSpawnNPC; // Output when a predator completes spawning offspring

	WanderState m_tWanderState; // How should this NPC wander?
};
#endif // NPC_BASEPREDATOR_H