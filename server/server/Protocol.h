

#pragma once

#define SERVER_IP	"182.212.60.37"
#define SERVER_PORT	3500


////����ȭ��
//client -> server
#define CS_LOGIN			1
#define	CS_JOIN_GAME		2	//���� ���� ������ ��
#define CS_EXIT_GAME		3
//server -> client
#define SC_LOGIN_OK			1
#define SC_LOGIN_DENY		2
#define SC_JOIN_GAME_OK		5
#define SC_JOIN_GAME_DENY	6

////�� ��� ȭ�� (�κ�)
//client -> server
#define	CS_MAKE_ROOM		10
#define CS_JOIN_ROOM		11
#define CS_ROOM_LIST		12
//server -> client
#define SC_MAKE_ROOM_OK		10
#define SC_MAKE_ROOM_DENY	11
#define SC_JOIN_ROOM_OK		12
#define SC_JOIN_ROOM_DENY	13

////ĳ���� ����
#define CHARACTER_NONE		0
#define CHARACTER_DESTROYER	1
#define CHARACTER_ENGINEER	2
#define CHARACTER_SWORDSMAN	3
#define CHARACTER_WITCH		4
#define CHARACTER_ROGUE		5
//client -> server
#define CS_SELECT_CHARACTER	20
#define CS_READY			21
#define CS_UN_READY			22
#define CS_LEAVE_ROOM		23
//server -> client
#define SC_SELECT_CHARACTER	20
#define SC_READY			21
#define	SC_UN_READY			22
#define SC_LEAVE_ROOM		23
#define SC_GAME_START		24

////���� ����
//animation type
#define ANIM_IDLE			1
#define ANIM_RUN			2
#define ANIM_JUMP			3 //�������� �ִϸ��̼� ���� ���� �߰� �ʿ�
#define ANIM_ATTACK			4 //�ϴ� attack��...
#define ANIM_KNOCKBACK		5 //�ϴ� �˹鸸...
#define ANIM_DIE			6
//client -> server
#define CS_POSITION			30
#define CS_ROTATE			31 //���� ���� ����
#define CS_ANIMATION		32
#define CS_ATTACK			33
#define CS_HEART_DECREAS	34 //�������� �� ��� ��
#define CS_DIE				35 //��Ʈ �ϳ��� ����
//server -> client
#define SC_POSITION			30
#define SC_ROTATE			31
#define SC_ANIMATOIN		32
#define SC_HIT				33 //��ų ����
#define SC_HEART_DECREAS	35
#define SC_DIE				36
#define SC_GAME_OVER		37

////���â
//client -> server
//server -> client

#pragma pack (push, 1)
//======================== main ==========================
struct csPacketLogin
{
	char size;
	char type;

	char id[10];
};

struct csPacketJoinGame
{
	char size;
	char type;
};

struct csPacketExitGame
{
	char size;
	char type;
};

struct scPacketLoginOk
{
	char size;
	char type;
};

struct scPacketLoginDeny
{
	char size;
	char type;
};

struct scPacketGameOk
{
	char size;
	char type;
};

struct scPacketGameDeny
{
	char size;
	char type;
};

//======================== lobby ==========================
struct csPacketMakeRoom
{
	char size;
	char type;

	char name[10];
};

struct csPacketJoinRoom
{
	char size;
	char type;

	int roomNum;
};

struct csPacketRoomList
{
	char size;
	char type;
	//����Ʈ �߰� �ʿ�
};

struct scPacketMakeRoomOk
{
	char size;
	char type;

	int roomNum;
};

struct scPacketMakeRoomDeny
{
	char size;
	char type;
};

struct scPacketJoinRoomOk
{
	char size;
	char type;

	int id;
};

struct scPacketJoinRoomDeny
{
	char size;
	char type;
};

//======================== GameRoom ==========================
struct csPacketSelectCharacter
{
	char size;
	char type;

	char character;
};

struct csPacketReady
{
	char size;
	char type;
};

struct csPacketUnReady
{
	char size;
	char type;
};

struct csPacketLeaveRoom
{
	char size;
	char type;
};

struct scPacketSelectCharacter
{
	char size;
	char type;

	int id;
	char character;
};

struct scPacketReady
{
	char size;
	char type;

	int id;
};

struct scPacketUnReady
{
	char size;
	char type;

	int id;
};

struct scPacketLeaveRoom
{
	char size;
	char type;

	int id;
};

struct scPacketGameStart
{
	char size;
	char type;
};

//======================== in Game ==========================

struct csPacketPosition
{
	char size;
	char type;

	float x;
	float y;
};

struct csPacketRotation
{
	char size;
	char type;

	//�ϴ� ����
};

struct csPacketAnimation
{
	char size;
	char type;

	char anim;
};

struct csPacketAttack
{
	char size;
	char type;

	//�ϴ� ����
};

struct csPacketHeartDecrease
{
	char size;
	char type;
};

struct csPacketDie
{
	char size;
	char type;
};

struct scPacketPosition
{
	char size;
	char type;

	int id;
	float x;
	float y;
};

struct scPacketRotation
{
	char size;
	char type;
	//�ϴ� ����
};

struct scPacketAnimation
{
	char size;
	char type;

	int id;
	char anim;
};

struct scPacketHit
{
	char size;
	char type;
	//�ϴ� ����
};

struct scPacketHeartDecrease
{
	char size;
	char type;

	int id;
	int num;
};

struct scPacketDie
{
	char size;
	char type;

	int id;
};

struct scGameOver
{
	char size;
	char type;
};

#pragma pack (pop)
