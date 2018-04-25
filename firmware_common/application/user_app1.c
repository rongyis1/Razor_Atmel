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
    //sAntSetupData.AntDeviceIdLo       = ANT_DEVICE1ID_LO_USERAPP;
    //sAntSetupData.AntDeviceIdHi       = ANT_DEVICE1ID_HI_USERAPP;
  }
  else
  {
    /* Configure ANT for this application */
    sAntSetupData.AntChannel          = ANT_CHANNEL_SLAVE;
    sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_SLAVE;
    //sAntSetupData.AntDeviceIdLo       = ANT_DEVICE0ID_LO_USERAPP;
    //sAntSetupData.AntDeviceIdHi       = ANT_DEVICE0ID_HI_USERAPP;
  }
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICE0ID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICE0ID_HI_USERAPP;
 
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
      bChannelIsMaster = FALSE;
      UserApp1_StateMachine = UserApp1Initialize;
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
  static u8 au8ChooseRoleLine1[] = "B1 Hider";
  static u8 au8ChooseRoleLine2[] = "B2 Seeker";
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    LedOff(GREEN);
    PWMAudioOff(BUZZER1);
    
    LCDCommand(LCD_CLEAR_CMD);  
    LCDMessage(LINE1_START_ADDR, au8ChooseRoleLine1);
    LCDMessage(LINE2_START_ADDR, au8ChooseRoleLine2);
    
    UserApp1_StateMachine = UserApp1_ChooseRole;
  }
} /* end UserApp1SM_Idle() */



/*choose initial role*/
static void UserApp1_ChooseRole(void)
{
  static bool bButtonHasPressed = FALSE;
  
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    bButtonHasPressed = TRUE;
    bChannelIsMaster = TRUE;    
    sAntSetupData.AntChannel = ANT_CHANNEL_MASTER;  
  }
  
  if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    bButtonHasPressed = TRUE;
    bChannelIsMaster = FALSE;   
    sAntSetupData.AntChannel = ANT_CHANNEL_SLAVE;  
  }
  
  if (bButtonHasPressed)
  {      
    bButtonHasPressed = FALSE;
    
    /*open the current channel*/
    AntOpenChannelNumber(sAntSetupData.AntChannel);
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1_WaitChannelOpen;
  }
}  /* end UserApp1_ChooseRole() */



/*Wait for Channel Open*/
static void UserApp1_WaitChannelOpen(void)
{
  static u8 au8SlaveMessageLine1[] = "Seeker";
  static u8 au8SlaveMessageLine2[] = "He is hiding : 10s";
  static u8 au8MasterMessageLine1[] = "Hide!";
  static u8 au8MasterMessageLine2[] = "I am Hiding";
  
  if (AntRadioStatusChannel(sAntSetupData.AntChannel) == ANT_OPEN)
  {
    
    /*display "seeker" or "Hide!" on LCD*/
    LCDCommand(LCD_CLEAR_CMD);   
    if (bChannelIsMaster)
    {
      LCDMessage(LINE1_START_ADDR, au8MasterMessageLine1);
      LCDMessage(LINE2_START_ADDR, au8MasterMessageLine2);
      UserApp1_StateMachine = UserApp1_ChannelMasterOpen;
    }
    else
    {
      LCDMessage(LINE1_START_ADDR, au8SlaveMessageLine1);
      LCDMessage(LINE2_START_ADDR, au8SlaveMessageLine2);
      UserApp1_StateMachine = UserApp1_WaitForHiding;
    }
   
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
      for(u16 i=0; i<10; i++);
      LCDClearChars(LINE2_START_ADDR, 20);
      
      LCDMessage(LINE2_START_ADDR, "Ready or Not?");
      
      PWMAudioSetFrequency(BUZZER1, C3);
      PWMAudioOn(BUZZER1);
      
      UserApp1_StateMachine = UserApp1_ChannelSlaveOpen;      
    }
  }
}/* end UserApp1_WaitForHiding() */


/*channel master has opened*/
static void UserApp1_ChannelMasterOpen(void)
{
  u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  static u8 au8HideOKMessage[] = {0x48, 0x69, 0x64, 0x65, 0x20, 0x4F, 0x6B, 0x3F};
  static bool bSearchingIsOn = FALSE;
  
  if (AntReadAppMessageBuffer())
  {
    if (G_eAntApiCurrentMessageClass == ANT_DATA)
    {  
      
      /*if receive a string "Hide Ok?"*/
      if (0 == strncmp((char const*)au8HideOKMessage,
                       (char const*)G_au8AntApiCurrentMessageBytes, 8))
      {
        /*display "Hide Ok?" on the LCD*/
        LCDClearChars(LINE2_START_ADDR, 20);
        LCDMessage(LINE2_START_ADDR, G_au8AntApiCurrentMessageBytes);
        LCDMessage(LINE2_START_ADDR+9, "(B1 OK)");
        
        /*Press Button1 to tell the other device "hide ok"*/
        if (WasButtonPressed(BUTTON1))
        {
          ButtonAcknowledge(BUTTON1);
          bSearchingIsOn = TRUE;
          au8TestMessage[7] = KEY_PARAMETER;
        }
      }  
      
      /*if receive a message to know the seeker is searching me*/
      if (G_au8AntApiCurrentMessageBytes[6] == KEY_PARAMETER
          && bSearchingIsOn)
      {    
        LCDClearChars(LINE2_START_ADDR, 20);
        LCDMessage(LINE2_START_ADDR, "He is searching me");
      }
      
      /*if receive a message to tell the seeker "found me"*/
      if (G_au8AntApiCurrentMessageBytes[5] == KEY_PARAMETER)
      {  
        bSearchingIsOn = FALSE;
        au8TestMessage[6] = KEY_PARAMETER;
        LCDClearChars(LINE2_START_ADDR, 20);
        LCDMessage(LINE2_START_ADDR, "Found me!(B2 Exit)");
        UserApp1_StateMachine = UserApp1SM_EndSearching;
      }
       
      /*send the ack message to the seeker*/
      AntQueueBroadcastMessage(sAntSetupData.AntChannel, au8TestMessage);
    }
    else if (G_eAntApiCurrentMessageClass == ANT_TICK)
    {

    }
  }
}


/*find and play music and flash*/
static void UserApp1SM_EndSearching(void)
{
  static u32 au32MusicBeep[] = {C6, D6, E6, C6, 
                                C6, D6, E6, C6, 
                                E6, F6, G6, G6, 
                                E6, F6, G6, G6,
                                G5, A5, G5, F5, E5, C5, 
                                G5, A5, G5, F5, E5, C5, 
                                D5, G5, C5, C5,
                                D5, G5, C5, C5, NO};
  static LedNumberType au8CurrentLed[] = {WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
  static u8 u8ToneIndex = 0;
  static u8 u8LedIndex = 0; 
  static u16 u16TimeCounter = 0;
  
  u16TimeCounter++;
  if (u16TimeCounter == EACH_TONE_TIME)
  {
    u16TimeCounter = 0;
    
    PWMAudioSetFrequency(BUZZER1, au32MusicBeep[u8ToneIndex]);
    PWMAudioOn(BUZZER1);
    u8ToneIndex++;
    if (au32MusicBeep[u8ToneIndex] == NO)
    {
      u8ToneIndex = 0;
    }
    
    if (u8LedIndex == 0)
    {
      LedOff(RED);
      LedOn(WHITE);
    }
    else
    {
      LedOff(au8CurrentLed[u8LedIndex-1]);
      LedOn(au8CurrentLed[u8LedIndex]);
    }
    
    u8LedIndex++;
    if (u8LedIndex == sizeof(au8CurrentLed))
    {   
      u8LedIndex = 0;
    }
    
  } 
  
  if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    
    PWMAudioOff(BUZZER1);
    
    if (u8LedIndex == 0)
    {
      LedOff(RED);
    }
    else
    {
      LedOff(au8CurrentLed[u8LedIndex-1]);
    }
    
    u8LedIndex = 0; 
    u8ToneIndex = 0;
    u16TimeCounter = 0;
    
    AntCloseChannelNumber(sAntSetupData.AntChannel);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1_WaitChannelClose;
  }
}/* end UserApp1SM_EndSearching() */

/*channel slave has opened*/
static void UserApp1_ChannelSlaveOpen(void)
{  
  static u8 au8FailMessage[] = "Lose Touch,Restart";
  static u8 au8FindingMessage[] = "Here I Come!";
  static u8 au8AskForReadyMessage[] = {0x48,0x69,0x64,0x65,0x20,0x4F,0x6B,0x3F};
  static LedNumberType au8CurrentLed[] = {RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE};
  static u32 au32BeepingTone[] = {C3,D4,E4,G4,G5,G6,A6,B6};
  static u8 u8DistanceIndex = 0;
  static bool bBeepIsOn = TRUE;  
  static u8 u8Status = WAIT_HIDING;
  static u8 au8RssiString[3] = "40";
  s8 s8RssiChannel = 0;
  u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  
  /* A slave channel can close on its own, so explicitly check channel status */
  if (AntRadioStatusChannel(sAntSetupData.AntChannel) != ANT_OPEN)
  {
    LedBlink(GREEN, LED_2HZ);
    
    LCDClearChars(LINE2_START_ADDR, 20);
    LCDMessage(LINE2_START_ADDR, au8FailMessage);
      
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  if (AntReadAppMessageBuffer())
  {
    if (G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /*calculate the RSSI and distance between two nodes*/
      s8RssiChannel = G_sAntApiCurrentMessageExtData.s8RSSI;
      u8DistanceIndex = (MAXIMUM_RSSI - s8RssiChannel)/TOTAL_LEDS_COUNT;
      
      
      /*wait the other node for hiding*/
      if (u8Status == WAIT_HIDING)
      {
        AntQueueBroadcastMessage(sAntSetupData.AntChannel, au8AskForReadyMessage);
        
        if (G_au8AntApiCurrentMessageBytes[7] == KEY_PARAMETER)
        {
          LCDClearChars(LINE2_START_ADDR, 20);
 
          LCDMessage(LINE2_START_ADDR, au8FindingMessage);
          LCDMessage(LINE1_START_ADDR+11, "S:-   dbm");
            
          u8Status = IS_SEARCHING;
        }
      } /* end WAIT_HIDING */
      
      /* the status is in searching */
      if (u8Status == IS_SEARCHING)
      {
        LCDClearChars(LINE1_START_ADDR+14, 6);
        NumberToAscii(abs(s8RssiChannel), au8RssiString);
        LCDMessage(LINE1_START_ADDR+14, au8RssiString);
        LCDMessage(LINE1_START_ADDR+17, "dbm");
        
        au8TestMessage[6] = KEY_PARAMETER;
        AntQueueBroadcastMessage(sAntSetupData.AntChannel, au8TestMessage);
        
        
        for (u8 i=0; i<TOTAL_LEDS_COUNT; i++)
        {
          LedOff(au8CurrentLed[i]);
        } /*end close all leds first*/
        
        /*the closer get, the more Leds will be on*/
        for (u8 i=0; i<(TOTAL_LEDS_COUNT-u8DistanceIndex); i++)
        {
          LedOn(au8CurrentLed[i]);
        }
        
        /*the closer get, the faster a beeping tone set*/
        if (bBeepIsOn)
        {
          PWMAudioSetFrequency(BUZZER1, au32BeepingTone[TOTAL_LEDS_COUNT-u8DistanceIndex]);
          PWMAudioOn(BUZZER1);
        }
        
        /*disable the beep when B3 pressed*/
        if (WasButtonPressed(BUTTON3))
        {
          ButtonAcknowledge(BUTTON3);
          
          PWMAudioOff(BUZZER1);
          bBeepIsOn = FALSE;
        }
        
        /*when the two device is almost touched, 
          LCD says "I found you" and beep close*/ 
        if (s8RssiChannel >= MAXIMUM_RSSI)
        {
          LCDClearChars(LINE2_START_ADDR, 20);
          LCDMessage(LINE2_START_ADDR, "I found you");
          
          PWMAudioOff(BUZZER1);
          bBeepIsOn = TRUE;
          u8Status = FIND_HIM;
        }
        
      } /* end IS_SEARCHING */
      
      /* the status is find him */
      if (u8Status == FIND_HIM)
      {
        au8TestMessage[5] = KEY_PARAMETER;
        AntQueueBroadcastMessage(sAntSetupData.AntChannel, au8TestMessage);
        
        if (G_au8AntApiCurrentMessageBytes[6] == KEY_PARAMETER)
        {                
          AntCloseChannelNumber(sAntSetupData.AntChannel);
          u8Status = WAIT_HIDING;
          
          for (u8 i=0; i<TOTAL_LEDS_COUNT; i++)
          {
            LedOff(au8CurrentLed[i]);
          } /*end close all leds first*/
          
          UserApp1_u32Timeout = G_u32SystemTime1ms;
          UserApp1_StateMachine = UserApp1_WaitChannelClose;
        }
      }/* end FIND_HIM */
      
    }
    else if (G_eAntApiCurrentMessageClass == ANT_TICK)
    {

    }
  }
  
  
}/* end UserApp1_ChannelCpen() */



/*Wait for Channel Close*/
static void UserApp1_WaitChannelClose(void)
{
  if (AntRadioStatusChannel(sAntSetupData.AntChannel) == ANT_CLOSED)
  {
    UserApp1_StateMachine = UserApp1_SwitchRole;
  }
  
  /*check for timeout*/
  if (IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE))
  {
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}/* end UserApp1_WaitChannelClose() */
 

/*switch the role when close the channel*/
static void UserApp1_SwitchRole(void)
{
  if (bChannelIsMaster)
  {
    bChannelIsMaster = FALSE;
    sAntSetupData.AntChannel = ANT_CHANNEL_SLAVE;
  }
  else
  {
    bChannelIsMaster = TRUE;
    sAntSetupData.AntChannel = ANT_CHANNEL_MASTER;
  }
  
  //AntDeQueueApplicationMessage();
  AntOpenChannelNumber(sAntSetupData.AntChannel);
  
  UserApp1_u32Timeout = G_u32SystemTime1ms;
  UserApp1_StateMachine = UserApp1_WaitChannelOpen;
}/* end UserApp1_SwitchRole() */

    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
