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
extern u8 G_au8DebugScanfBuffer[];

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
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

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  /*declare and initialize varibles*/
  static u8 u8InitializeLine1[] = "B0:Name  B1:Debug";
  static u8 u8InitializeLine2[] = "B3:Main Interface";
  static u8 u8Line1Message[40]  = "     Rong Yi        ";
  static u8 u8Line2Message[40]  = "                    ";
  static u16 u16MusicNote[7]      = {C6,D6,E6,F6,G6,A6,B6};
  static u8 u8InputLine1Message[21];
  static u8 u8InputLine2Message[31];
  static u8 u8EachChar[1];
  static u8 u8Status         = 0;
  static u8 u8PositionIndex  = 0;
  static u16 u16TimeCounter  = 0;
  static s8 s8MusicNoteIndex = 0;
  static u8 u8MusicTime      = 0;  
  static u8 u8CharCount      = 0;
  static bool bInitialize    = TRUE;
  static bool bShiftIsOn     = FALSE;
  static bool bShiftLeftIsOn = FALSE;
  static bool bMusicLeftIsOn = FALSE;
  static bool bAlarmIsOn     = FALSE;
  static bool bIsInLine1     = TRUE;
  
  
  /*main interface*/
  if (u8Status == 0)
  {
    /*main interface diaplay*/
    if (bInitialize)
    {
      bInitialize  = FALSE;
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,u8InitializeLine1);
      LCDMessage(LINE2_START_ADDR,u8InitializeLine2);
    }
    
    /*Press BUTTON0 to display dynamic name*/  
    if ( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      u8Status = 1;
      
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,"B0:Shift Left");
      LCDMessage(LINE2_START_ADDR,"B1:Shift Right");
    }
    
    /*Press BUTTON1 to display input message in debug*/  
    if ( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      
      u8PositionIndex = 0;
      u8Status = 2;
      u16TimeCounter = 0;
      
      DebugScanf(G_au8DebugScanfBuffer);
      DebugSetPassthrough();
      LCDCommand(LCD_CLEAR_CMD);     
    }
  }
  
  /*begin to display dynamic name*/
  if (u8Status == 1)
  {
    /*Press BUTTON0 to shift name left*/  
    if ( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      
      bShiftIsOn = TRUE;
      bShiftLeftIsOn = TRUE;
      bMusicLeftIsOn = TRUE;
      
      s8MusicNoteIndex = 7;
      u8MusicTime = 0;
      u16TimeCounter = NAME_SHIFT_TIME-1; 
      
      PWMAudioSetFrequency(BUZZER1,0);
      PWMAudioOn(BUZZER1);    //turn on the Music
    }
    
    /*Press BUTTON1 to shift name right*/  
    if ( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      
      bShiftIsOn = TRUE;
      bShiftLeftIsOn = FALSE;
      bMusicLeftIsOn = FALSE;
      
      s8MusicNoteIndex = 0;
      u8MusicTime = 0;
      u16TimeCounter = NAME_SHIFT_TIME-1; 
      
      PWMAudioSetFrequency(BUZZER1,0);
      PWMAudioOn(BUZZER1);    //turn on the Music
    }
    
    /*begin to shift name*/
    if (bShiftIsOn)
    {
      u16TimeCounter++;
      
      /*shift name per 800ms*/
      if (u16TimeCounter == NAME_SHIFT_TIME)
      {
        u16TimeCounter = 0;
        
        /*shift name left*/
        if (bShiftLeftIsOn)
        {
          /*shift each char left*/
          for (u8PositionIndex=0; u8PositionIndex<20; u8PositionIndex++)
          {
            u8Line2Message[u8PositionIndex] = u8Line2Message[u8PositionIndex+1];
            u8Line1Message[u8PositionIndex] = u8Line1Message[u8PositionIndex+1];
          }
          u8Line1Message[20] = u8Line2Message[0];    //the solution to go to next line
          u8Line2Message[19] = ' ';                   //the soulution when touch the Line2 end
        }
        
        /*shift name right*/
        else
        {
          /*shift each char right*/
          for (u8PositionIndex=20; u8PositionIndex>=1; u8PositionIndex--)
          {
            u8Line1Message[u8PositionIndex] = u8Line1Message[u8PositionIndex-1];
            u8Line2Message[u8PositionIndex] = u8Line2Message[u8PositionIndex-1];
          }
          u8Line2Message[0] = u8Line1Message[20];    //the solution to go to next line
          u8Line1Message[0] = ' ';                   //the soulution when touch the Line1 start
        }
        
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR,u8Line1Message);
        LCDMessage(LINE2_START_ADDR,u8Line2Message); 
      }
    }
    
    /*turn on the BUZZER1 in 1000HZ when 
    touch the Line1 start or Line2 end*/
    if (u8Line1Message[0]=='R' ||
        u8Line2Message[19] == 'i')
    {
      bShiftIsOn = FALSE;      //stop shift
      
      PWMAudioSetFrequency(BUZZER1,1000);
      PWMAudioOn(BUZZER1);
      bAlarmIsOn = TRUE;
    }
    else
    {
      bAlarmIsOn = FALSE;
    }
    
    /*turn on the music*/
    if (!bAlarmIsOn)
    {
      u8MusicTime++;
      
      /*change the music note per 100ms*/
      if (u8MusicTime == MUSIC_NOTE_TIME)
      {
        u8MusicTime = 0;
        PWMAudioSetFrequency(BUZZER1,u16MusicNote[s8MusicNoteIndex]);
        
        /*turn on the music when shift left*/
        if (bMusicLeftIsOn)
        {
          s8MusicNoteIndex--;
          
          if (s8MusicNoteIndex < 0)
          {
            s8MusicNoteIndex = 7;
          }
        }
        
        /*turn on the music when shift right*/
        else
        {
          s8MusicNoteIndex++;
          
          if (s8MusicNoteIndex > 7)
          {
            s8MusicNoteIndex = 0;
          }
        }
      }
    }
  }
  
  /*begin to display input Message in Debug*/
  if (u8Status == 2)
  {
    u8CharCount = DebugScanf(u8EachChar);
    
    /*display when input one char */
    if (u8CharCount == 1)
    {
      if (u8EachChar[0] == '\r')
      {
        DebugLineFeed();
      }
      /*display in Line1 when input chars*/
      if (bIsInLine1)
      {
        u8InputLine1Message[u8PositionIndex++] = u8EachChar[0];
        if (u8PositionIndex == 20)
        {
          u8PositionIndex = 0;
          bIsInLine1 = FALSE; 
        }
      }
      
      /* display in Line2 when input >20 chas*/
      else
      {
        u8InputLine2Message[u8PositionIndex++] = u8EachChar[0];
      }
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,u8InputLine1Message);
      LCDMessage(LINE2_START_ADDR,u8InputLine2Message); 
    }
    
    /*when input >40 chars begin to shift*/
    if (u8PositionIndex > 20)
    {
      bShiftIsOn = TRUE;
    }
    
    /*begin to shift*/
    if (bShiftIsOn)
    {
      u16TimeCounter++;
      
      /*shift message per 800ms*/
      if (u16TimeCounter == NAME_SHIFT_TIME)
      {
        u16TimeCounter = 0; 
        
        /*shift each char left*/
        for (u8PositionIndex=0; u8PositionIndex<31; u8PositionIndex++)
        {
          u8InputLine2Message[21] = u8InputLine1Message[0];
          u8InputLine1Message[u8PositionIndex] = u8InputLine1Message[u8PositionIndex+1];
          u8InputLine1Message[20] = u8InputLine2Message[0];
          u8InputLine2Message[u8PositionIndex] = u8InputLine2Message[u8PositionIndex+1];
        }
        
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR,u8InputLine1Message);
        LCDMessage(LINE2_START_ADDR,u8InputLine2Message); 
      }
    }   
  }
  
  /*Press BUTTON3 to return to main interface*/  
  if ( WasButtonPressed(BUTTON3) )
  {
    ButtonAcknowledge(BUTTON3);
    
    /*initial status*/
    u8Status = 0;
    bInitialize = TRUE;
    bShiftIsOn = FALSE;
    bIsInLine1 = TRUE;
    
    /*clear input chars*/
    memset(u8InputLine1Message,'\0',sizeof(u8InputLine1Message));
    memset(u8InputLine2Message,'\0',sizeof(u8InputLine2Message));
    
    
    PWMAudioOff(BUZZER1);
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
