

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
	UxVoid EnterRoom( UxInt32 id, std::string name );

public:
	UxBool IsEmpty();
	UxBool IsAlive();
	UxInt32 GetId();
	std::string GetName();
	UxBool GetReady();
	UxInt32 GetHeartNum();
	UxSingle GetPosX();
	UxSingle GetPosY();
	UxSingle GetSpeed();
	UxSingle GetRot();
	UxInt8 GetCharacterType();
	UxInt8 GetAnimation();
	UxBool IsAnimChange();
	UxBool IsInvincible();

	UxVoid SetReady( UxBool input );
	UxVoid SetPos( UxSingle x, UxSingle y );
	UxVoid SetSpeed( UxSingle speed );
	UxVoid SetRot( UxSingle rot );
	UxVoid SetAnim( UxInt8 anim );
	UxVoid SetCharacter( UxInt8 character );
	UxVoid SetInvincible( UxBool invincible );
	UxVoid SetDie();
	UxVoid DeductHeart();

private:
	//m_clients의 key값과 동일
	UxInt32 m_id;
	std::string m_name;

	UxInt32 m_heart;

	UxBool m_isEmpty;
	UxBool m_isReady;
	UxBool m_isAlive;

	UxBool m_invincible;

	UxInt8 m_character;
	UxInt8 m_animation;
	UxInt8 m_preAnimation;

	UxSingle m_x;
	UxSingle m_y;
	UxSingle m_speed;

	UxSingle m_rot;
};
