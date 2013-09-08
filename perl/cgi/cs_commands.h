/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename  : cs_commands.h                                               **
** version   : 2                                                           **
** date      : September 20, 2007                                          **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2007,  C-RAD Imaging AB                                   **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 2
Date        : Oktober 2, 2007
Revised by  : Fredrik Svard
Description : Reviewed

Version     : 1
Date        : September 20, 2007
Revised by  : Fredrik Svard
Description : First draft

*/
#ifndef _CS_COMMANDS_INCLUDED
#define _CS_COMMANDS_INCLUDED
/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                     DEFINITIONS AND MACROS                             **/
/**                                                                        **/
/****************************************************************************/


/* HVPS */

#define MAX_HVPS_HIGHER       6000
#define MAX_HVPS_LOWER        3000
#define MAX_GEM_VOLTAGE        400
#define MAX_DRIFT_VOLTAGE      400
#define MAX_DEVIATION           30
#define MIN_HVPS              7.0

#define MAX_GEM_PERCENTAGE       2.0
#define MAX_DRIFT_PERCENTAGE     5.0


#define HIGHER_CHANNELS    19
#define LOWER_CHANNELS     8
// FIXME: change to  MAX_NO_CH??
#define MAX_NO_OF_CHANNELS (LOWER_CHANNELS + HIGHER_CHANNELS) 

/* Connection */
#define CMD_PORT                     4390 
#define CMD_ALARM_PORT               5390

/* Changes that lead to a stepping of protocol version (avoid Never GEM Manager talking to a an old CS or vice versa */

/*  Version
      3           New calibration flag, simple_gain_frame and  asic initial and intercolumn delay in body_set_get_configuration_t.


*/
      
#define PROTOCOL_VERSION             (int8u) 3

/* Markers used to avoid reading crap... */
#define CS_MARKER                    '!'
#define IRM_MARKER                   '#'


/* Commands issued from IRM */


// FIXME: but them in order ...
/* SETUP  **************************************************************/
/* ACQISITION***********************************************************/ 
/* STATUS  *************************************************************/
/* TEST  ***************************************************************/ 


#define CMD_GET_FRAME                               0x01
#define CMD_GET_OFFSET_CORRECTED_FRAME              0x02
#define CMD_ABORT                                   0x03
#define CMD_SET_SIMPLE_GAIN                         0x04  
#define CMD_SET_CONFIGURATION                       0x05
#define CMD_GET_STATUS                              0x06
#define CMD_GET_SIMPLE_GAIN                         0x07
#define CMD_HW_TEST                                 0x08
#define CMD_GET_CONFIGURATION                       0x09
#define CMD_GET_CALIBRATED_FRAME                    0x0a
#define CMD_SET_OFFSET_FRAME                        0x0b
#define CMD_GET_OFFSET_FRAME                        0x0c
#define CMD_SET_FLAT_FIELD                          0x0d
#define CMD_GET_FLAT_FIELD                          0x0e
//#define CMD_STOP_ANSWER_WATCHDOG                    0x0f
#define CMD_DIAG_SELF_TEST                          0x10
#define CMD_SET_PIXEL_MAP                           0x11
#define CMD_ECHO_SW                                 0x12
//#define CMD_SET_FRAME_RATE                          0x13
#define CMD_SET_STANDBY                             0x14
#define CMD_SET_FRAME_MODE                          0x15
#define CMD_GET_CURRENT_CONFIGURATION               0x16
#define CMD_SAVE_CURRENT_CONFIGURATION              0x17
// #define CMD_SET_SYSTEM_STATE                        0x18

//#define CMD_GET_OFFSET_GAIN_FRAME                   0x19
#define CMD_GET_OFFSET_GAIN_CORRECTED_FRAME           0x19
#define CMD_GET_PIXEL_MAP                           0x1a
#define CMD_SET_TEST_PATTERN                        0x1b
#define CMD_SAVE_NEW_CONFIGURATION                  0x1c
#define CMD_GET_VERSIONS                            0x1d
#define CMD_GET_OFFSET_SIMPLE_GAIN_CORRECTED_FRAME  0x1e
#define CMD_GET_TEST_FRAME                          0x1f
#define CMD_OFFSET_FRAME_COLLECT                    0x20
#define CMD_MAX                                     (int8u) CMD_OFFSET_FRAME_COLLECT
#define PEER_CLOSED                                 0x7f

#define CMD_MIN_SW                                  0x30
#define CMD_SW_TEST                                 0x30
//#define CMD_TERMINATE_SESSIONS                      0x31
#define CMD_MAX_SW                                  CMD_TERMINATE_SESSIONS


//#define CMD_PING_PEER                               0x99



/* CMD_HW_TEST sub commands */
#define SUB_CMD_HW_READ              0x01
#define SUB_CMD_HW_WRITE             0x02
#define SUB_CMD_FILL_PCI_MEMORY      0x03
#define SUB_CMD_GET_FRAME            0x04
#define SUB_CMD_READ_TEMP            0x05
#define SUB_CMD_DUMP_REGISTERS       0x06

#define SUB_CMD_GET_HVPS             0x07
#define SUB_CMD_SET_HVPS             0x08
#define SUB_CMD_SET_REGS             0x09

#define SUB_CMD_SET_THRESHOLD        0x0a
#define SUB_CMD_GET_THRESHOLD        0x0b


#define SUB_CMD_SET_DETECTOR_CONF    0x0c
#define SUB_CMD_GET_DETECTOR_CONF    0x0d
#define SUB_CMD_SET_DEBUG            0x0e
#define SUB_CMD_SET_UNSCRAMBLE       0x0f
#define SUB_CMD_KILL_HW_SERVER       0x10
#define SUB_CMD_KILL_FRAME_GRABBER   0x11
#define SUB_CMD_KILL_CMD_HANDLER     0x12
#define SUB_CMD_REMOVE_DRIVER        0x13
#define SUB_CMD_SIMULATE_BAD_PIXEL   0x14
#define SUB_CMD_READ_PLX_REG         0x15
#define SUB_CMD_WRITE_PLX_REG        0x16
#define SUB_CMD_READ_FPGA_REG        0x17
#define SUB_CMD_WRITE_FPGA_REG       0x18 
#define SUB_CMD_ORDER_HW             0x19
#define SUB_CMD_IMMEDIATE_CMD        0x1a
//#define SUB_CMD_FRAME_GRABBER_READ   0x1b
#define SUB_CMD_FRAME_GRABBER_TEST   0x1b
#define SUB_CMD_WRITE_SRAM           0x1c
#define SUB_CMD_RESET_SYSTEM_STATE   0x1d
#define SUB_CMD_INIT_HW              0x1e
#define SUB_CMD_TEST_MEAN_VAL        0x1f
/* CMD_SW_TEST Sub commands */

#define SUB_CMD_PRINT_HASH           0x01
#define SUB_CMD_PRINT_CNTRS          0x02
#define SUB_CMD_STOP_ANSWER_WATCHDOG 0x03
#define SUB_CMD_TERMINATE_SESSIONS   0x04
#define SUB_CMD_SET_UTC_TIME         0x05
#define SUB_CMD_SET_LOG_LEVEL        0x06
#define SUB_CMD_SNAPSHOT_LOGS        0x07
#define SUB_CMD_PRINT_CONNECT_LIST   0x08

/* Replies from CS to IRM */
#define REPLY_BIT                    0x80

#define CMD_GET_FRAME_REPLY                         (CMD_GET_FRAME | REPLY_BIT)
#define CMD_GET_OFFSET_CORRECTED_FRAME_REPLY        (CMD_GET_OFFSET_CORRECTED_FRAME | REPLY_BIT)
#define CMD_ABORT_REPLY                             (CMD_ABORT | REPLY_BIT)
#define CMD_INIT_SYSTEM_REPLY                       (CMD_INIT_SYSTEM | REPLY_BIT)
#define CMD_SET_CONFIGURATION_REPLY                 (CMD_SET_CONFIGURATION | REPLY_BIT)
#define CMD_GET_STATUS_REPLY                        (CMD_GET_STATUS | REPLY_BIT)
#define CMD_ECHO_CS_REPLY                           (CMD_ECHO_CS | REPLY_BIT)
#define CMD_HW_TEST_REPLY                           (CMD_HW_TEST | REPLY_BIT)
#define CMD_SW_TEST_REPLY                           (CMD_SW_TEST | REPLY_BIT)
#define CMD_GET_CONFIGURATION_REPLY                 (CMD_GET_CONFIGURATION | REPLY_BIT)
#define CMD_GET_CALIBRATED_FRAME_REPLY              (CMD_GET_CALIBRATED_FRAME | REPLY_BIT)
#define CMD_SET_OFFSET_FRAME_REPLY                  (CMD_SET_OFFSET_FRAME | REPLY_BIT)
#define CMD_GET_OFFSET_FRAME_REPLY                  (CMD_GET_OFFSET_FRAME | REPLY_BIT)
#define CMD_SET_FLAT_FIELD_REPLY                    (CMD_SET_FLAT_FIELD | REPLY_BIT)  
#define CMD_GET_FLAT_FIELD_REPLY                    (CMD_GET_FLAT_FIELD | REPLY_BIT)  
#define CMD_STOP_ANSWER_WATCHDOG_REPLY              (CMD_STOP_ANSWER_WATCHDOG | REPLY_BIT)                          
#define CMD_DIAG_SELF_TEST_REPLY                    (CMD_DIAG_SELF_TEST | REPLY_BIT)                           
#define CMD_GET_OFFSET_GAIN_CORRECTED_FRAME_REPLY   (CMD_GET_OFFSET_GAIN_CORRECTED_FRAME | REPLY_BIT)
#define CMD_SET_PIXEL_MAP_REPLY                     (CMD_SET_PIXEL_MAP | REPLY_BIT)                            
#define CMD_ECHO_SW_REPLY                           (CMD_ECHO_SW | REPLY_BIT)                                  
#define CMD_SET_FRAME_RATE_REPLY                    (CMD_SET_FRAME_RATE | REPLY_BIT)                           
#define CMD_TERMINATE_SESSIONS_REPLY                (CMD_TERMINATE_SESSIONS | REPLY_BIT)                       
#define CMD_SET_FRAME_MODE_REPLY                    (CMD_SET_FRAME_MODE | REPLY_BIT)
#define CMD_SAVE_CURRENT_CONFIGURATION_REPLY        (CMD_SAVE_CURRENT_CONFIGURATION | REPLY_BIT)
#define CMD_GET_VERSIONS_REPLY                      (CMD_GET_VERSIONS | REPLY_BIT)
#define CMD_GET_OFFSET_SIMPLE_GAIN_CORRECTED_FRAME_REPLY (CMD_GET_OFFSET_SIMPLE_GAIN_CORRECTED_FRAME | REPLY_BIT)
//#define CMD_PING_PEER_REPLY          (int8u)(CMD_PING_PEER | REPLY_BIT)

/* Frame size in short, 16 bit */
#define ROWS   1024
#define COLS   1024
#define MAX_FRAME_SIZE               (ROWS * COLS)

/* Option field */

#define OPTION_SINGLE_REPLY          0x00
#define OPTION_MULTI_REPLY           0x40
 
/* The body part, i.e. data, could be delivered with header or be pointed out in an array */

#define OPTION_DATA_WITH_HEADER_REPLY       0x80
#define OPTION_DATA_IN_ARRAY_REPLY          0X00
 


/* Return codes from CS */

#define CMD_SUCESSFULL               0x0

#define ERROR_NO_DETECTOR_CONTACT    0x1
#define ERROR_TIMOUT_ON_CS           0x2

#define ERROR_INVALID_COMMAND        0x3
#define ERROR_INVALID_SUB_COMMAND    0x4
#define ERROR_WRONG_PROTOCOL_VERSION 0x5

#define ERROR_HVPS_VALUE_TOO_HIGH    0x6
#define ERROR_INVALID_CONFIGURATION  0x7
#define ERROR_INVALID_REGISTER_ADDR  0x8
#define ERROR_READ_FROM_DRIVER       0x9
#define ERROR_INVALID_FRAME_MODE     0xa
#define ERROR_PARAMETERS_INCLUDED    0xb
#define ERROR_CONFIGURATION_FILE_OPEN_FAILED  0xc
#define ERROR_CONFIGURATION_FILE_PARSE_ERROR  0xd
#define ERROR_CONFIGURATION_NOT_FOUND  0xe
#define ERROR_CONFIGURATION_FILE_FAILED_OPEN  0xf
#define ERROR_NO_GAIN_FRAME_INCLUDED  0x10
#define ERROR_NO_FRAME_GRABBER_CONTACT  0x11
#define ERROR_FAILED_READING_CURRENT_CONFIG  0x12
#define ERROR_FAILED_SAVING_CURRENT_CONFIG  0x13
#define ERROR_WRONG_CONFIG_TYPE    0x14
#define ERROR_SETTING_NOT_FOUND    0x15
#define ERROR_NO_ROOT_NOT_FOUND    0x16
#define ERROR_FAILED_CREATE_CONFIG    0x17
#define ERROR_FAILED_CREATE_GROUP    0x18
#define ERROR_FAILED_PARENT_NOT_GROUP    0x19
#define ERROR_OUTSIDE_CONFIG_RANGE       0x1a
#define ERROR_FULL_LIST             0x1b
#define ERROR_FAILED_SAVING_CALIBRATION_FILE     0x1c
#define ERROR_FAILED_READING_CALIBRATION_FILE 0x1d

//#define ERROR_NO_OFFSET_FRAME        0x1d
#define ERROR_NO_GAIN_FRAME          0x1e
#define ERROR_FAILED_SAVING_NEW_CONFIG 0x1f
#define ERROR_FUNCTION_NOT_IMPLEMENTED 0x20
#define ERROR_FLAG_NOT_DEFINED         0x21
#define ERROR_NO_CALIBRATION_DATA      0x22
#define ERROR_NO_HVPS_CONTACT          0x23
#define ERROR_FAILED_CREATE_LIST       0x24
#define ERROR_SNMP_ERROR               0x25
#define ERROR_HVPS_TOO_HIGH_DEVIATION  0x26
#define ERROR_INVALID_HVPS_CHANNEL     0x27
#define ERROR_HVPS_OFF                 0x28
#define ERROR_HVPS_CHANNEL_OFF         0x29
#define ERROR_CORRUPT_CALIBRATION_FILE 0x2a
#define ERROR_NO_OFFSET_FRAME_INCLUDED 0x2b
#define ERROR_INVALID_LAYER            0x2c
#define ERROR_INVALID_SRC              0x2d
#define ERROR_NO_CONTACT_W_DEVICE_DRIVER 0x2e
#define ERROR_FAILED_CHANGING_SYSTEM_STATE 0x2f
#define ERROR_HVPS_TIMEOUT             0x30
#define ERROR_INPUT_OUTPUT_ERROR       0x31
#define ERROR_WRONG_CONFIG_FORMAT    0x32
#define ERROR_GROUP_NOT_FOUND        0x33
#define ERROR_TOO_BIG_DIFF           0x34
#define ERROR_WRONG_SYSTEM_STATE     0x35

#define ERROR_PEER_CLOSED            0x36

/* Frame modes */
#define SINGLE_FRAME_MODE            0x01
#define MULTI_FRAME_MODE             0x02
#define LIVE_FRAME_MODE              0x03

#define EXTERNAL_TRIGGER              0x01
#define INTERNAL_TRIGGER              0x00


/* Thresholds */

#define TEMP_ALARM                   0xff
//#define HVPS_NO_CONTACT              0x99
#define HVPS_TOO_BIG_DIFF            0xaa
#define HVPS_POWER_OFF               0xbb


#define TRUE      (boolean)(1)
#define FALSE      (boolean)(0)


/****************************************************************************/
/**                                                                        **/
/**                     TYPEDEFS AND STRUCTURES                            **/
/**                                                                        **/
/****************************************************************************/

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#ifdef CS_TYPES
/**** C type extensions ****/
typedef unsigned char   boolean;
typedef unsigned char   int8u;
typedef          char   int8s;
typedef unsigned short  int16u;
typedef          short  int16s;
typedef unsigned long   int32u;
typedef          long   int32s;
typedef          float  float32;
typedef          double float64;
#else
typedef unsigned char   boolean;
typedef unsigned char   int8u;
typedef          char   int8s;
typedef unsigned short  int16u;
typedef          short  int16s;
typedef unsigned int    int32u;
typedef          int    int32s;
typedef          float  float32;
typedef          double float64;
#endif

#define MAX_NO_OF_FRAMES  16

/****************************************************************************
 System States

The Control System Software have a number of system states defined to
be able to keep track of status of hardware and give external units
information about the system state, thus avoiding requests when the system
not ready for it. State transistions will be initiated both by request from
external units, eg SET_CONFIGURATION/SET_STANDBY or changes in hardware status.

INIT: When the CS application starts, system state is set to INIT.
      A default configuration is set. High Voltage supply and
      gas equipment not active. When CS has initiated internal
      structures, eg device drivers etc there's a transition to STANDBY

STANDBY: The CS software will susceptible to a limited number of commands,
      GET_STATUS, HW_TEST. SET_CONFIGURATION initiates a transition
      to WARMUP.

WARMUP: The SET_CONFIGURATION sets High Voltage Supply and launches
        the gas pressure equipment. Since these 'actions' takes time
        the software needs to supervise the hardware. No acqusition
        possible. CS software make transistion to OPERATIONAL when
        it get okey from hardware.

OPERATIONAL: The detector system is fully operational, ready for
        acqusition.All defined commands allowed. Possible to
        go back to STANDBY state (working day over) or change of
        in case of failure, see below.

REDUCED: Internal problem in software, eg. failed to read or
        write to configuration file or similar. Intermitent
        hardware problems. No contact with device driver.
        Acqusition not allowed but a limited number of commands
        allowed. Problem could be resolved by:
        * restarting application or device driver or both.
        * reboot the system

FAILED: Major problems. No contact with hardware, Command Link
        down. No contact with High voltage Supply. Need
        hardware trouble shooting to resolve problem.
        Problem could be resolved by:
        * restarting application or device driver or both.
        * reboot the system


*****************************************************************************/
typedef enum _SYSTEM_STATE {
  INIT        = 1,                 /* Initial state after boot */
  STANDBY     = 2,                 /* The system is configured */
  WARMUP      = 3, 
  CHILL       = 4, 
  OPERATIONAL = 5,                 /* The system is ... gas on gas off?*/
  FAILED      = 6,                 /* The system has failed under operation */
  REDUCED     = 7                  /* The system reduced functionality.
                                      Possible to read registers but no
                                      acqusition could take place */
}SYSTEM_STATE;


typedef enum _BACKGROUND_STATE {

 OFFSET_COLLECTING,
 NO_COLLECTING
} BACKGROUND_STATE;

/****************************************************************************
 Layout of command from IRM to CS
*****************************************************************************/


/* Specific layout for CMD_SET_OFFSET_FRAME */

typedef struct  {
 u8    local_copy;
} set_offset_frame_t;


/* Specific layout for CMD_OFFSET_FRAME_COLLECT */

typedef struct  {
 u8    enable;
} offset_frame_collect_t;


/* Specific layout for CMD_DIAG_SELF_TEST */

typedef struct  {
 u8    index;
// u32 quality;
 float quality;
} diag_self_test_t;

/* Specific layout for CMD_SET_OFFSET_FRAME, GAIN_FRAME. */

typedef struct {
 int8u layer;
 int8u src;
#if 0
  int8u frame_count;
  int16u current_config_no;
#endif
} set_offset_gain_frame_t;

#define SUBSYSTEM_DETECTOR        0x01
#define SUBSYSTEM_HVPS            0x02
#define SUBSYSTEM_ALL             0x03

/* Specific layout for CMD_SET_CONFIGURATION */
typedef struct  {
  int8u use_included;    /* Use included parameters for set configuration */
  int8u config_no;       /* use predefined configuration in CS */ 
  int8u subsystems;      /* subsystems involved in setting */
} set_configuration_t;

// FIXME introduce packed for all structs?
/* Specific layout for CMD_GET_CONFIGURATION */
typedef struct   {
  int16u config_no;/* use predefined configuration in CS */  
  int16u count;   /* number of configurations (valid in reply) */
} get_configuration_t;

typedef struct   {
  int16u config_no;/* Return new config no in reply*/  
} save_new_configuration_t;

/* Specific layout for CMD_GET_FRAME */
typedef struct   {
  u32 frame_count;    // obsolete??
//  u32 chk_sum;
  u8 hw_buff;
  u16 seq_no;
  u16 hw_seq_no;
  u32 secs;
  u32 usecs;
} get_frame_t;

/* Specific layout for CMD_SET_FRAME_MODE */
typedef struct   {
  int8u    mode;
  int16u   frame_count;
  int8u    external_trig;

} set_frame_mode_t;


/* Specific layout for CMD_SET_TEST_PATTERN */
typedef struct   {
  int8u  enable;
} set_test_pattern_t;


/* Specific layout for CMD_HW_TEST */
typedef struct  {
  u32    dw[4] ;
  int8u  sub_cmd;
} hw_test_t;


/* Specific layout for CMD_SW_TEST */
typedef struct   {
  u8   sub_cmd;
  u32  dw[4];
} sw_test_t;


typedef struct   {
  u32  dw[5];
} test_t;


/* General layout */
typedef struct Cmd {
  int8u    version; /* version handling of the 'protocol' */
  int8u    cmd;
  int8u    option;  /* misc usage, turn on/off logging ... */
  int8u    result;
  int32u   cmd_tag; /* Unique command identity, shows up in the reply */ 
//  int32u   length;  /* length of data (body) */ 
  u32   length;  /* length of data (body) */ 
  union {

    /* commands from IRM to CS */
    set_offset_gain_frame_t    set_offset_gain_frame;

    set_configuration_t        set_configuration;
    get_configuration_t        get_configuration;
    get_frame_t                get_frame ;
    set_frame_mode_t           set_frame_mode ;
    hw_test_t                  hw_test;
    sw_test_t                  sw_test;
    save_new_configuration_t   save_new_configuration;
    set_test_pattern_t         set_test_pattern; 
    diag_self_test_t           diag_self_test;
    test_t                     test;
    offset_frame_collect_t     offset_frame_collect;
    set_offset_frame_t         set_offset_frame;

  } specific;

} Cmd;



/* Definition for the body part of a message */

typedef struct  {

  u8 matrix;
  u8 coordinates;
  u8 binning;
  u8 pixel_pitch;
  u8 integration_time;
  u8 gain;
  u8 correction_status;
  u8 trigger_mode;
  u8 ring_counter;
  u32 frame_identifier;
  u8 panel_type;   // serial number
  u8 firmware_rev;
  u8 software_rev;
  u8 image_type;
} image_header_t;


/* CMD_GET_VERSIONS */

typedef struct  {

int32u cs_software_version;
u32 cs_device_driver_version;
u32 cs_fpga_version;
int32u dc_fpga_version;
int32u mb_fpga_version;
int32u eeprom_revision;
int32u silicon_revision;
char   kernel_release[40];
int32u  bios_version;
int32u detector_witdh;
int32u detector_heigth;
} get_versions_t;


/* CMD_GET_STATUS */
typedef struct  {

  int32u software_status;     /* This is a 32 bit vector, check below for possible 
                                 decection failures ... */   
  int8u cs_temperature;    
  int8u dc_temperature;        
  int8u mb_temperature;                                     
  int8u high_voltage;
  int8u gas_pressure;
  SYSTEM_STATE system_state;
  int8u defect_pixels;       /* percentage, distribution ? */

} system_status_t ;

/* Errors detected by software during setup or similar, not related to a specific command event */

/* Problems detected by hw_server, bit 7-0 */
#define FAILURE_ECHO_COMMUNICATION                  (1<<0)
#define FAILURE_CORRUPT_CONFIGURATION               (1<<1)
#define FAILURE_CONFIGURATION_FILE                  (1<<2)
#define HS_NO_DEVICE_DRIVER_CONTACT                 (1<<3)
//#define HS_HVPS_POWER_OFF22                           (1<<4)
#define FAILURE_NO_HVPS_CONTACT_1                   (1<<4)
#define FAILURE_NO_HVPS_CONTACT_2                   (1<<5)
#define FAILURE_NO_HVPS_CONTACT_3                   (1<<6)
#define HS_TIMEOUT_AT_CHILL                         (1<<5)
#define HS_TIMEOUT_AT_WARMUP                        (1<<7)
/* Problems detected by frame_grabber bit 15-8*/
#define FG_NO_DEVICE_DRIVER_CONTACT                 (1<<8)

/* Problems detected by cmd_handler bit 24-16*/
#define CM_FAILURE_READING_DMA_REGS                    (1<<16)

/* CMD_SET_CONFIGURATION, CMD_GET_CONFIGURATION, CMD_GET_CURRENT_CONFIGURATION*/

typedef struct  {

  char description[40];
  int8u  frame_mode;           /* 1 == single, 2 == multi, 3 == live */
  boolean ext_trigg;            /* 1 == yes, 0 == no */
  boolean delayed_ext_trigg;     /* 1 == yes, 0 == no */
  int8u  gain;                  /* 0b00 == 3,70mV/fC .... 0b11 == 36uV/fC */
  int16u frame_count;   
  int32u integration_time;   /* 24 bits */
  int32u reset_time;          /* 24 bits */
  int16u trigger_delay;
  u8     asic_initial_val;
  u16    intercolumn_delay;
  int16u hvps[MAX_NO_OF_CHANNELS];       /* Only valid for set_ and get_current_ */
  int8u  frame_rate;    /* no regs today for this ...*/
  int8u  binning;
  int8u  field_x0;
  int8u  field_y0;
  int16u field_width;
  int16u field_height;    /* no regs today for this ...*/
  int16u  detector_version;
  boolean valid_offset;      /* Status of calibration data for current configuration */
  boolean valid_gain;
  boolean valid_simple_gain;
  boolean valid_bad_pixel;
} body_set_get_configuration_t;


/* CMD_DIAG_SELF_TEST */

typedef struct  {

  int8u hwtest_result;
  int8u asic_loop_test_result;
  int8u universal_image_quality;
  int8u image_chksum_test_result;

} body_diag_self_test_t;


#define MAX_GAIN_FRAMES 10
#define DMA_SIZE  (1024 * 1024*2)

typedef unsigned short ch_frame[DMA_SIZE];

#if IMAGE_HDR
typedef unsigned short ch_frame[DMA_SIZE+sizeof(image_header)];
#endif

typedef unsigned short gain_frame_t[MAX_GAIN_FRAMES][MAX_FRAME_SIZE];



/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED VARIABLES                                 **/
/**                                                                        **/
/****************************************************************************/
#ifndef _CS_COMMANDS_C_SRC

#endif /*_CS_COMMANDS_C_SRC */
/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED FUNCTIONS                                 **/
/**                                                                        **/
/****************************************************************************/


#endif /* _CS_COMMANDS_INCLUDED */
/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/
