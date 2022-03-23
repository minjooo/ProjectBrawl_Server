

#include "Protocol.h"
#include "Global.h"
#include "Header.h"
#include "TypeDef.h"
#include "Player.h"


Player::Player()
{
	Initialize();
}

Player::~Player()
{

}

UxVoid Player::EnterRoom( UxInt32 id, std::wstring name )
{
	m_isEmpty = false;
	m_id = id;
	m_name = name;
}

UxVoid Player::Initialize()
{
	m_isEmpty = true;
	m_isReady = false;
	m_isAlive = true;
	m_heart = maxHeart;

	m_character = CHARACTER_DESTROYER;
	m_animation = ANIM_IDLE;

	m_id = -1;
	m_name.clear();
	m_x = 160.0;
	m_y = 670.0;
}

UxVoid Player::Reset()
{
	//게임 끝났을 때 게임룸으로 돌아가면 변경 필요
	m_isEmpty = true;
	m_isReady = false;
	m_isAlive = true;
	m_heart = maxHeart;

	m_character = CHARACTER_DESTROYER;
	m_animation = ANIM_IDLE;

	//m_id = -1;
	m_name.clear();
}

UxVoid Player::Update()
{
	//필요
}

UxBool Player::IsEmpty()
{
	return m_isEmpty;
}

UxBool Player::IsAlive()
{
	return m_isAlive;
}

UxInt32 Player::GetId()
{
	return m_id;
}

UxInt32 Player::GetHeartNum()
{
	return m_heart;
}

UxSingle Player::GetPosX()
{
	return m_x;
}

UxSingle Player::GetPosY()
{
	return m_y;
}

UxSingle Player::GetSpeed()
{
	return m_speed;
}

UxSingle Player::GetRot()
{
	return m_rot;
}

std::wstring Player::GetName()
{
	return m_name;
}

UxInt8 Player::GetCharacterType()
{
	return m_character;
}

UxInt32 Player::GetAnimation()
{
	return m_animation;
}

UxBool Player::IsInvincible()
{
	return m_invincible;
}

UxBool Player::IsSkillCoolReset()
{
	return m_skillCool;
}

UxVoid Player::SetPos( UxSingle x, UxSingle y )
{
	m_x = x;
	m_y = y;
}

UxVoid Player::SetSpeed( UxSingle speed )
{
	m_speed = speed;
}

UxVoid Player::SetRot( UxSingle rot )
{
	m_rot = rot;
}

UxVoid Player::SetAnim( UxInt32 anim )
{
	m_animation = anim;
}

UxVoid Player::SetCharacter( UxInt8 character )
{
	m_character = character;
}

UxVoid Player::SetInvincible( UxBool invincible )
{
	m_invincible = invincible;
}

UxVoid Player::SetSkillCool( UxBool skillCool )
{
	m_skillCool = skillCool;
}

UxVoid Player::SetDie()
{
	m_isAlive = false;
}

UxVoid Player::DeductHeart()
{
	--m_heart;
}

UxBool Player::GetReady()
{
	return m_isReady;
}

UxVoid Player::SetReady( UxBool input )
{
	m_isReady = input;
}
