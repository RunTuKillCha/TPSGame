#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8 {
	ECS_Unoccupied UMETA(DisplayNeme = "Unoccupied"),
	ECS_Reloading UMETA(DisplayNeme = "Reloading"),


	ECS_Max UMETA(DisplayName = "枚举最大数值")
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8 {
	EET_Unarmed UMETA(DisplayName = "徒手"),
	EET_MainWeapon UMETA(DisplayName = "主武器"),
	EET_Pistol UMETA(DisplayName = "手枪"),
	EET_Knife UMETA(DisplayName = "刀"),


	EET_Max UMETA(DisplayName = "枚举最大数值")
};

UENUM(BlueprintType)
enum class EFaction : uint8 {
	EF_NULL UMETA(DisplayName = "未选择"),
	EF_CT UMETA(DisplayName = "反恐精英"),
	EF_T UMETA(DisplayName = "恐怖分子"),



	EET_Max UMETA(DisplayName = "枚举最大数值")
};

UENUM(BlueprintType)
enum class _ENUM_PACK_ : uint8{
    _ENUM_PACK_BASE,

    _ENUM_PACK_REGISTER_RQ = _ENUM_PACK_BASE,
    _ENUM_PACK_REGISTER_RS UMETA(DisplayName = "注册回复"),
    _ENUM_PACK_USER_INFO  UMETA(DisplayName = "用户信息"),
    _ENUM_PACK_LOGIN_RQ UMETA(DisplayName = "登录回复"),
    _ENUM_PACK_LOGIN_RS UMETA(DisplayName = "登录回复"),
    _ENUM_PACK_UPLOAD_FILE_RQ,
    _ENUM_PACK_UPLOAD_FILE_RS,
    _ENUM_PACK_FILE_CONTENT_RQ,
    _ENUM_PACK_FILE_CONTENT_RS,
    _ENUM_PACK_GET_FILRE_INFO_RQ,
    _ENUM_PACK_GET_FILRE_INFO_RS,
    _ENUM_PACK_DOWNLOAD_FILE_RQ,
    _ENUM_PACK_DOWNLOAD_FOLDER_RQ,
    _ENUM_PACK_DOWNLOAD_FILE_RS,
    _ENUM_PACK_FILE_HEADER_RQ,
    _ENUM_PACK_FILE_HEADER_RS,
    _ENUM_PACK_ADD_FOLDER_RQ,
    _ENUM_PACK_ADD_FOLDER_RS,
    _ENUM_PACK_QUICK_UPLOAD_RS,
    _ENUM_PACK_SHARE_FILE_RQ,
    _ENUM_PACK_SHARE_FILE_RS,
    _ENUM_PACK_MY_SHARE_RQ,
    _ENUM_PACK_MY_SHARE_RS,
    _ENUM_PACK_GET_SHARE_RQ,
    _ENUM_PACK_GET_SHARE_RS,
    _ENUM_PACK_DELETE_FILE_RQ,
    _ENUM_PACK_DELETE_FILE_RS,
    _ENUM_PACK_PLAYER_STARTER_RQ,


    _ENUM_PACK_COUNT UMETA(DisplayName = "枚举最大数值")
};

#define _MAX_SIZE (40)
typedef _ENUM_PACK_ PackType;

//返回的结果
//注册请求的结果
#define tel_is_exist        (0)
#define user_is_exist		(1)
#define register_success	(2)
//登录请求的结果
#define tel_not_exist		(0)
#define password_error		(1)
#define login_success		(2)

//登录
struct STRU_LOGIN_RQ {
    //登录需要: 手机号 密码 
    STRU_LOGIN_RQ() :type(PackType::_ENUM_PACK_LOGIN_RQ) {
        memset(tel, 0, sizeof(tel));
        memset(password, 0, sizeof(password));
    }
    PackType type;
    char tel[_MAX_SIZE];
    char password[_MAX_SIZE];

};// STRU_LOGIN_RQ;

struct STRU_LOGIN_RS
{
    // 需要 结果 , 用户的id
    STRU_LOGIN_RS() : type(PackType::_ENUM_PACK_LOGIN_RS), result(login_success), userid(0), port(0)
    {
        memset(Name, 0, sizeof(Name));
        memset(ip, 0, sizeof(ip));
        memset(LoginKey, 0, sizeof(LoginKey));
    }
    PackType type;
    int result;
    int userid;
    char Name[_MAX_SIZE];
    char ip[_MAX_SIZE];
    int port;
    char LoginKey[_MAX_SIZE];

};// STRU_LOGIN_RS;

//用户信息
#define status_online                      (0)
#define status_offer                      (1)
struct _STRU_USER_INFO {
    _STRU_USER_INFO() :type(PackType::_ENUM_PACK_USER_INFO), iconID(0), userID(0), status(status_offer) {
        memset(name, 0, _MAX_SIZE);
        memset(feelintg, 0, _MAX_SIZE);
    }
    PackType type;
    int iconID;
    long long userID;
    int status;
    char name[_MAX_SIZE];
    char feelintg[_MAX_SIZE];
};// _STRU_USER_INFO;


//开始游戏请求
struct _STRU_PLAYER_STARTER_RQ {
    _STRU_PLAYER_STARTER_RQ() :type(PackType::_ENUM_PACK_PLAYER_STARTER_RQ), userID(0) {
        memset(LoginKey, 0, sizeof(LoginKey));
    }
    PackType type;
    char LoginKey[_MAX_SIZE];
    long long userID;
};// _STRU_USER_INFO;