

#pragma once


#define SERVER_IP	"182.212.60.37"
//#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	3500


////메인화면
//client -> server
#define CS_LOGIN			1
#define	CS_JOIN_GAME		2	//게임 입장 눌렀을 때
#define CS_EXIT_GAME		3
//server -> client
#define SC_LOGIN_OK			1
#define SC_LOGIN_DENY		2
#define SC_JOIN_GAME_OK		5
#define SC_JOIN_GAME_DENY	6

////방 목록 화면 (로비)
//client -> server
#define	CS_MAKE_ROOM		10
#define CS_JOIN_ROOM		11
#define CS_ROOM_LIST		12
//server -> client
#define SC_MAKE_ROOM_OK		10
#define SC_MAKE_ROOM_DENY	11
#define SC_JOIN_ROOM_OK		12
#define SC_JOIN_ROOM_DENY	13
#define SC_ROOM_LIST		14
#define SC_ROOM_USER_LIST	15

////캐릭터 선택
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

////게임 진행
//animation type
#define ANIM_IDLE			1
#define ANIM_RUN			2
#define ANIM_JUMP			3
#define ANIM_ATTACK			4
#define ANIM_KNOCKBACK		5
#define ANIM_KNOCKDOWN		7
#define ANIM_DIE			6
//hit type
#define HIT_KNOCKBACK		1
#define HIT_KNOCKDOWN		2
//client -> server
#define CS_POSITION			30
#define CS_ROTATE			31 //수정 여부 있음
#define CS_ANIMATION		32
#define CS_ATTACK			33
#define CS_DEDUCT_HEART		34 //떨어져서 피 까였을 때
#define CS_DIE				35 //하트 하나도 없음
//server -> client
#define SC_POSITION			30
#define SC_ROTATE			31
#define SC_ANIMATOIN		32
#define SC_HIT				33 //스킬 맞음
#define SC_DEDUCT_HEART		35
#define SC_DIE				36
#define SC_GAME_OVER		37
#define SC_LEFT_TIME		40
#define SC_RESET_COOLTIME	41

////결과창
//client -> server
//server -> client

#pragma pack (push, 1)
//======================== main ==========================
struct csPacketLogin
{
	char size;
	char type;

	wchar_t id[24];
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

	int id;
	wchar_t name[24];
};

struct scPacketLoginDeny
{
	char size;
	char type;
};

struct scPacketJoinGameOk
{
	char size;
	char type;
};

struct scPacketJoinGameDeny
{
	char size;
	char type;
};

//======================== lobby ==========================
struct csPacketMakeRoom
{
	char size;
	char type;

	wchar_t name[24];
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

	wchar_t roomName[24];
};

struct scPacketRoomUserList
{
	char size;
	char type;

	int	id;
	wchar_t name[24];
	char character_type;
	char isReady;
};

struct scPacketJoinRoomDeny
{
	char size;
	char type;
};

struct scPacketRoomList
{
	char size;
	char type;

	char id;
	wchar_t name[24];
	char participant;
};
//totalNum에 총 갯수를 보냅니다
//항상 5개를 보내지만 예를들어 3개만 있는경우 나머지의 id는 -1이 들갑니다
//예를 들어 totalNum이 8이 오면 5개/3개 총 두번 보낼 예정입니다

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

struct PTC_Player
{
	int id;

	float x;
	float y;
};

struct scPacketGameStart
{
	char size;
	char type;

	PTC_Player player_list[4];
};

//======================== in Game ==========================

struct csPacketPosition
{
	char size;
	char type;

	float x;
	float y;
	float speed;
};

struct csPacketRotation
{
	char size;
	char type;

	float z;
};

struct csPacketAnimation
{
	char size;
	char type;

	int anim;
};

struct csPacketAttack
{
	char size;
	char type;
};

struct csPacketDeductHeart
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
	float speed;
};

struct scPacketRotation
{
	char size;
	char type;

	int id;
	float z;
};

struct scPacketAnimation
{
	char size;
	char type;

	int id;
	int anim;
};

struct scPacketHit
{
	char size;
	char type;

	int id;
	char char_type;
	char hit_type;
};

struct scPacketResetCooltime
{
	char size;
	char type;
};

struct scPacketDeductHeart
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

struct PTC_Winner
{
	int id;
	char isWin;//1 or 0
};

struct scGameOver
{
	char size;
	char type;

	PTC_Winner winner[4];
};

struct scLeftTime
{
	char size;
	char type;

	unsigned char leftTime;
};

#pragma pack (pop)
