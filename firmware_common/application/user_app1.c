/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

/* From ant_api.c */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;   
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;  

extern u8 G_au8DebugScanfBuffer[];  /* From debug.c */
extern u8 G_u8DebugScanfCharCount;  /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  static u8 au8NetworkKey[] = {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};
  AntAssignChannelInfoType sAntSetupData;
  
  /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = au8NetworkKey[i];
  }

  /* If good initialization, set state to ChannelAssign */
  if( AntAssignChannel(&sAntSetupData) )
  {
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    UserApp1_StateMachine = UserApp1SM_Error;
  }


} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/* Wait for ANT channel assignment */
static void UserApp1SM_WaitChannelAssign(void)
{
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    /* Channel assignment is successful, so open channel and
    proceed to Idle state */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserAppSM_WaitChannelOpen;
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp1_u32Timeout, 3000))
  {
    UserApp1_StateMachine = UserApp1SM_Error;    
  }
     
} /* end UserApp1SM_WaitChannelAssign */


static void UserAppSM_WaitSetPassword(void)
{
  static u8 au8PasswordMessage[] = "Set password in Debug:";
  
  
}


/* Wait for ANT channel Open */
static void UserAppSM_WaitChannelOpen(void)
{ 
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  { 
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserAppSM_WaitChannelClose;
  }
} /* end UserAppSM_WaitChannelOpen() */


/* Wait for ANT channel Close */
static void UserAppSM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    //UserApp_StateMachine = UserAppSM_Idle;
    LedOn(GREEN);
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    UserApp1_StateMachine = UserApp1SM_Error;
  }
    
} /* end UserAppSM_WaitChannelClose() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  static u8 u8CurrentHeartRate       = 0;
  static u8 u8MaxHeartRate           = 0;
  static u8 u8MinHeartRate           = 255;
  static u8 u8MaxWarningHR           = 200;
  static u8 u8MinWarningHR           = 45;
  static u16 u16BlinkTimeCounter1ms  = 0;
  static u16 u16TimeCounter1ms       = 0;
  static u8 u8TimeCounter1s          = 0;
  static s8 s8YearCount              = 18;
  static s8 s8MonthCount             = 5;
  static s8 s8DayCount               = 9;
  static s8 s8MinuteCount            = 0;
  static s8 s8HourCount              = 0;
  static s8 s8WeekdayIndex           = 0;
  static u8 u8CurrentStatus          = MAIN_INTERFACE_STATUS;
  static s8 s8ModifyPartLocation     = 17;
  static u8 u8BatteryLevel           = 0;
  static u8 u8BatteryStatusIndex     = 0;
  
  static u8 au8MaxHeartRate[]        = "xxx";
  static u8 au8MinHeartRate[]        = "xxx";
  static u8 au8CurrentHeartRate[]    = "Current HR:     bpm ";
  static u8 au8ModifyTimeMessage[]   = "Set Current Time:   ";
  static u8 au8CurrentTime[]         = "2018/05/09 00:00 Mon";
  static u8* pau8CurrentWeekday[]    = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
  static u8 au8SetWarningHRMessage[] = "Set Warning HR:     ";
  static u8 au8WarningHR[]           = "Max: 200 || Min: 45 ";
  static u8 au8BatteryLEVEL[]        = "Battery Level:     %";
  static u8 au8BatteryStatus[]       = "Status:             "; 
  static u8* pau8BatteryEachStatus[] = {"New     ","Good    ","OK      ","Low     ","Critical","Invalid "};
  static u8 au8RequestMessage[]      = {0x46,0xFF,0xFF,0xFF,0xFF,0x80,0x07,0x01};
  static u8 au8DebugHRMessage[156];
  
  static bool bWarningBeepIsOn       = FALSE;
  static bool bTimeBlinkPartIsOff    = FALSE;
  static bool bModifyPartIsOn        = FALSE;
  static bool bIncreasePart          = FALSE;
  
  /* count the time and calculate */
  u16TimeCounter1ms++; 
  if (1000 == u16TimeCounter1ms)
  {
    u16TimeCounter1ms = 0;
    u8TimeCounter1s++;
    if (60 == u8TimeCounter1s)
    {
      u8TimeCounter1s = 0;
      s8MinuteCount++;
      if (60 == s8MinuteCount)
      {
        s8MinuteCount = 0;
        s8HourCount++;
        if (24 == s8HourCount)
        {
          s8HourCount = 0;
          s8WeekdayIndex++;
          s8DayCount++;
          if (7 == s8WeekdayIndex)
          {
            s8WeekdayIndex = 0;
          }
          if (32 == s8DayCount)
          {
            s8DayCount = 1;
            s8MonthCount++;
            if (13 == s8MonthCount)
            {
              s8MonthCount = 1;
              s8YearCount++;
            }
          }
        }
      }
    }   
  }
  
  /* update the current time*/
  au8CurrentTime[YEAR_LCD_LOCATION]      = HexToASCIICharUpper(s8YearCount/10);
  au8CurrentTime[YEAR_LCD_LOCATION+1]    = HexToASCIICharUpper(s8YearCount%10);
  au8CurrentTime[MONTH_LCD_LOCATION]     = HexToASCIICharUpper(s8MonthCount/10);
  au8CurrentTime[MONTH_LCD_LOCATION+1]   = HexToASCIICharUpper(s8MonthCount%10);
  au8CurrentTime[DAY_LCD_LOCATION]       = HexToASCIICharUpper(s8DayCount/10);
  au8CurrentTime[DAY_LCD_LOCATION+1]     = HexToASCIICharUpper(s8DayCount%10);
  au8CurrentTime[HOUR_LCD_LOCATION]      = HexToASCIICharUpper(s8HourCount/10);
  au8CurrentTime[HOUR_LCD_LOCATION+1]    = HexToASCIICharUpper(s8HourCount%10);
  au8CurrentTime[MINUTE_LCD_LOCATION]    = HexToASCIICharUpper(s8MinuteCount/10);
  au8CurrentTime[MINUTE_LCD_LOCATION+1]  = HexToASCIICharUpper(s8MinuteCount%10);
  au8CurrentTime[WEEKDAY_LCD_LOCATION]   = pau8CurrentWeekday[s8WeekdayIndex][0];
  au8CurrentTime[WEEKDAY_LCD_LOCATION+1] = pau8CurrentWeekday[s8WeekdayIndex][1];
  au8CurrentTime[WEEKDAY_LCD_LOCATION+2] = pau8CurrentWeekday[s8WeekdayIndex][2];
  
  /* Always check for ANT messages */
  if( AntReadAppMessageBuffer() )
  {
    /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {    
      /* receive the current HR data */
      u8CurrentHeartRate = G_au8AntApiCurrentMessageBytes[MAIN_PAGE_HR_LOCATION];
      
      /*choose the max HeartRate*/
      if (u8CurrentHeartRate > u8MaxHeartRate)
      {
        u8MaxHeartRate = u8CurrentHeartRate;
      }
      
      /*choose the min HeartRate*/
      if (u8CurrentHeartRate < u8MinHeartRate)
      {
        u8MinHeartRate = u8CurrentHeartRate;
      }
      
      /*warning beep when current HR touch the warning HR*/
      if (u8CurrentHeartRate <= u8MaxWarningHR 
          && u8CurrentHeartRate >= u8MinWarningHR)
      {
        //bWarningBeepIsOn = FALSE;
        //PWMAudioOff(BUZZER1); 
      }
      else
      {
        //bWarningBeepIsOn = TRUE;
        u8CurrentStatus = IN_DANGEROUS_STATUS;
        //PWMAudioOn(BUZZER1);        
      }    
      
      if (MAIN_INTERFACE_STATUS == u8CurrentStatus)
      {
        LedOn(WHITE);
        LedOn(LCD_RED);
        LedOn(LCD_GREEN);
        LedOn(LCD_BLUE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(RED);
        
        /* We got new Heart Rate data and choose location to show on LCD */  
        au8CurrentHeartRate[HR_LCD_LOCATION] = HexToASCIICharUpper(u8CurrentHeartRate/100);
        au8CurrentHeartRate[HR_LCD_LOCATION+1] = HexToASCIICharUpper(u8CurrentHeartRate/10%10);
        au8CurrentHeartRate[HR_LCD_LOCATION+2] = HexToASCIICharUpper(u8CurrentHeartRate%10);
        /*process when the first bit is '0'*/
        if ('0' == au8CurrentHeartRate[HR_LCD_LOCATION])
        {
          au8CurrentHeartRate[HR_LCD_LOCATION] = ' ';
        } 
        LCDMessage(LINE2_START_ADDR, au8CurrentHeartRate);
        //display the current time in main interface
        LCDMessage(LINE1_START_ADDR, au8CurrentTime);
        
        
        /*Press BUTTON0 to view the Max and Min Heart Rate*/
        if (WasButtonPressed(BUTTON0))
        {
          ButtonAcknowledge(BUTTON0);
          
          u8CurrentStatus = MAX_AND_MIN_STATUS;
          
          LedOn(PURPLE);
          
          /*Convert u8 data to string data to display Max and Min HR on LCD*/
          NumberToAscii(u8MaxHeartRate,au8MaxHeartRate);
          NumberToAscii(u8MinHeartRate,au8MinHeartRate);
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR, "Max HR:     bpm");
          LCDMessage(LINE1_START_ADDR+8,au8MaxHeartRate);
          LCDMessage(LINE2_START_ADDR, "Min HR:     bpm");
          LCDMessage(LINE2_START_ADDR+8,au8MinHeartRate);
        }
        
        /*Press BUTTON1 to enter the sleeping status*/
        if (WasButtonPressed(BUTTON1))
        {
          ButtonAcknowledge(BUTTON1);
          LCDCommand(LCD_CLEAR_CMD);
          
          LedOff(LCD_RED);
          LedOff(LCD_GREEN);
          LedOff(LCD_BLUE);
          LedOff(WHITE);
          
          u8CurrentStatus = SLEEP_STATUS;
        }
        
        /*Press BUTTON2 to view current battery*/
        if (WasButtonPressed(BUTTON2))
        {
          ButtonAcknowledge(BUTTON2);
          u8CurrentStatus = BATTERY_STATUS;
          LCDCommand(LCD_CLEAR_CMD);
          AntQueueAcknowledgedMessage(ANT_CHANNEL_USERAPP,au8RequestMessage);
        }
      }
      
      /* view the current battery status and level in this status */
      if (BATTERY_STATUS == u8CurrentStatus)
      {       
        if (G_au8AntApiCurrentMessageBytes[PAGE_NUMBER_LOCATION] == 0x07
            || G_au8AntApiCurrentMessageBytes[PAGE_NUMBER_LOCATION] == 0x87)
        {
          /* get the current battery level */
          u8BatteryLevel = G_au8AntApiCurrentMessageBytes[BATTERY_PAGE_LEVEL_LOCATION];
          au8BatteryLEVEL[BATTERY_LEVEL_LCD_LOCATION] = HexToASCIICharUpper(u8BatteryLevel/100);
          au8BatteryLEVEL[BATTERY_LEVEL_LCD_LOCATION+1] = HexToASCIICharUpper(u8BatteryLevel/10%10);
          au8BatteryLEVEL[BATTERY_LEVEL_LCD_LOCATION+2] = HexToASCIICharUpper(u8BatteryLevel%10);
          /* process when the first bit is '0'*/
          if ('0' == au8BatteryLEVEL[BATTERY_LEVEL_LCD_LOCATION])
          {
            au8BatteryLEVEL[BATTERY_LEVEL_LCD_LOCATION] = ' ';
          }
          
          /* get the current battery status */
          u8BatteryStatusIndex = G_au8AntApiCurrentMessageBytes[BATTERY_PAGE_STATUS_LOCATION] >> 4;
          for (u8 i=0; i<BATTERY_STATUS_LCD_LOCATION; i++)
          {
            au8BatteryStatus[i+BATTERY_STATUS_LCD_LOCATION] = pau8BatteryEachStatus[u8BatteryStatusIndex-1][i];
          } 
        } 
        /* show battery level and status on the LCD */
        LCDMessage(LINE1_START_ADDR, au8BatteryLEVEL);
        LCDMessage(LINE2_START_ADDR, au8BatteryStatus);      
      }
      
      /*draw the graphycal HR data*/ 
      DebugPrintNumber(u8CurrentHeartRate);
      for (u8 i=0; i<u8CurrentHeartRate-44; i++)
      {
        au8DebugHRMessage[i] = '-';
      }
      for (u8 i=u8CurrentHeartRate-44; i<156; i++)
      {
        au8DebugHRMessage[i] = '\0';
      }
      DebugPrintf(au8DebugHRMessage);
      DebugLineFeed();      
      
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
    
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {          
      
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
    
  }/* end AntReadAppMessageBuffer() */
  
  /* view the Max and Min HR in this status */ 
  if (MAX_AND_MIN_STATUS == u8CurrentStatus)
  {
    /*Press BUTTON0 to enter in the next status:MODIFY_TIME_STATUS*/
    if (WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      
      u8CurrentStatus = SET_WARNING_HR_STATUS;
      
      LedOff(PURPLE);
        
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8SetWarningHRMessage);
    }
  } /* end max/min HR status */
  
  /*Set Warning HR in this status*/ 
  if (SET_WARNING_HR_STATUS == u8CurrentStatus)
  {
    /* start blink the part which need modified */
    u16BlinkTimeCounter1ms++;
    if (u16BlinkTimeCounter1ms == 500)
    {
      u16BlinkTimeCounter1ms = 0;
      
      if (bTimeBlinkPartIsOff)
      {
        bTimeBlinkPartIsOff = FALSE;
        LedOff(BLUE);
        LCDClearChars(LINE2_START_ADDR+s8ModifyPartLocation, 3);
      }
      else
      {
        bTimeBlinkPartIsOff = TRUE;
        LedOn(BLUE);
        au8WarningHR[MAX_WARNING_HR_LCD_LOCATION]   = HexToASCIICharUpper(u8MaxWarningHR/100);
        au8WarningHR[MAX_WARNING_HR_LCD_LOCATION+1] = HexToASCIICharUpper(u8MaxWarningHR/10%10);
        au8WarningHR[MAX_WARNING_HR_LCD_LOCATION+2] = HexToASCIICharUpper(u8MaxWarningHR%10);
        au8WarningHR[MIN_WARNING_HR_LCD_LOCATION]   = HexToASCIICharUpper(u8MinWarningHR/10);
        au8WarningHR[MIN_WARNING_HR_LCD_LOCATION+1] = HexToASCIICharUpper(u8MinWarningHR%10);      
        LCDMessage(LINE2_START_ADDR, au8WarningHR);
      }              
    }
    
    /*Press BUTTON0 to choose which part to be modified*/
    if (WasButtonPressed(BUTTON0))
    { 
      ButtonAcknowledge(BUTTON0);
      s8ModifyPartLocation -= (MIN_WARNING_HR_LCD_LOCATION-MAX_WARNING_HR_LCD_LOCATION);
      if (s8ModifyPartLocation < 0)
      {
        s8ModifyPartLocation = WEEKDAY_LCD_LOCATION;
        u16BlinkTimeCounter1ms = 0;
        bTimeBlinkPartIsOff = FALSE;
        u8CurrentStatus = MODIFY_TIME_STATUS;
        
        LedOff(BLUE);
        
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, au8ModifyTimeMessage);
      }
    }
    
    bModifyPartIsOn = FALSE;
    /*Press BUTTON1 to increase current part*/
    if (WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      bModifyPartIsOn = TRUE;
      bIncreasePart   = TRUE;
    }
    
    /*Press BUTTON2 to decrease current part*/
    if (WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      bModifyPartIsOn = TRUE;
      bIncreasePart   = FALSE;
    }
    
    /*begin to modify Warning HR*/
    if (bModifyPartIsOn)
    {
      /* modify the min HR part */
      if (MIN_WARNING_HR_LCD_LOCATION == s8ModifyPartLocation)
      {
        /* increase the number*/
        if (bIncreasePart)
        {
          u8MinWarningHR++;
          if (61 == u8MinWarningHR)
          {
            u8MinWarningHR = 45;
          }
        }
        
        /* decrease the number*/
        else
        {
          u8MinWarningHR--;
          if (44 == u8MinWarningHR)
          {
            u8MinWarningHR = 60;
          }
        }
      }
      
      /* modify the max HR part */
      if (MAX_WARNING_HR_LCD_LOCATION == s8ModifyPartLocation)
      {
        /* increase the number*/
        if (bIncreasePart)
        {
          u8MaxWarningHR++;
          if (201 == u8MaxWarningHR)
          {
            u8MaxWarningHR = 100;
          }
        }
        /* decrease the number*/
        else
        {
          u8MaxWarningHR--;
          if (99 == u8MaxWarningHR)
          {
            u8MaxWarningHR = 200;
          }
        }
      }
    } /* end bModifyPartIsOn */   
  } /* end Set Warning HR status */
  
  
  /*Modify current time in this status*/ 
  if (MODIFY_TIME_STATUS == u8CurrentStatus)
  {
    /* interrupt the current time counter 
       and start the blink time counter when begin to modify time */
    u16TimeCounter1ms = 0;
    u8TimeCounter1s = 0;
    u16BlinkTimeCounter1ms++;
    if (u16BlinkTimeCounter1ms == 500)
    {
      u16BlinkTimeCounter1ms = 0;
      
      if (bTimeBlinkPartIsOff)
      {
        bTimeBlinkPartIsOff = FALSE;
        
        LedOff(CYAN);
        if (WEEKDAY_LCD_LOCATION == s8ModifyPartLocation)
        {
          LCDClearChars(LINE2_START_ADDR+s8ModifyPartLocation, 3);
        }
        else
        {
          LCDClearChars(LINE2_START_ADDR+s8ModifyPartLocation, 2);
        }
      }
      else
      {
        bTimeBlinkPartIsOff = TRUE;
        
        LedOn(CYAN);
        LCDMessage(LINE2_START_ADDR, au8CurrentTime);
      }
               
    }

    /*Press BUTTON0 to choose which part to be modified*/
    if (WasButtonPressed(BUTTON0))
    { 
      ButtonAcknowledge(BUTTON0);
      s8ModifyPartLocation -= 3;     //choose the part to blink and modify
      if (s8ModifyPartLocation < 0)
      {
        s8ModifyPartLocation = WEEKDAY_LCD_LOCATION;
      }
    }
    
    bModifyPartIsOn = FALSE;
    /*Press BUTTON1 to increase current part*/
    if (WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      bModifyPartIsOn = TRUE;
      bIncreasePart   = TRUE;
    }
    
    /*Press BUTTON2 to decrease current part*/
    if (WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      bModifyPartIsOn = TRUE;
      bIncreasePart   = FALSE;
    }
    
    /*begin to modify current time*/
    if (bModifyPartIsOn)
    {
      /* modify the weekday part */
      if (WEEKDAY_LCD_LOCATION == s8ModifyPartLocation)
      {
        /* increase the number*/
        if (bIncreasePart)
        {
          s8WeekdayIndex++;
          if (7 == s8WeekdayIndex)
          {
            s8WeekdayIndex = 0;
          }
        }
        /* decrease the number*/
        else
        {
          s8WeekdayIndex--;
          if (s8WeekdayIndex<0)
          {
            s8WeekdayIndex = 6;
          }
        }
      }
      
      /* modify the minute part */
      if (MINUTE_LCD_LOCATION == s8ModifyPartLocation)
      {
        if (bIncreasePart)
        {
          s8MinuteCount++;
          if (60 == s8MinuteCount)
          {
            s8MinuteCount = 0;
          }
        }
        else
        {
          s8MinuteCount--;
          if (s8MinuteCount<0)
          {
            s8MinuteCount = 59;
          }
        }
      }
      
      /* modify the hour part */
      if (HOUR_LCD_LOCATION == s8ModifyPartLocation)
      {
        if (bIncreasePart)
        {
          s8HourCount++;
          if (24 == s8HourCount)
          {
            s8HourCount = 0;
          }
        }
        else
        {
          s8HourCount--;
          if (s8HourCount<0)
          {
            s8HourCount = 23;
          }
        }
      }
      
      /* modify the day part */
      if (DAY_LCD_LOCATION == s8ModifyPartLocation)
      {
        if (bIncreasePart)
        {
          s8DayCount++;
          if (32 == s8DayCount)
          {
            s8DayCount = 1;
          }
        }
        else
        {
          s8DayCount--;
          if (0 == s8DayCount)
          {
            s8DayCount = 31;
          }
        }
      }
      
      /* modify the month part */
      if (MONTH_LCD_LOCATION == s8ModifyPartLocation)
      {
        if (bIncreasePart)
        {
          s8MonthCount++;
          if (13 == s8MonthCount)
          {
            s8MonthCount = 1;
          }
        }
        else
        {
          s8MonthCount--;
          if (0 == s8MonthCount)
          {
            s8MonthCount = 12;
          }
        }
      }
      
      /* modify the year part */
      if (YEAR_LCD_LOCATION == s8ModifyPartLocation)
      {
        if (bIncreasePart)
        {
          s8YearCount++;
          if (100 == s8YearCount)
          {
            s8YearCount = 0;
          }
        }
        else
        {
          s8YearCount--;
          if (s8YearCount<0)
          {
            s8YearCount = 99;
          }
        }
      }     
    } /* end bModifyPartIsOn */
    
  } /* end Modify time status */
  
  /*process dangerous HR in this status*/ 
  if (IN_DANGEROUS_STATUS == u8CurrentStatus)
  {
    u16BlinkTimeCounter1ms++;
    
    if (500 == u16BlinkTimeCounter1ms)
    {
      u16BlinkTimeCounter1ms = 0;
      
      LedOff(LCD_GREEN);
      LedOff(LCD_BLUE);
      
      if (bWarningBeepIsOn)
      {
        bWarningBeepIsOn = FALSE;
        LedOff(LCD_RED);
        LedOff(RED);
        LCDCommand(LCD_CLEAR_CMD);
        PWMAudioSetFrequency(BUZZER1, B6);
      }
      else
      {
        bWarningBeepIsOn = TRUE;       
        LedOn(LCD_RED);
        LedOn(RED);
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, "In dangerous!");
        PWMAudioSetFrequency(BUZZER1, E5);
      }
    }
  }
  
  /* enter the sleep status */
  if (SLEEP_STATUS == u8CurrentStatus)
  {
    if (WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      u8CurrentStatus = MAIN_INTERFACE_STATUS;
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
    }
  }
  
  /*Press BUTTON3 to ensure status and return to the main interface*/
  if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    
    u8CurrentStatus = MAIN_INTERFACE_STATUS;
    s8ModifyPartLocation = MIN_WARNING_HR_LCD_LOCATION;
   
    LCDCommand(LCD_CLEAR_CMD);
    
  }
  
} /* end UserApp1SM_Idle() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
