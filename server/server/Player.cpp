

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

UxVoid Player::EnterRoom( UxInt32 id, std::string name )
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

	m_character = CHARACTER_NONE;
	m_animation = ANIM_IDLE;

	m_id = -1;
	m_name.clear();
	m_x = 160.0;
	m_y = 670.0;
}

UxVoid Player::Reset()
{
	//���� ������ �� ���ӷ����� ���ư��� ���� �ʿ�
	m_isEmpty = true;
	m_isReady = false;
	m_isAlive = true;
	m_heart = maxHeart;

	m_character = CHARACTER_NONE;
	m_animation = ANIM_IDLE;

	//m_id = -1;
	m_name.clear();
}

UxVoid Player::Update()
{
	//�ʿ�
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

std::string Player::GetName()
{
	return m_name;
}

UxInt8 Player::GetCharacterType()
{
	return m_character;
}

UxVoid Player::SetPos( UxSingle x, UxSingle y )
{
	m_x = x;
	m_y = y;
}

UxVoid Player::SetAnim( UxInt8 anim )
{
	m_animation = anim;
}

UxVoid Player::SetCharacter( UxInt8 character )
{
	m_character = character;
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
