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

/* Existing variables */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;   
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;  

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u32 UserApp1_u32DataMsgCount = 0;
static u32 UserApp1_u32TickMsgCount = 0;
static AntAssignChannelInfoType sAntSetupData;
static bool bChannelIsMaster = TRUE;
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
  u8 au8WelcomeMessage[] = "Hide and Go Seek!";
  u8 au8Instructions[] = "Press B0 to Start";
  
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  /* Start with LED0 in RED state = channel is not configured */
  
  if (bChannelIsMaster)
  {
      /* Configure ANT for this application */
    sAntSetupData.AntChannel          = ANT_CHANNEL_MASTER;
    sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_MASTER;
    sAntSetupData.AntDeviceIdLo       = ANT_DEVICE1ID_LO_USERAPP;
    sAntSetupData.AntDeviceIdHi       = ANT_DEVICE1ID_HI_USERAPP;
  }
  else
  {
    /* Configure ANT for this application */
    sAntSetupData.AntChannel          = ANT_CHANNEL_SLAVE;
    sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_SLAVE;
    sAntSetupData.AntDeviceIdLo       = ANT_DEVICE0ID_LO_USERAPP;
    sAntSetupData.AntDeviceIdHi       = ANT_DEVICE0ID_HI_USERAPP;
  }
 
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
  
  /* If good initialization, set state to Idle */
  if (AntAssignChannel(&sAntSetupData))
  {
    UserApp1_StateMachine = UserApp1SM_AntChannelAssign;
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
static void UserApp1SM_AntChannelAssign(void)
{
  if( AntRadioStatusChannel(sAntSetupData.AntChannel) == ANT_CONFIGURED)
  {
    /* Channel assignment is successful, so open channel and
    proceed to Idle state */
    if (bChannelIsMaster)
    {
      UserApp1_StateMachine = UserApp1Initialize;
      bChannelIsMaster = FALSE;
    }
    else
    {
      UserApp1_StateMachine = UserApp1SM_Idle;
    }
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE))
  {
    UserApp1_StateMachine = UserApp1SM_Error;    
  }
     
} /* end UserApp1SM_AntChannelAssign */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    /*open the current channel*/
    AntOpenChannelNumber(sAntSetupData.AntChannel);
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1_WaitChannelOpen;
  }
} /* end UserApp1SM_Idle() */


/*Wait for Channel Open*/
static void UserApp1_WaitChannelOpen(void)
{
  static u8 au8SlaveMessageLine1[] = "Seeker";
  static u8 au8SlaveMessageLine2[] = "He is hiding : 10s";
  static u8 au8MasterMessage[] = "Hide!";
  
  if (AntRadioStatusChannel(sAntSetupData.AntChannel) == ANT_OPEN)
  {
    
    /*display "seeker" or "Hide!" on LCD*/
    LCDCommand(LCD_CLEAR_CMD);   
    if (bChannelIsMaster)
    {
      LCDMessage(LINE1_START_ADDR, au8MasterMessage);
    }
    else
    {
      LCDMessage(LINE1_START_ADDR, au8SlaveMessageLine1);
      LCDMessage(LINE2_START_ADDR, au8SlaveMessageLine2);
    }
    
    UserApp1_StateMachine = UserApp1_WaitForHiding;
  }
  
  /*check for timeout*/
  if (IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE))
  {
    AntCloseChannelNumber(sAntSetupData.AntChannel);
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
}/* end UserApp1_WaitChannelOpen() */


/*Wait for the first device hiding*/
static void UserApp1_WaitForHiding(void)
{
  static u16 u16TimeCounter1ms = 0;
  static u8 u8HidingTime = HIDING_TIME_COUNTER;
  static u8 au8HidingTimeString[2] = "10";
  
  /*count the time for waiting the other hiding*/  
  u16TimeCounter1ms++;
  if (ONE_SECOND_TIME == u16TimeCounter1ms)
  {
    u16TimeCounter1ms = 0;
    
    /*display and update  the hiding time on LCD*/
    u8HidingTime--;
    NumberToAscii(u8HidingTime, au8HidingTimeString);
    LCDClearChars(LINE2_START_ADDR+HIDING_TIME_DIS_PLACE, 2); 
    LCDMessage(LINE2_START_ADDR+HIDING_TIME_DIS_PLACE, au8HidingTimeString); 
    
    if (0 == u8HidingTime)
    {
      u8HidingTime = HIDING_TIME_COUNTER;
      LCDClearChars(LINE2_START_ADDR, 20);
      LCDMessage(LINE2_START_ADDR, "Ready or Not?(B2 OK)");
      UserApp1_StateMachine = UserApp1_ChannelOpen;      
    }
  }
}/* end UserApp1_WaitForHiding() */


/*channel has opened*/
static void UserApp1_ChannelOpen(void)
{  
  static u8 u8LastState = 0xff;
  static u8 au8TickMessage[] = "EVENT x\n\r";  /* "x" at index [6] will be replaced by the current code */
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static u8 au8FailMessage[] = "Lose Touch,Restart";
  static u8 au8LastAntData[ANT_APPLICATION_MESSAGE_BYTES] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  static u8 au8FindingMessage[] = "Here I Come!";
  static u8 au8AskForReadyMessage[] = {0x48,0x69,0x64,0x65,0x20,0x4F,0x6B,0x3F};
  
  
  bool bGotNewData;
  s8 s8RssiChannel0 = 0;
  
  /*if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    LCDClearChars(LINE2_START_ADDR, 20);
    LCDMessage(LINE2_START_ADDR, au8FindingMessage);
    PWMAudioSetFrequency(BUZZER1, 500);
    PWMAudioOn(BUZZER1);
  }*/
  
  /* A slave channel can close on its own, so explicitly check channel status */
  if (AntRadioStatusChannel(sAntSetupData.AntChannel) != ANT_OPEN)
  {
    LedBlink(GREEN, LED_2HZ);
    LedOff(BLUE);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    
    LCDClearChars(LINE2_START_ADDR, 20);
    LCDMessage(LINE2_START_ADDR, au8FailMessage);
    
    /*open the current channel*/
    AntOpenChannelNumber(sAntSetupData.AntChannel);
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  if (AntReadAppMessageBuffer())
  {
    if (G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      
      s8RssiChannel0 = G_sAntApiCurrentMessageExtData.s8RSSI;
      
      if (G_au8AntApiCurrentMessageBytes[7] == 0x12)
      {
        LCDClearChars(LINE2_START_ADDR, 20);
        LCDMessage(LINE2_START_ADDR, au8FindingMessage);
        PWMAudioSetFrequency(BUZZER1, 500);
        PWMAudioOn(BUZZER1);
        //UserApp1_StateMachine = UserApp1_WaitForHiding;
      }
      
      
      AntQueueBroadcastMessage(sAntSetupData.AntChannel, au8AskForReadyMessage);
    }
    else if (G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp1_u32TickMsgCount++;
      
      /* Look at the TICK contents to check the event code and respond only if it's different */
      if(u8LastState != G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        /* The state changed so update u8LastState and queue a debug message */
        u8LastState = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
        au8TickMessage[6] = HexToASCIICharUpper(u8LastState);
        DebugPrintf(au8TickMessage);
      } /* end if (u8LastState ...) */

    }
  }
}/* end UserApp1_ChannelCpen() */



/*Wait for Channel Close*/
static void UserApp1_WaitChannelClose(void)
{
  if (AntRadioStatusChannel(sAntSetupData.AntChannel) == ANT_CLOSED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /*check for timeout*/
  if (IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE))
  {
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}/* end UserApp1_WaitChannelClose() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
