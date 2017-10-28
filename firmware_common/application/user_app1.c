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
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
 
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
  /*declare and define variables*/
  static u8 u8Index = 0;
  static bool bResetIsOn = TRUE;
  static u16 auPasswordNew[]     = {0,0,0,0,0,0,0,0,0,0};
  static u16 auPasswordElement[] = {0,0,0,0,0,0,0,0,0,0};
  
  /*judge whether the BUTTON3 held*/
  if ( IsButtonHeld(BUTTON3,1000) )
  {
    ButtonAcknowledge(BUTTON3);//clear the BUTTON3 log
    u8Index = 0;           
    LedBlink(RED,LED_4HZ);     //blink the RED LED
    LedBlink(GREEN,LED_4HZ);   //blink the GREEN LED
    bResetIsOn = TRUE;         //begin to reset the password
  }
  
  /*judge whether beginning to reset*/
  if (bResetIsOn)
  {
    /*BUTTON0 = Password  '0'*/
    if ( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      auPasswordNew[u8Index] = PASSWORD1;
      u8Index++;    
    }
    
    /*BUTTON0 = Password  '1'*/
    if ( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      auPasswordNew[u8Index] = PASSWORD2;
      u8Index++;
    }
    
    /*BUTTON0 = Password  '2'*/
    if ( WasButtonPressed(BUTTON2) )
    {
      ButtonAcknowledge(BUTTON2);
      auPasswordNew[u8Index] = PASSWORD3;
      u8Index++;   
    }
    
    /*judge whether to enter ten digits*/
    if (u8Index > 9)
    {
      /*Press BUTTON3 to save and affirm the password*/
      if ( WasButtonPressed(BUTTON3) )
      {
        ButtonAcknowledge(BUTTON3);
        u8Index = 0;          //return to first element
        bResetIsOn = FALSE;   //finish resetting the password
        LedOff(GREEN);        //turn off the Green Led
        LedOn(RED);           //turn on the Red Led
      }
    }
    else  
    {
      ButtonAcknowledge(BUTTON3);
    }
  }
  
  /*begin to determine if the password is correct*/
  else
  {
    /*BUTTON0 = Password  '0'*/
    if ( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      auPasswordElement[u8Index] = PASSWORD1;
      u8Index++;    
    }
    
    /*BUTTON0 = Password  '1'*/
    if ( WasButtonPressed(BUTTON1) )
    {
      ButtonAcknowledge(BUTTON1);
      auPasswordElement[u8Index] = PASSWORD2;
      u8Index++;
    }
    
    /*BUTTON0 = Password  '2'*/
    if ( WasButtonPressed(BUTTON2) )
    {
      ButtonAcknowledge(BUTTON2);
      auPasswordElement[u8Index] = PASSWORD3;
      u8Index++;   
    }
    
    /*press BUTTON3 to determine if the password is correct*/
    if ( WasButtonPressed(BUTTON3) )
    {
      ButtonAcknowledge(BUTTON3);
      
      /*determine if the two array is equal*/  
      for (u8Index = 0;u8Index<10;u8Index++)
      {
        if (auPasswordElement[u8Index] != auPasswordNew[u8Index])
        {
          break;
        }
      }
      if (u8Index < 10)
      {
        LedOff(GREEN);
        LedBlink(RED,LED_4HZ);
        u8Index = 0;
      }
      else
      {
        LedOff(RED);
        LedBlink(GREEN,LED_4HZ);
        u8Index = 0;
      }
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
