

#pragma once


class Player
{
public:
	Player();
	~Player();

public:
	UxVoid Initialize();
	UxVoid Reset();
	UxVoid Update();
	UxVoid EnterRoom( UxInt32 id, std::wstring name );

public:
	UxBool IsEmpty();
	UxBool IsAlive();
	UxInt32 GetId();
	std::wstring GetName();
	UxBool GetReady();
	UxInt32 GetHeartNum();
	UxSingle GetPosX();
	UxSingle GetPosY();
	UxSingle GetSpeed();
	UxSingle GetRot();
	UxInt32 GetCharacterType();
	UxInt32 GetAnimation();
	UxBool IsInvincible();
	UxBool IsSkillCoolReset();

	UxVoid SetReady( UxBool input );
	UxVoid SetPos( UxSingle x, UxSingle y );
	UxVoid SetSpeed( UxSingle speed );
	UxVoid SetRot( UxSingle rot );
	UxVoid SetAnim( UxInt32 anim );
	UxVoid SetCharacter( UxInt32 character );
	UxVoid SetInvincible( UxBool invincible );
	UxVoid SetSkillCool( UxBool skillCool );
	UxVoid SetDie();
	UxVoid DeductHeart();

private:
	//m_clients의 key값과 동일
	UxInt32 m_id;
	std::wstring m_name;

	UxInt32 m_heart;

	UxBool m_isEmpty;
	UxBool m_isReady;
	UxBool m_isAlive;

	UxBool m_invincible;
	UxBool m_skillCool;

	UxInt32 m_character;
	UxInt32 m_animation;

	UxSingle m_x;
	UxSingle m_y;
	UxSingle m_speed;

	UxSingle m_rot;
};
