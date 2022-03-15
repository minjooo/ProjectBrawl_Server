

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
	UxBool GetReady();
	UxInt32 GetHeartNum();

	UxVoid SetReady( UxBool input );
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

	UxInt8 m_character;
	UxInt8 m_animation;

	//위치, 방향 가지고있어야 함
};
