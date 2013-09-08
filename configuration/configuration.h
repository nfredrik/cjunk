/*
	                     *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename  : configuration.h                                             **
** version   : 2                                                           **
** date      : September 18, 2007                                          **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2007,  C-RAD Imaging                                      **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************

*/
#ifndef _CONFIGURATION_INCLUDED
#define _CONFIGURATION_INCLUDED

#include "cs_commands.h"    // FIXME: possible to move?
 
/*
*****************************************************************************
** 1  DESCRIPTION.
*****************************************************************************
**
** 

*/

/*
*****************************************************************************
** 2  HISTORY OF DEVELOPMENT.
*****************************************************************************
** date       responsible              notes
** --------   --------------------     --------------------------------------
** 2007-10-30 Fredrik Svärd            Initial version
*/

/*
*****************************************************************************
** 3  DEFINITIONS.
*****************************************************************************
*/


/*
**===========================================================================
** 3.1  Manifest constants.
**===========================================================================
*/
#define DAA_PATH   ".
//#define PATH_CALIBRATION_FILES "./config"
#define PATH_CALIBRATION_FILES "./calibration/config"
//#define PATH_CALIBRATION_FILES DAA_PATH/config"
#define PATH_TEST_FILES        "./test/"
 
//#define CONFIGURATION_FILE     DAA_PATH/cs_settings.cfg"  

//#define CONFIGURATION_FILE     "./cs_settings.cfg" 
#define CONFIGURATION_FILE     "./configuration/cs_settings.cfg" 

#define VALIDATE                1
#define INVALIDATE              0


#define MAX_NO_OF_CONFIGS   (1<<16)-1
#define DEFAULT_CONFIGS    10



#define OFFSET_FLAG        1
#define GAIN_FLAG          2
#define BAD_PIXEL_FLAG     3
#define SIMPLE_GAIN_FLAG   4



/*
**===========================================================================
** 3.2  Type definitions.
**===========================================================================
*/






typedef struct {

  int debug;
  int profiling;
  int detector_version;
  int max_no_of_configs;
  int default_config_no;
  char snmp_agent[20];

  /* thresholds */
  int temp_max;
  int temp_min;
  int temp_hysteres;

} common_configuration;

typedef struct {

 /* Description */
  char description[40];
  /* High Voltage Power Support */

  int16u channel[MAX_NO_OF_CHANNELS];

  /* Registers */
  unsigned int cmd;
  unsigned int setup;
  unsigned int frame_count;
  unsigned int integration_time;
  unsigned int reset_time;
  unsigned int trigger_delay;
  unsigned int asic_initial_value;
  unsigned int intercolumn_delay;

  /* Misc */
  unsigned char  frame_rate;    /* no regs today for this ...*/
  unsigned char  binning;
  unsigned char  field_x0;
  unsigned char  field_y0;
  unsigned short field_width;
  unsigned short field_height;    /* no regs today for this ...*/

  /* Calibration data */
  boolean valid_gain_frame;       /* complex calibration flag */
  boolean valid_simple_gain;
  boolean valid_offset_frame;
  boolean valid_bad_pixel_map;

} configuration;


typedef struct {
  common_configuration c_config;
  configuration configurations[MAX_NO_OF_CONFIGS];
} config;





/*
**===========================================================================
** 3.3  Macros.
**===========================================================================
*/

/*
*****************************************************************************
** 4  EXTERNAL REFERENCES.
*****************************************************************************
*/

/*
**===========================================================================
** 4.1  Functions.
**===========================================================================
*/

int save_default_configuration_number(char *file, int config_no);
int read_configuration_file(char *file);
int get_common_config(common_configuration *c);
configuration *get_configuration(int number);
int update_configuration(char *file, configuration *c, int config_no);
int save_configuration(char *file, configuration *c, int config_no);
int copy_configuration_file(char *current_file);
int set_calibration_flag(char *file, int c, int flag, int current_config_no);
int get_calibration_flag(int current_config_no, int flag, boolean *value);
int get_default_configuration_number(int *no);

/*
**===========================================================================
** 4.2  Variables.
**===========================================================================
*/
#ifndef _CONFIGURATION_C_SRC

#endif /*_CONFIGURATION_C_SRC */

#endif /* _CONFIGURATION_INCLUDED */

