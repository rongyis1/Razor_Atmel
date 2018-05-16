/**********************************************************************************************************************
File: user_app1.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for user_app1.c

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Required constants for ANT channel configuration */
#define ANT_CHANNEL_USERAPP             ANT_CHANNEL_0         
#define ANT_CHANNEL_TYPE_USERAPP        CHANNEL_TYPE_SLAVE    
#define ANT_DEVICEID_LO_USERAPP         (u8)0xD8             
#define ANT_DEVICEID_HI_USERAPP         (u8)0x1E        
#define ANT_DEVICE_TYPE_USERAPP         (u8)120        
#define ANT_TRANSMISSION_TYPE_USERAPP   (u8)1             
#define ANT_CHANNEL_PERIOD_LO_USERAPP   (u8)0x86   
#define ANT_CHANNEL_PERIOD_HI_USERAPP   (u8)0x1F 
#define ANT_FREQUENCY_USERAPP           (u8)57          
#define ANT_TX_POWER_USERAPP            RADIO_TX_POWER_4DBM
/* Required status constant in User*/
#define MAIN_INTERFACE_STATUS           (u8)0
#define MAX_AND_MIN_STATUS              (u8)1
#define SET_WARNING_HR_STATUS           (u8)2
#define MODIFY_TIME_STATUS              (u8)3
#define IN_DANGEROUS_STATUS             (u8)4
#define SLEEP_STATUS                    (u8)5
#define BATTERY_STATUS                  (u8)6
#define CHOOSE_MODE_STATUS              (u8)7
/* Required LCD_location constant in User*/
#define HR_LCD_LOCATION                 (u8)12
#define YEAR_LCD_LOCATION               (u8)2
#define MONTH_LCD_LOCATION              (u8)5
#define DAY_LCD_LOCATION                (u8)8
#define HOUR_LCD_LOCATION               (u8)11
#define MINUTE_LCD_LOCATION             (u8)14
#define WEEKDAY_LCD_LOCATION            (u8)17
#define MAX_MIN_HR_LCD_LOCATION         (u8)8
#define MAX_WARNING_HR_LCD_LOCATION     (u8)5
#define MIN_WARNING_HR_LCD_LOCATION     (u8)17
#define BATTERY_LEVEL_LCD_LOCATION      (u8)15
#define BATTERY_STATUS_LCD_LOCATION     (u8)8
#define MAIN_PAGE_HR_LOCATION           (u8)7
#define BATTERY_PAGE_LEVEL_LOCATION     (u8)1
#define BATTERY_PAGE_STATUS_LOCATION    (u8)3
#define PAGE_NUMBER_LOCATION            (u8)0
#define RUNNING_MODE_LCD_LOCATION       (u8)17
#define CYCLING_MODE_LCD_LOCATION       (u8)9
#define SWIMMING_MODE_LCD_LOCATION      (u8)1



/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_WaitChannelAssign(void);

static void UserAppSM_WaitChannelOpen(void);

static void UserAppSM_WaitChannelClose(void);

static void UserApp1SM_Idle(void); 

static void UserApp1SM_Error(void);         


#endif /* __USER_APP1_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
