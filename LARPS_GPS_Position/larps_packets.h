// Packet field positions

#define PKT_VERSION 0
#define PKT_LEN 1
#define PKT_FROM 2
#define PKT_TO 14
#define PKT_RELAY 26
#define PKT_COUNTER 38
#define PKT_TTL 39
#define PKT_OPTIONS 40
#define PKT_TYPE 41
#define PKT_DATA 42

// Packet Values

#define L_VERSION 0x01

// Packet Types

#define L_ACK                 0x01
#define L_POSITION_UPDATE     0x02
#define L_WEATHER_UPDATE      0x03
#define L_BULLETIN            0x04
#define L_EMERGENCY_ALERT     0x05
#define L_REAPTER_STATUS      0x06
#define L_REPEATER_DIRECTORY  0x07
#define L_TEXT_MESSAGE        0x08
#define L_CHAT_MSG            0x09
#define L_CHAT_DIR            0x0A
#define L_BROADCAST           0x0B
#define L_BEACON              0x0C
#define L_BBS_SEARCH          0x0D
#define L_BBS_RESPONSE        0x0E
#define L_ACTIVE_SESSION      0x0F
#define L_LAST_SEEN           0x10
#define L_CONTROL             0x11
#define L_BLOCK               0x12

// station types

#define S_HANDHELD            0x00
#define S_PEDESTRIAN          0x01
#define S_CAR                 0x02
#define S_TRUCK               0x03
#define S_VAN                 0x04
#define S_EMERG_VEHICLE       0x05
#define S_AMBULANCE           0x06
#define S_FIRE_TRUCK          0x07
#define S_COMMAND_VEHICLE     0x08
#define S_OFFICER             0x09
#define S_AIRCRAFT            0x0A
#define S_BOAT                0x0B
#define S_UAS_QUADCOPTER      0x0C
#define S_UAS_FIXEDWING       0x0D
#define S_BALLOON             0x0E
#define S_FLOAT               0x0F
#define S_LANDMARK            0x10
#define S_ROAD_CLOSED         0x11
#define S_ACCIDENT            0x12
#define S_HAZARD              0x13
#define S_PARIMETER           0x14

