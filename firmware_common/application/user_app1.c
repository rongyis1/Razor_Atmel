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
  //LCDMessage of  different states
  static u8 au8LCDMessage1[] = "STATE1";
  static u8 au8LCDMessage2[] = "STATE2";
  static u16 u16TimeCounter = 0;
  static bool bBuzzerIsOn = FALSE;
  
  /*Press BUTTON1 to enter state 1*/
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1); 
    
    DebugPrintf("Entering state 1\n\r");    //Debug out message when pressing BUTTON1
    
    /*LCD operations*/
    LCDCommand(LCD_CLEAR_CMD);             //clear the ever LCDMessage
    LCDMessage(LINE1_START_ADDR, au8LCDMessage1);  //LCDMessage :State 1
    
    /*LCD Back Light : PURPLE 
      (make up of LCD_RED and LCD_BLUE)*/
    LedOff(LCD_GREEN);
    LedOn(LCD_RED);
    LedOn(LCD_BLUE);
    
    /*LED operations,turn on WHITE,PURPLE,BLUE,CYAN*/
    LedOff(RED);
    LedOff(ORANGE);
    LedOff(YELLOW);
    LedOff(GREEN);
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    
    bBuzzerIsOn = FALSE;  //turn off the buzzer
    
  }
  
  /*Press BUTTON2 to enter state 1*/
  if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2); 
    
    DebugPrintf("Entering state 2\n\r");    //Debug out message when pressing BUTTON2
    
    /*LCD operations*/
    LCDCommand(LCD_CLEAR_CMD);             //clear the ever LCDMessage
    LCDMessage(LINE1_START_ADDR, au8LCDMessage2);  //LCDMessage :State 2
    
    /*LCD Back Light : ORANGE 
      (make up of LCD_RED PWM100 and LCD_GREEN PWM35)*/
    LedOff(LCD_BLUE);
    LedPWM(LCD_RED,LED_PWM_100);
    LedPWM(LCD_GREEN,LED_PWM_35);
    
    /*LED operations,turn on RED 8HZ,ORANGE 4HZ,YELLOW 2HZ,GREEN 1HZ*/
    LedBlink(RED,LED_8HZ);
    LedBlink(ORANGE,LED_4HZ);
    LedBlink(YELLOW,LED_2HZ);
    LedBlink(GREEN,LED_1HZ);
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    
    /*turn on the buzzer and begin to count time*/
    u16TimeCounter = 0;
    bBuzzerIsOn = TRUE;
  }
  
  if (bBuzzerIsOn)
  {
    u16TimeCounter++;
  
    /*BUZZER operations : 100ms 200HZ tone every second*/
    if (u16TimeCounter <= BUZZER_TIME)             //turn on the buzzer 100ms
    {
      PWMAudioSetFrequency(BUZZER1, 200);  //200HZ
      PWMAudioOn(BUZZER1);
    }
    else            //turn off the buzzer over 100ms
    {
      PWMAudioOff(BUZZER1);
    }
    
    if (u16TimeCounter == ONE_SECOND)  //restart to count time every second
    {
      u16TimeCounter = 0;
    }
  }
  
  else
  {
    PWMAudioOff(BUZZER1);   //turn off the buzzer in state 1
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
