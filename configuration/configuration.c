/*
*******************
******************************* C SOURCE FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : Gemini                                                      **
** filename :  configuration.c                                             **
** version   : 1                                                           **
** date      : September 18, 2007                                          **
**                                                                         **
*****************************************************************************
**                                                                         **
** Copyright (c) 2007, C-RAD Imaging                                       **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
*/
	
/*
	
CONTENTS
--------
	
1  Description
2  History of development
3  Include files
3.1 Blockwide definitions
3.2 Libraries
3.3 Modulewide definitions
3.4 Global signals
3.5 Local signals
4  Local definitions
4.1 Manifest constants
4.2 Type definitions
4.3 Macros
4.4 Signal composition
5  External references
5.1 Functions
5.2 Variables
5.3 Forward references
5.4 Processes
6  Global variables
7  Local variables
7.1 General variables
7.2 Receive specifications
8  Local functions
9  Global functions
10 Process entrypoints
	
*****************************************************************************
*/
	
/*
*****************************************************************************
** 1  DESCRIPTION.
*****************************************************************************
** This file is part of the Control System application. The modules 
** implements primitives to read a configuration file and to read
** predefined parameter settings for the hard and software
** 
	
TODO: - Save integers in hex format. Support functions created.
- Better handling on how to build strings??
- Introduce a program that parses configuration file before scs using
it?
*/
/*
*****************************************************************************
** 2  HISTORY OF DEVELOPMENT.
*****************************************************************************
** date      responsible           notes
** --------  --------------------  ------------------------------------------
** 07-10-30  Fredrik Svärd         Initial version
**
*/
	
/*
*****************************************************************************
** 3  INCLUDE FILES.
*****************************************************************************
*/
	
/*
**===========================================================================
** 3.1  Blockwide definitions.
**===========================================================================
*/

#include <stdbool.h>

#include <libconfig.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
	
	
#include "configuration.h"
#include "log.h"
#include "cs_commands.h"
	
/*
**===========================================================================
** 3.2  Libraries.
**===========================================================================
*/
	
/*
**===========================================================================
** 3.3  Modulewide definitions.
**===========================================================================
*/
	
/*
**===========================================================================
** 3.4  Global signals.
**	These signals are visible outside the current module.
**===========================================================================
*/
	
	
/*
**===========================================================================
** 3.5  Local signals.
**	These signals are used only within the current module.
**===========================================================================
*/
	
/*
*****************************************************************************
** 4  LOCAL DEFINITIONS.
*****************************************************************************
*/
	
/*
**===========================================================================
** 4.1  Manifest constants.
**===========================================================================
*/
	
#define PATH "application.config"
	
/*
**===========================================================================
** 4.2  Type definitions.
**===========================================================================
*/
	

config i_config;
	
	
/*
**===========================================================================
** 4.3  Macros.
**===========================================================================
*/
	
#define ERROR -1
#define OK     0
/*
**===========================================================================
** 4.4  Signal composition.
**===========================================================================
*/
	
/*
*****************************************************************************
** 5  EXTERNAL REFERENCES.
*****************************************************************************
*/
	
/*
**===========================================================================
** 5.1  Functions.
**===========================================================================
*/
	
/*
**===========================================================================
** 5.2  Variables.
**===========================================================================
*/
	
	
/*
**===========================================================================
** 5.3  Forward references.
**===========================================================================
*/
	
/*
**===========================================================================
** 5.4  Processes.
**===========================================================================
*/
	
/*
*****************************************************************************
** 6  GLOBAL VARIABLES.
*****************************************************************************
*/
	
/*
*****************************************************************************
** 7  LOCAL VARIABLES.
*****************************************************************************
*/
	
/*
**===========================================================================
** 7.1  General variables.
**===========================================================================
*/
/*
**===========================================================================
** 7.2 Receive specifications.
**===========================================================================
*/
	
/*
*****************************************************************************
** 8  LOCAL FUNCTIONS.
*****************************************************************************
*/
#define _CONFIGURATION_C_SRC
	
	
/*
**===========================================================================
** 8.0			add_element()
**===========================================================================
** Description: 
**
** Parameters:  
**
** Returns:     
**
** Globals:     
*/
	
int add_element(config_setting_t *group, char *path, char *name, unsigned int value) {
	
  config_setting_t *reg;
	
	
  /* Add register to group */
  if (NULL == (reg = config_setting_add(group, name, CONFIG_TYPE_INT))) {
    return ERROR_FAILED_PARENT_NOT_GROUP;
  }
  if(CONFIG_TRUE != config_setting_set_int(reg,value )) { 
    return ERROR_WRONG_CONFIG_TYPE;
  }
	
  /* Set to hex format */
  if ( CONFIG_TRUE != config_setting_set_format(reg, CONFIG_FORMAT_HEX)) {
    return ERROR_WRONG_CONFIG_FORMAT;   
	
  }
	
  return 0;
}
	
/*
**===========================================================================
** 8.0			update_element()
**===========================================================================
** Description: 
**
** Parameters:  
**
** Returns:     
**
** Globals:     
*/
	
int update_element(struct config_t *cfg, char *path, char *name, unsigned int value) {
	
  config_setting_t *reg;
  char lpath[100];
	
  /* Add register to path */
  strcpy(lpath,path);
  strcat(lpath,name);
	
  /* Find the register  */
  if(NULL == (reg = config_lookup(cfg, lpath))) {
		
    return ERROR_SETTING_NOT_FOUND;
  }
	
  if(CONFIG_TRUE != config_setting_set_int(reg, value)) {
		
    return ERROR_WRONG_CONFIG_TYPE;
  }
	
  /* Set to hex format */
  if ( CONFIG_TRUE != config_setting_set_format(reg, CONFIG_FORMAT_HEX)) {
    return ERROR_WRONG_CONFIG_FORMAT;   
  }
	
  return 0;
	
}
	
	
/*
**===========================================================================
** 8.0			get_default_configuration_number()
**===========================================================================
** Description: Get default configuration number
**
** Parameters:  
**
** Returns:     
**
** Globals:     
*/
int get_default_configuration_number(int *no) {
	
  *no = i_config.c_config.default_config_no;
	
  return 0;
}


/*
**===========================================================================
** 8.0			save_default_configuration_number()
**===========================================================================
** Description: 
**
** Parameters:  
**
** Returns:     
**
** Globals:     
*/

int save_default_configuration_number(char *file, int config_no) {

 int rc; 
 FILE *fp;
 struct config_t cfg;
 char *path = "application.software";
	
  LOG2FILE(LOG4C_PRIORITY_INFO,"Update configuration number...");

  /* Try to open configuration file for binary reading */
  if(!(fp=fopen(file, "rb"))) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to open: %s", file );
    return ERROR_CONFIGURATION_FILE_OPEN_FAILED;
  }
	
  /* Initiate config structure and parse the configuration file */
  config_init(&cfg);
	
  /* Show parsing errors */
  if(config_read(&cfg, fp) == CONFIG_FALSE) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"error on line:%d:%s",cfg.error_line, cfg.error_text);
    fclose(fp);
    return ERROR_CONFIGURATION_FILE_PARSE_ERROR;
  }

  if( NULL == config_lookup(&cfg, path)) {
    fclose(fp);
    return ERROR_SETTING_NOT_FOUND;
  }

  if( 0 != (rc = update_element(&cfg, path, ".default_config_no" , config_no))) {
    fclose(fp);
    return rc;
  }


  /* We're ready, write to file ...*/
  if(config_write_file(&cfg, file)== CONFIG_FALSE) {
    fclose(fp);
    return ERROR_CONFIGURATION_NOT_FOUND;
  }
	
  /* We are ready, close file ... */
  fclose(fp);
  return CMD_SUCESSFULL;

}


/*
**===========================================================================
** 8.0			read_configuration_file()
**===========================================================================
** Description: Read a configuration file with parameters for the CS.
**
** Parameters:  file - name of configuration file
**
** Returns:     none
**
** Globals:     
*/
int read_configuration_file(char *file)
{
  FILE *fp; int x;
  int i = 0;
	
  struct config_t cfg;
  char number[20] ="";
  char hvps[40] ="";
  char description[40] ="";
  char orig[40] = PATH;
  char string[40] = "";
	
  char regs[40]  ="";  char cmd[40]  =""; char setup[40]  =""; char frame_count[40]  =""; char integration_time[40]  =""; 
  char reset_time[40] =""; char trigger_delay[40] =""; char asic_initial_value[40] =""; char intercolumn_delay[40] ="";
	
	
  /* Try to open configuration file for binary reading */
  fp = fopen(file, "rb");
  if(!fp) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to open: %s", file );
    return (ERROR);
  }
	
  /* Initiate config structure and parse the configuration file */
  config_init(&cfg);
  x = config_read(&cfg, fp);
  fclose(fp);
	
  /* Show parsing errors */
  if(!x) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"error on line:%d:%s",cfg.error_line, cfg.error_text);
	
    return (ERROR);
  }
	
  /* Check first group */
  if(!(config_lookup(&cfg, "application"))) {
    return (ERROR);
  }
	
  /* Make sure that everthing is cleared first */
	
  memset(&i_config, 0x0, sizeof(i_config));
	
  /* Read software settings ...*/
  i_config.c_config.debug     = config_lookup_int(&cfg, "application.software.debug");
  i_config.c_config.profiling = config_lookup_int(&cfg, "application.software.profiling");
  i_config.c_config.detector_version = config_lookup_int(&cfg, "application.software.detector_version");
  i_config.c_config.max_no_of_configs = config_lookup_int(&cfg, "application.software.max_no_of_configs");
  i_config.c_config.default_config_no = config_lookup_int(&cfg, "application.software.default_config_no");
  LOG2FILE(LOG4C_PRIORITY_INFO,"Hey, there should be : %d no of configs to read, default config is:%d",
	   i_config.c_config.max_no_of_configs, i_config.c_config.default_config_no);
	
  if(config_lookup_string(&cfg, "application.software.snmp_agent")) {
    strcpy(i_config.c_config.snmp_agent, config_lookup_string(&cfg, "application.software.snmp_agent"));
  }
	
	
  /* Read threshold settings ... */
  i_config.c_config.temp_max = config_lookup_int(&cfg, "application.thresholds.temp_max");
  i_config.c_config.temp_min = config_lookup_int(&cfg, "application.thresholds.temp_min");
  i_config.c_config.temp_hysteres = config_lookup_int(&cfg, "application.thresholds.temp_hysteres");
	
	
  /* Iterate over number of configurations ... */
  //  while (i < MAX_NO_OF_CONFIGS) {
  while (i < i_config.c_config.max_no_of_configs) {
    /* Use start string and append a number for every configuration */
    strcpy(string,orig);
    sprintf(number,"%d",i);
    strcat(string,number);
	
    if( config_lookup(&cfg, string)) {
      LOG2FILE(LOG4C_PRIORITY_INFO,"Hey, we got configuration: %d", i);
	
	
      /* Read description string for configuration */
      strcpy(description, string);
      strcat(description,".description");
      strcpy(i_config.configurations[i].description, config_lookup_string(&cfg, description));
      /* Read parameters for the High Power Voltage Support */
      strcpy(hvps, string);
      strcat(hvps,".hvps");
      if(config_lookup(&cfg, hvps)) {
	
	
	{ int ch; const config_setting_t *list; char channels[40] ="";
	  strcpy(channels, hvps); 
	  strcat(channels,".channels");
	  list = config_lookup(&cfg, channels);

          memset(i_config.configurations[i].channel, 0x0, sizeof(i_config.configurations[i].channel));

	  if(list) {
	    for(ch =0; ch <  config_setting_length(list); ch++) {
	      i_config.configurations[i].channel[ch] = config_setting_get_int_elem(list, ch);
	    }
	  }
	}
	
      }
	
	
      /* Read parameters regs */
      strcpy(regs, string);
      strcat(regs,".regs");
      if(config_lookup(&cfg, regs)) {
	strcpy(cmd,regs);strcpy(setup,regs);
	strcpy(frame_count,regs);strcpy(integration_time,regs); strcpy(reset_time,regs);strcpy(trigger_delay,regs);
	strcpy(asic_initial_value,regs);strcpy(intercolumn_delay,regs);
	
	strcat(cmd,".cmd");
	i_config.configurations[i].cmd = config_lookup_int(&cfg, cmd);
	strcat(setup,".setup");
	i_config.configurations[i].setup = config_lookup_int(&cfg, setup);
	
	strcat(frame_count,".frame_count");
	i_config.configurations[i].frame_count = config_lookup_int(&cfg, frame_count);
	strcat(integration_time,".integration_time");
	i_config.configurations[i].integration_time = config_lookup_int(&cfg, integration_time);
	
	strcat(reset_time,".reset_time");
	i_config.configurations[i].reset_time = config_lookup_int(&cfg, reset_time);
	
	strcat(trigger_delay,".trigger_delay");
	i_config.configurations[i].trigger_delay = config_lookup_int(&cfg, trigger_delay);
	
	strcat(asic_initial_value,".asic_initial_value");
	i_config.configurations[i].asic_initial_value = config_lookup_int(&cfg, asic_initial_value);
	
	strcat(intercolumn_delay,".intercolumn_delay");
	i_config.configurations[i].intercolumn_delay = config_lookup_int(&cfg, intercolumn_delay);
	
      }
	
      /* Read misc */
      {
	char misc[40];char rate[40]; char binning[40];char x0[40];char y0[40]; char width[40]; char height[40];   
	
	memset(misc,0, sizeof(misc)); memset(rate,0, sizeof(rate));memset(binning,0, sizeof(binning));
	memset(x0,0, sizeof(x0));memset(y0,0, sizeof(y0));memset(width,0, sizeof(width)); memset(height,0, sizeof(height));
	
	strcpy(misc, string);
	strcat(misc,".misc");
	if(config_lookup(&cfg, misc)) {
	  strcpy(rate,misc); strcpy(binning,misc);strcpy(x0,misc);strcpy(y0,misc);strcpy(width,misc);strcpy(height,misc);
	
	  strcat(rate,".frame_rate");
	  i_config.configurations[i].frame_rate = config_lookup_int(&cfg, rate);
	
	  strcat(binning,".binning");
	  i_config.configurations[i].binning = config_lookup_int(&cfg, binning);
	
	  strcat(x0,".field_x0");
	  i_config.configurations[i].field_x0 = config_lookup_int(&cfg, x0);
	
	  strcat(y0,".field_y0");
	  i_config.configurations[i].field_y0 = config_lookup_int(&cfg, y0);
	
	  strcat(width,".field_width");
	  i_config.configurations[i].field_width = config_lookup_int(&cfg, width);
	
	  strcat(height,".field_height");
	  i_config.configurations[i].field_height = config_lookup_int(&cfg, height);
	
	}
	
      }
	
      /* Read calibration flags */
      {   char calib[50] = "";  char cal_flag[50] = ""; 
	
	strcpy(calib, string);
	strcat(calib, ".calibration");
	if(config_lookup(&cfg, calib)) {
	  strcpy(cal_flag,calib);
	  strcat(cal_flag,".valid_gain_frame");
	  i_config.configurations[i].valid_gain_frame = config_lookup_bool(&cfg, cal_flag); 
	  memset(cal_flag, 0, sizeof(cal_flag));
	  strcpy(cal_flag,calib);
	  strcat(cal_flag,".valid_offset_frame");   
	  i_config.configurations[i].valid_offset_frame = config_lookup_bool(&cfg, cal_flag); 
	  memset(cal_flag, 0, sizeof(cal_flag));
	  strcpy(cal_flag,calib);
	  strcat(cal_flag,".valid_bad_pixel_map");   
	  i_config.configurations[i].valid_bad_pixel_map = config_lookup_bool(&cfg, cal_flag); 
          memset(cal_flag, 0, sizeof(cal_flag));
	  strcpy(cal_flag,calib);
	  strcat(cal_flag,".valid_simple_gain");   
	  i_config.configurations[i].valid_simple_gain = config_lookup_bool(&cfg, cal_flag); 
		
	}
      }
    }
    memset(string,0, sizeof(string));
	
    memset(hvps,0, sizeof(hvps));
	
    memset(regs,0, sizeof(regs));
    memset(cmd,0, sizeof(cmd));
    memset(setup,0, sizeof(setup));
    memset(frame_count,0, sizeof(frame_count));
    memset(integration_time,0, sizeof(integration_time));
    memset(reset_time,0, sizeof(reset_time));
    memset(trigger_delay,0, sizeof(trigger_delay));
    memset(asic_initial_value,0, sizeof(asic_initial_value));
    memset(intercolumn_delay,0, sizeof(intercolumn_delay));
    i++;
	
  } /* while */
  return (OK);
}
	
	
/*
**===========================================================================
** 8.0			get_common_config()
**===========================================================================
** Description: Returns a common stuff for all configurations
**
** Parameters:  file - name of configuration file
**
** Returns:     ptr to a configuration
**
** Globals:     
*/
	
	
int get_common_config(common_configuration *c) {
	
  *c = i_config.c_config;
	
  return 0;
}
/*
**===========================================================================
** 8.0			get_configuration()
**===========================================================================
** Description: Returns a configuration loaded in an earlier stage
**
** Parameters:  file - name of configuration file
**
** Returns:     ptr to a configuration
**
** Globals:     
*/
configuration *get_configuration(int number) {
  //  if ( 0 <= number && number <= MAX_NO_OF_CONFIGS) {
  if ( 0 <= number && number <= i_config.c_config.max_no_of_configs) {
    return &i_config.configurations[number];
  }
  else
    return NULL;
}
	
	
/*
**===========================================================================
** 8.0			update_configuration()
**===========================================================================
** Description: 
**
** Parameters:  ptr to a configuration
**
** Returns:     0 - if success 
**              none zero - failure
**
** Globals:     
*/
int update_configuration(char *file, configuration *c, int config_no) {
	
  FILE *fp;
  int rc;
	
  char number[20] ="";
  //  char orig[50] = PATH;
  char string[50] = ""; 
  char description[50] = ""; 
  char hvps[50]; 
  char regs[50]  ="";
  char misc[50]  =""; 
  char channel[50]  ="";
  config_setting_t  *channels;
	
	
  struct config_t cfg;
	
  LOG2FILE(LOG4C_PRIORITY_INFO,"Update configuration, ....");
	
  /* Try to open configuration file for binary reading */
  if(!(fp=fopen(file, "rb"))) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to open: %s", file );
    return ERROR_CONFIGURATION_FILE_OPEN_FAILED;
  }
	
  /* Initiate config structure and parse the configuration file */
  config_init(&cfg);
	
  /* Show parsing errors */
  if(config_read(&cfg, fp) == CONFIG_FALSE) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"error on line:%d:%s",cfg.error_line, cfg.error_text);
    fclose(fp);
    return ERROR_CONFIGURATION_FILE_PARSE_ERROR;
  }
	
  /* Find the configuration that we want to update */
	
  /* Use start string and append the config_no */
  strcpy(string,PATH);
  sprintf(number,"%d",config_no);
  strcat(string,number);
	
  {
    strcpy(description, string);
    strcat(description,".description"); 
	
    strcpy(hvps, string);
    strcat(hvps,".hvps");
	
    strcpy(channel, hvps);
    strcat(channel,".channels");
	
    strcpy(regs, string);
    strcat(regs,".regs");
	
    strcpy(misc, string);
    strcat(misc,".misc");
	
  }
	
  LOG2FILE(LOG4C_PRIORITY_INFO,"Hey, you want to update configuration: %d",config_no);
	
  if( NULL == config_lookup(&cfg, string)) {
	
    return ERROR_CONFIGURATION_NOT_FOUND;
  }
	
  /* Okey, we got to do it know ..., store in the file ... */
	
	
  /* HVPS */
  if(NULL == config_lookup(&cfg, hvps)) {
    fclose(fp);
    return ERROR_SETTING_NOT_FOUND;
  }
	
  if(NULL == (channels = config_lookup(&cfg, channel))) {
    fclose(fp);
    return ERROR_SETTING_NOT_FOUND;
  }
	
  { 
    int i;
    //         for(i = 0; i< config_setting_length(channels); i++) {  
    for(i = 0; i< MAX_NO_OF_CHANNELS; i++) { 
      if((config_setting_set_int_elem(channels, i, c->channel[i]) == CONFIG_FALSE)) {
	fclose(fp);
	return ERROR_WRONG_CONFIG_TYPE;
      }
    }
  }
	
  /* Registers */
  if( NULL == config_lookup(&cfg, regs)) {
    fclose(fp);
    return ERROR_SETTING_NOT_FOUND;
  }
	
  /* cmd */
  if( 0 != (rc = update_element(&cfg, regs, ".cmd" , c->cmd))) {
    fclose(fp);
    return rc;
  }
	
  /* setup */
  if( 0 != (rc = update_element(&cfg, regs, ".setup" , c->setup))) {
    fclose(fp);
    return rc;
  }
	
  /* frame_count */
  if( 0 != (rc = update_element(&cfg, regs, ".frame_count" , c->frame_count))) {
    fclose(fp);
    return rc;
  }
	
	
  /* integration_time */
	
  if( 0 != (rc = update_element(&cfg, regs, ".integration_time" , c->integration_time))) {
    fclose(fp);
    return rc;
  }
	
  /* reset_time */
  if( 0 != (rc = update_element(&cfg, regs, ".reset_time" , c->reset_time))) {
    fclose(fp);
    return rc;
  }
	
	
  /* trigger_delay */
  if( 0 != (rc = update_element(&cfg, regs, ".trigger_delay" , c->trigger_delay))) {
    fclose(fp);
    return rc;
  }
	
  /* asic_initial_value */
  if( 0 != (rc = update_element(&cfg, regs, ".asic_initial_value" , c->asic_initial_value))) {
    fclose(fp);
    return rc;
  }
	
  /* intercolumn_delay */
  if( 0 != (rc = update_element(&cfg, regs, ".intercolumn_delay" , c->intercolumn_delay))) {
    fclose(fp);
    return rc;
  }
	
  /* Misc group */
	
  if(NULL == config_lookup(&cfg, misc)) {
    fclose(fp);
    return ERROR_GROUP_NOT_FOUND;
  }
	
  /* frame_rate */
  if( 0 != (rc = update_element(&cfg, misc, ".frame_rate" , c->frame_rate))) {
    fclose(fp);
    return rc;
  }
	
	
	
  if( 0 != (rc = update_element(&cfg, misc, ".binning" , c->binning))) {
    fclose(fp);
    return rc;
  }
	
	
	
  if( 0 != (rc = update_element(&cfg, misc, ".field_x0" , c->field_x0))) {
    fclose(fp);
    return rc;
  }
	
  if( 0 != (rc = update_element(&cfg, misc, ".field_y0" , c->field_y0))) {
    fclose(fp);
    return rc;
  }
	
	
  if( 0 != (rc = update_element(&cfg, misc, ".field_width" , c->field_width))) {
    fclose(fp);
    return rc;
  }
	
	
  if( 0 != (rc = update_element(&cfg, misc, ".field_height" , c->field_height))) {
    fclose(fp);
    return rc;
  }
	

  /* Calibration */
	
	
  /* We're ready, write to file ...*/
  if(config_write_file(&cfg, file)== CONFIG_FALSE) {
    fclose(fp);
    return ERROR_CONFIGURATION_NOT_FOUND;
  }
	
  /* We are ready, close file ... */
  fclose(fp);
	
  return CMD_SUCESSFULL;
}
	
/*
**===========================================================================
** 8.0			save_configuration()
**===========================================================================
** Description: Save a new configuration
**
** Parameters:  ptr to a configuration
**
** Returns:     0 - if success 
**              none zero - failure
**
** Globals:     
*/
	
int save_configuration(char *file, configuration *c, int config_no) {
	
  FILE *fp;
  int rc, max_config;
	
  char number[20] ="";
	
  char string[50] = "";  char description[50] = ""; 
  char regs[50]  =""; char misc[50]  =""; 
	
	
  char s_config_no[30];
  config_setting_t *registers;
  config_setting_t *root;
  config_setting_t *newconfig, *desc, *hvps,  *reg, *miscs, *calibration; // i.e a new group it libconfig terms
	
	
	
  struct config_t cfg;
	
  LOG2FILE(LOG4C_PRIORITY_INFO,"Save configuration, ....");
	
  /* Try to open configuration file for binary reading */
  if(!(fp=fopen(file, "rb"))) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to open: %s", file );
    return ERROR_CONFIGURATION_FILE_OPEN_FAILED;
  }
	
  /* Initiate config structure and parse the configuration file */
  config_init(&cfg);
	
  /* Show parsing errors */
  if(CONFIG_FALSE == config_read(&cfg, fp)) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"error on line:%d:%s",cfg.error_line, cfg.error_text);
    fclose(fp);
    return (ERROR_CONFIGURATION_FILE_PARSE_ERROR);
  }
	
  /* Find the configuration that we want to update */
	
  /* Use start string and append the config_no */
  strcpy(string,PATH);
  sprintf(number,"%d",config_no);
  strcat(string,number);
	
  {
    strcpy(description, string);
    strcat(description,".description"); 
	
    strcpy(regs, string);
    strcat(regs,".regs");
	
    strcpy(misc, string);
    strcat(misc,".misc");
	
  }
	
  LOG2FILE(LOG4C_PRIORITY_INFO,"Hey, we will create a brand new configuration: %d", config_no);
	
	
  if(config_no > MAX_NO_OF_CONFIGS) {
    fclose(fp);
    return ERROR_OUTSIDE_CONFIG_RANGE;
  }
	
	
  if(NULL == (root = config_lookup(&cfg, "application"))) { 
    fclose(fp);
    return ERROR_NO_ROOT_NOT_FOUND;
  }
	
  /* We have the new config_no. Create a new configuration ! */
  strcpy(s_config_no, "config");
  strcat(s_config_no,number);  
  if(NULL == (newconfig = config_setting_add(root, s_config_no, CONFIG_TYPE_GROUP))) {
    fclose(fp);
    return ERROR_FAILED_CREATE_CONFIG;
  }
	
  /* Description */
	
  if (NULL == (desc = config_setting_add(newconfig, "description", CONFIG_TYPE_STRING))) {
    fclose(fp);
    return ERROR_FAILED_PARENT_NOT_GROUP;
  }
  if( CONFIG_FALSE == (config_setting_set_string(desc,c->description))) {
    fclose(fp);  
    return ERROR_WRONG_CONFIG_TYPE;
  }
	
  /* HVPS */
	
  if(NULL == (hvps = config_setting_add(newconfig, "hvps", CONFIG_TYPE_GROUP))) {
    fclose(fp);
    return ERROR_FAILED_CREATE_GROUP;
  }
	
  if(NULL == (hvps = config_setting_add(hvps, "channels", CONFIG_TYPE_LIST))) {
    fclose(fp);
    return ERROR_FAILED_CREATE_LIST;
  }
	
  {
    int i;
	
    for(i=0; i<MAX_NO_OF_CHANNELS; i++) {
      if (!(reg = config_setting_add(hvps, NULL, CONFIG_TYPE_INT))) {
	fclose(fp);
	return ERROR_FAILED_PARENT_NOT_GROUP;
      }
	
      if(!(config_setting_set_int(reg,c->channel[i] ))) {
	fclose(fp);  
	return ERROR_WRONG_CONFIG_TYPE;
      }
    }
	
	
  }
	
	
  /* Registers */
  strcpy(regs, string);
  strcat(regs,".regs");  
  if(NULL == (registers = config_setting_add(newconfig, "regs", CONFIG_TYPE_GROUP))) {
    fclose(fp);
    return ERROR_FAILED_CREATE_GROUP;
  }
	
	
  /* cmd */
	
  if( 0 != (rc = add_element(registers, regs, "cmd" , c->cmd))) {
    fclose(fp);
    return rc;
  }
	
  /* setup */
	
  if( 0 != (rc = add_element(registers, regs, "setup" , c->setup))) {
    fclose(fp);
    return rc;
  }
	
  /* frame_count */
	
	
  if( 0 != (rc = add_element(registers, regs, "frame_count" , c->frame_count))) {
    fclose(fp);
    return rc;
  }
	
  /* integration time */
	
  if( 0 != (rc = add_element(registers, regs, "integration_time" , c->integration_time))) {
    fclose(fp);
    return rc;
  }
	
	
  /* reset time */
	
  if( 0 != (rc = add_element(registers, regs, "reset_time" , c->reset_time))) {
    fclose(fp);
    return rc;
  }
	
  /* trigger delay */
	
  if( 0 != (rc = add_element(registers, regs, "trigger_delay" , c->trigger_delay))) {
    fclose(fp);
    return rc;
  }
	
	
	
	
  if( 0 != (rc = add_element(registers, regs, "asic_initial_value" , c->asic_initial_value))) {
    fclose(fp);
    return rc;
  }
	
  if( 0 != (rc = add_element(registers, regs, "intercolumn_delay" , c->intercolumn_delay))) {
    fclose(fp);
    return rc;
  }
	
	
	
	
  /* Misc ... */
	
  if(NULL == (miscs = config_setting_add(newconfig, "misc", CONFIG_TYPE_GROUP))) {
    fclose(fp);
    return ERROR_FAILED_CREATE_GROUP;
  }
	
	
  if( 0 != (rc = add_element(miscs, misc, "frame_rate" , c->frame_rate))) {
    fclose(fp);
    return rc;
  }
	
  if( 0 != (rc = add_element(miscs, misc, "binning" , c->binning))) {
    fclose(fp);
    return rc;
  }
	
	
  if( 0 != (rc = add_element(miscs, misc, "field_x0" , c->field_x0))) {
    fclose(fp);
    return rc;
  }
	
	
  if( 0 != (rc = add_element(miscs, misc, "field_y0" , c->field_y0))) {
    fclose(fp);
    return rc;
  }
	
	
  if( 0 != (rc = add_element(miscs, misc, "field_width" , c->field_width))) {
    fclose(fp);
    return rc;
  }
	
	
  if( 0 != (rc = add_element(miscs, misc, "field_height" , c->field_height))) {
    fclose(fp);
    return rc;
  }
	
	
  /* Calibration */
  if(!(calibration = config_setting_add(newconfig, "calibration", CONFIG_TYPE_GROUP))) {
    fclose(fp);
    return ERROR_FAILED_CREATE_GROUP;
  }
	
  /* Add offset, gain and pixmap flags */
  if (NULL == (reg = config_setting_add(calibration, "valid_offset_frame", CONFIG_TYPE_BOOL))) {
    return ERROR_FAILED_PARENT_NOT_GROUP;
  }
	
  if(CONFIG_TRUE != config_setting_set_bool(reg, false )) { 
    return ERROR_WRONG_CONFIG_TYPE;
  }
	
  if (NULL == (reg = config_setting_add(calibration, "valid_gain_frame", CONFIG_TYPE_BOOL))) {
    return ERROR_FAILED_PARENT_NOT_GROUP;
  }
	
  if(CONFIG_TRUE != config_setting_set_bool(reg, false )) { 
    return ERROR_WRONG_CONFIG_TYPE;
  }

 if (NULL == (reg = config_setting_add(calibration, "valid_simple_gain", CONFIG_TYPE_BOOL))) {
    return ERROR_FAILED_PARENT_NOT_GROUP;
  }
	
  if(CONFIG_TRUE != config_setting_set_bool(reg, false )) { 
    return ERROR_WRONG_CONFIG_TYPE;
  }


	
  if (NULL == (reg = config_setting_add(calibration, "valid_bad_pixel_map", CONFIG_TYPE_BOOL))) {
    return ERROR_FAILED_PARENT_NOT_GROUP;
  }
	
  if(CONFIG_TRUE != config_setting_set_bool(reg, false )) { 
    return ERROR_WRONG_CONFIG_TYPE;
  }
	
  /* BUG_FIX */
  /* Finally, we ready increment max_config_no */
  max_config = config_no + 1;
	
  if(!(registers = config_lookup(&cfg, "application.software.max_no_of_configs"))) {
    fclose(fp);
    return ERROR_SETTING_NOT_FOUND;
  }
  if(!(config_setting_set_int(registers,max_config))) {
    fclose(fp);  
    return ERROR_WRONG_CONFIG_TYPE;
  }   
	
	
	
  /* We're ready, write to file ...*/
  if(config_write_file(&cfg, file)== CONFIG_FALSE) {
    fclose(fp);
    return ERROR_CONFIGURATION_NOT_FOUND;
  }
	
  /* We are ready, close file ... */
  fclose(fp);
	
  return CMD_SUCESSFULL;
}
	
/*
**===========================================================================
** 8.0			copy_configuration_file()
**===========================================================================
** Description: Copy a complete the configuration file and attach a timestamp
**              in the filename to be able to track history for the 
**              configurations.
**
** Parameters:  ptr to a configuration
**
** Returns:     0 - if success 
**              none zero - failure
**
** Globals:     
*/
int copy_configuration_file(char *current_file) {
	
  struct tm *local_time;
  time_t now;
	
  char str_time[40];
  char ts_file[60];
	
  FILE *current_fp, *timestamp_fp;
  struct config_t cfg;
	
  /* Try to open current configuration file for binary reading */
  current_fp = fopen(current_file, "rb");
  if(!current_fp) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to open: %s", current_file );
    return (ERROR);
  }
	
  /* Get current time */
  now = time(NULL);
  local_time = localtime(&now);
	
  sprintf(str_time,"_%d%02d%02d-%02d%02d%02d",1900 +local_time->tm_year, 1 + local_time->tm_mon,local_time->tm_mday, local_time->tm_hour,local_time->tm_min,local_time->tm_sec);
  strcpy(ts_file, current_file); 
  strcat(ts_file, str_time);
	
  /* Try to open timestamp configuration file for binary writing */
  timestamp_fp = fopen(ts_file, "wb");
  if(!timestamp_fp) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to open: %s", ts_file );
    return (ERROR);
  }
	
  /* Initiate config structure and parse the configuration file */
  config_init(&cfg);
  if(config_read(&cfg, current_fp) == CONFIG_FALSE) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to read: %s", current_file );
    fclose(current_fp);
    return ERROR_CONFIGURATION_NOT_FOUND;
  }
	
	
	
  if(config_write_file(&cfg, ts_file)== CONFIG_FALSE) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to write: %s", ts_file );
    fclose(timestamp_fp);
    return ERROR_CONFIGURATION_NOT_FOUND;
  }
	
  fclose(current_fp);
  fclose(timestamp_fp);
	
  return 0;
}
	
/*
**===========================================================================
** 8.0			set_calibration_flag()
**===========================================================================
** Description:  Validate calibration data, ie. gain, offset or bad_pixel
**
** Parameters:  
**
** Returns:     0 - if success 
**              none zero - failure
**
** Globals:     
*/
	
int set_calibration_flag(char *file,  int current_config_no, int flag, int c) {
	
  FILE *fp;
  config_setting_t *calibration;
	
  char number[20] ="";
  char orig[50] = PATH;
  char string[50] = ""; 
  char calib[50] = ""; 
  char cal_flag[100] = ""; 
	
  struct config_t cfg;
	
	

	
	
	
	
  /* Try to open configuration file for binary writing */
  if(!(fp=fopen(file, "rb"))) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"Unable to open: %s", file );
    return ERROR_CONFIGURATION_FILE_OPEN_FAILED;
  }
	
  /* Initiate config structure and parse the configuration file */
  config_init(&cfg);
	
  /* Show parsing errors */
  if(!(config_read(&cfg, fp))) {
    LOG2FILE(LOG4C_PRIORITY_ERROR,"error on line:%d:%s",cfg.error_line, cfg.error_text);
    fclose(fp);
    return (ERROR_CONFIGURATION_FILE_PARSE_ERROR);
  }
	
  /* Use start string and append the config_no */
  strcpy(string,orig);
  sprintf(number,"%d",current_config_no);
  strcat(string,number);
  strcpy(calib,string);
  strcat(calib,".calibration");
	
	
  /* Update the calibration info for the current configuration */
  if( config_lookup(&cfg, string)) {
	
    if(!(config_lookup(&cfg, calib))) {
      fclose(fp);
      return ERROR_SETTING_NOT_FOUND;
    }
	
	
    switch (flag) {
	
    case GAIN_FLAG:
      strcpy(cal_flag,calib);
      strcat(cal_flag,".valid_gain_frame");
      i_config.configurations[current_config_no].valid_gain_frame = (boolean) flag; 
      break;
    case OFFSET_FLAG:
      strcpy(cal_flag,calib);
      strcat(cal_flag,".valid_offset_frame");
      i_config.configurations[current_config_no].valid_offset_frame = (boolean) flag; 
      break;
    case BAD_PIXEL_FLAG:
      strcpy(cal_flag,calib);
      strcat(cal_flag,".valid_bad_pixel_map");
      i_config.configurations[current_config_no].valid_bad_pixel_map = (boolean) flag; 
      break;

    case SIMPLE_GAIN_FLAG:
      strcpy(cal_flag,calib);
      strcat(cal_flag,".valid_simple_gain");
      i_config.configurations[current_config_no].valid_simple_gain = (boolean) flag; 
      break;
	
    default:
      fclose(fp);
      return ERROR_FLAG_NOT_DEFINED;
      break;
    }
	
    if(!(calibration = config_lookup(&cfg, cal_flag))) {
      fclose(fp);
      return ERROR_SETTING_NOT_FOUND;
    }
    if((config_setting_set_bool(calibration, c) == CONFIG_FALSE)) {
      fclose(fp);
      return ERROR_WRONG_CONFIG_TYPE;
    }
	
  }
  else {
	
    fclose(fp);
    return ERROR_CONFIGURATION_NOT_FOUND;
	
  }
	
  /* We're ready, write to file ...*/
  if(config_write_file(&cfg, file)== CONFIG_FALSE) {
    fclose(fp);
    return ERROR_CONFIGURATION_NOT_FOUND;
  }
	
  /* We are ready, close file ... */
  fclose(fp);
	
	
  return CMD_SUCESSFULL;
	
	
}
	
	
/*
**===========================================================================
** 8.0			get_calibration_flag()
**===========================================================================
** Description:  Get value on flag, reading from RAM
**
** Parameters:  
**
** Returns:     0 - if success 
**              none zero - failure
**
** Globals:     
*/
	
int get_calibration_flag(int current_config_no, int flag, boolean *value) {
	
	

	
  if(i_config.c_config.max_no_of_configs <current_config_no) {
    return ERROR_CONFIGURATION_NOT_FOUND;
  } 
	
  switch (flag) {
	
  case GAIN_FLAG:
	
    *value = (boolean) (i_config.configurations[current_config_no].valid_gain_frame);
    break;
  case OFFSET_FLAG:
		
    *value = (boolean) i_config.configurations[current_config_no].valid_offset_frame; 
    break;
  case BAD_PIXEL_FLAG:
		
    *value = (boolean) i_config.configurations[current_config_no].valid_bad_pixel_map; 
    break;

  case SIMPLE_GAIN_FLAG:	
    *value = (boolean) (i_config.configurations[current_config_no].valid_simple_gain);
    break;
	
  default:
    return ERROR_FLAG_NOT_DEFINED;
    break;
  }
  return 0;
	
}
