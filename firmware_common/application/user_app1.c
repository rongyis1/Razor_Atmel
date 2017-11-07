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
extern u8 G_au8DebugScanfBuffer[];  /* From debug.c */
extern u8 G_u8DebugScanfCharCount;  /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_au8UserInputBuffer[U16_USER_INPUT_BUFFER_SIZE];

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
  
  for(u16 i = 0; i < U16_USER_INPUT_BUFFER_SIZE  ; i++)
  {
    UserApp_au8UserInputBuffer[i] = 0;
  }
 
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
  /*Declare and intialize variables*/
  static u8 u8NoteIndex     = 0;
  static u8 u8CharCount     = 0;
  static u8 u8BuzzerTime    = 0;
  static u8 u8CurrentStatus = 0;
  static bool bIsInputing = FALSE;
  static bool bInitial = TRUE;
  static bool bBuzzerIsOn = FALSE;
  static u8* pu8Address;
  static u8* u8MusicNote[]      = {"1234567",
                                   "QWERTYU",
                                   "ASDFGHJ",
                                   "ZXCVBNM"};
  static u8* u8ButtonMessage[]  = {"C3 Scale--Input Number 1-7: ",
                                   "C4 Scale--Input Number Q-U: ",
                                   "C5 Scale--Input Number A-J: ",
                                   "C6 Scale--Input Number Z-M: "};
  static u8* u8DefaultMessage[] = {"\n\rWrong Input!  Please Input Number 1-7: ",
                                   "\n\rWrong Input!  Please Input Number Q-U: ",
                                   "\n\rWrong Input!  Please Input Number A-J: ",
                                   "\n\rWrong Input!  Please Input Number Z-M: "};
  static u8 u8InitialMessage[]  = "\n\rPress BUTTON 0-3 TO Choose Musical Scale";
  u32 au32Tone[8][4]={{C3,C4,C5,C6},
                      {D3,D4,D5,D6},
                      {E3,E4,E5,E6},
                      {F3,F4,F5,F6},
                      {G3,G4,G5,G6},
                      {A3,A4,A5,A6},
                      {B3,B4,B5,B6}};
  
  /*enter the initia interface*/
  if (bInitial) 
  {
    DebugPrintf(u8InitialMessage);
    bInitial = FALSE;
  }
  
  /*Press BUTTON 0-3 TO Choose Musical Scale*/
  if ( WasButtonPressed(BUTTON0) )
  {
    ButtonAcknowledge(BUTTON0);
    u8CurrentStatus = 1;                                  //Status :C3
    DebugLineFeed();
    DebugPrintf(u8ButtonMessage[u8CurrentStatus-1]);
    bIsInputing = TRUE;                                  //begin to input character
  }
  if ( WasButtonPressed(BUTTON1) )
  {
    ButtonAcknowledge(BUTTON1);
    u8CurrentStatus = 2;                                 //Status :C4
    DebugLineFeed();
    DebugPrintf(u8ButtonMessage[u8CurrentStatus-1]);
    bIsInputing = TRUE; 
  }
  if ( WasButtonPressed(BUTTON2) )
  {
    ButtonAcknowledge(BUTTON2);
    u8CurrentStatus = 3;                                 //Status :C5
    DebugLineFeed();
    DebugPrintf(u8ButtonMessage[u8CurrentStatus-1]);
    bIsInputing = TRUE; 
  }
  if (  WasButtonPressed(BUTTON3) )
  {
    ButtonAcknowledge(BUTTON3);
    u8CurrentStatus = 4;                                 //Status :C6
    DebugLineFeed();
    DebugPrintf(u8ButtonMessage[u8CurrentStatus-1]);
    bIsInputing = TRUE; 
  }
  
  /*input the correct characters to turn on the Buzzer*/
  if (bIsInputing)
  {
    u8CharCount = DebugScanf(UserApp_au8UserInputBuffer);
    if (u8CharCount == 1)
    {
      UserApp_au8UserInputBuffer[u8CharCount] = '\0';
      pu8Address = (u8*)strstr((char*)u8MusicNote[u8CurrentStatus-1],(char*)UserApp_au8UserInputBuffer); 
      
      /*check if the character correct*/
      if (pu8Address)
      {
        bBuzzerIsOn = TRUE;        //turn on the BUZZER1
      }
      else
      {
        PWMAudioOff(BUZZER1);
        DebugPrintf(u8DefaultMessage[u8CurrentStatus-1]);
      }
    }    
  }
  else
  {
    u8CharCount = DebugScanf(UserApp_au8UserInputBuffer);
    if(u8CharCount > 0)
    {
      DebugPrintf(u8InitialMessage);
    }
  }
  
  /*turn on the buzzer and turn off it when reaching 100ms*/
  if (bBuzzerIsOn)
  {
    u8BuzzerTime++;
    u8NoteIndex = pu8Address - u8MusicNote[u8CurrentStatus-1];        //Choose current musical note
    PWMAudioSetFrequency(BUZZER1,au32Tone[u8NoteIndex][u8CurrentStatus-1]);   //set the BUZZER1 frequency
    PWMAudioOn(BUZZER1);
    
    /*turn off the BUZZER1 when reaching 100ms*/
    if (u8BuzzerTime == BUZZER_ON_TIME)
    {
      u8BuzzerTime = 0;
      PWMAudioOff(BUZZER1);
      bBuzzerIsOn = FALSE;
    }
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
