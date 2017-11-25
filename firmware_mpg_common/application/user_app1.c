/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

Test1.
Test2 from Engenuics.
Test3.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */


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
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  
    /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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
/* Wait for input */
static void UserApp1SM_Idle(void)
{
  static bool bDisplayMessage  = TRUE;
  static bool bCheckInputIsOn  = FALSE;
  static bool bShowCurrentList = FALSE;
  static bool bShowNewList     = FALSE;
  static bool bDisplayCheck    = FALSE;
  static bool bFindError       = FALSE;
  static bool bModifyCommand   = FALSE;
  static bool bEndStatus       = FALSE;
 
  static u8 u8DebugDelayTime    = 0;
  static u8 u8Status            = 0;
  static u8 u8LinkCount         = 0;
  static u8 u8CommandLength     = 0;
  static u8 u8CheckIndex        = 0;
  static u8 u8CheckCount        = 0;
  static u8 u8TotalCommandIndex = 0;
  static u8 u8ListEntry         = 0;
  static u8 u8AddCommandIndex   = 0;
  
  static u8 u8EachChar[1];
  static u8 u8CheckLed[3];
  static u8 u8SetPassword[6];
  static u8 u8EnsurePassword[6];
  static u8 u8CheckStartTime[10];
  static u8 u8CheckStopTime[10];
  static u8 u8InputMessage[100];
  static LedCommandType aeUserList[100];
  static u8 u8LedMessage[] = "WPBCGYORwpbcgyor";
  static u8 u8PasswordMessage[] = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
    
  static u8 u8IntialMessage1[]  = "\n\r****************************************";
  static u8 u8IntialMessage2[]  = "\n\rLED Programming Interface";
  static u8 u8IntialMessage3[]  = "\n\rPress 1 to programe LED command sequence";
  static u8 u8IntialMessage4[]  = "\n\rPress 2 to show current USER program    ";
  static u8 u8IntialMessage5[]  = "\n\rPress 3 to clear the current USER list  ";
  static u8 u8IntialMessage6[]  = "\n\rPress 4 to reset the password";
  static u8 u8IntialMessage7[]  = "\n\rWelcome to FW1 Final Project";
  static u8 u8IntialMessage8[]  = "\n\rPlease set password for future use";
  static u8 u8IntialMessage9[]  = "\n\rThe password consists of six characters (0-9/a-z/A-Z)\n\rPASSWORD:";
  
  static u8 u8CreateMessage1[]  = "\n\rEnter commands as LED-ONTIME-OFFTIME and press enter";
  static u8 u8CreateMessage2[]  = "\n\rTime is in milliseconds,amsx 100 commands";
  static u8 u8CreateMessage3[]  = "\n\rLed colors: R,O,Y,G,C,B,P,W";
  static u8 u8CreateMessage4[]  = "\n\rExample: R-100-200(Red on at 100ms and off at 200ms)";
  static u8 u8CreateMessage5[]  = "\n\rInput invalid:Press 'Enter' to reinput or 'Tab' to modify";
  static u8 u8CreateMessage6[]  = "\n\rPress Enter on blank line to end";
  
  static u8 u8DisplayMessage1[] = "\n\rCurrent User Program:";
  static u8 u8DisplayMessage2[] = "\n\r\n\rLED  ON-TIME   OFF-TIME\n\r";
  static u8 u8DisplayMessage3[] = "-----------------------";
  static u8 u8DisplayMessage4[] = "\n\rPress Enter to show new add programs\n\r";
  static u8 u8DisplayMessage5[] = "\n\rCommand entry complete";
  static u8 u8DisplayMessage6[] = "\n\rCommands entered: ";
  static u8 u8DisplayMessage7[] = "\n\rNew Add Program:";
  static u8 u8DisplayMessage8[] = "\n\rPress Enter to return main interface\n\r";
  static u8 u8DisplayMessage0[] = "\n\rTotal Commands:";
  
  static u8 u8ClearMessage1[]   = "\n\rBegin to clear the list...";
  static u8 u8ClearMessage2[]   = "\n\rPlease input password to ensure:";
  static u8 u8ClearMessage3[]   = "\n\rClear all commands alredy!";
  static u8 u8ClearMessage4[]   = "\n\rPlease readd new commands!";
  
  /*The status is used to set password*/
  if (u8Status == SET_PASSWORD_STATUS)
  {
    /*initial display*/
    if (bDisplayMessage)
    {
      bDisplayMessage = FALSE;      
      u8CheckIndex = 0;
      
      DebugPrintf(u8IntialMessage1);
      DebugPrintf(u8IntialMessage7);
      DebugPrintf(u8IntialMessage8);
      DebugPrintf(u8IntialMessage9);
    }
    
    /*check each char you have inputted*/
    if (DebugScanf(u8EachChar) == 1)
    {
      if (u8CheckIndex <= 5)
      {
        /*check the password element is (0-9)number or (a-z/A-Z)character*/
        if (strstr((char*)u8PasswordMessage,(char*)u8EachChar))
        {
          u8SetPassword[u8CheckIndex++] = u8EachChar[0];
        }
        else
        {
          DebugPrintf("\t\t\tPlease input correct number or character!\n\rPASSWORD:");
          memset((char*)u8SetPassword,'\0',6);   //clear the array when input wrong element for reinput  
          memset((char*)u8EachChar,'\0',1);      //clear the char to avoid input 'enter'
          u8CheckIndex = 0;
        }
        
        /*instruction when input six password elements correctly*/
        if (u8CheckIndex == 6)
        {
          DebugPrintf("\t\tPress 'Enter' to ensure the password!");
        }
      }     
      
      /*Press 'Enter' to continue*/
      if (u8EachChar[0] == '\r')
      {
        u8Status = MAIN_INTERFACE_STATUS;
        bDisplayMessage = TRUE;
        DebugPrintf(u8IntialMessage1);
        DebugLineFeed();
      }  
    }
  }
  
  /*Display the main interface*/
  if (u8Status == MAIN_INTERFACE_STATUS)
  { 
    if (bDisplayMessage)
    {
      bDisplayMessage = FALSE;
      DebugPrintf(u8IntialMessage1);
      DebugPrintf(u8IntialMessage2);
      DebugPrintf(u8IntialMessage3);
      DebugPrintf(u8IntialMessage4);
      DebugPrintf(u8IntialMessage5);
      DebugPrintf(u8IntialMessage6);
      DebugPrintf(u8IntialMessage1);
      DebugLineFeed();
    }
    
    if (DebugScanf(u8EachChar) == 1)
    {    
      /*choose 1-4 to enter in the corresponding state*/ 
      switch (u8EachChar[0])
      {
        case '1':
          u8Status = ADD_COMMAND_STATUS;      //go to the status to add command 
          u8AddCommandIndex = 0;
          break;
        case '2':
          u8Status = CURRENT_PROGRAM_STATUS;  //go to the status to show commands
          u8ListEntry = 0;
          break;
        case '3':
          u8Status = CLEAR_LIST_STATUS;       //go to the status to clear list
          break;
        case '4':
          u8Status = SET_PASSWORD_STATUS;     //go to the status to set password
          break;
        default:
          DebugPrintf("Input Wrong Status Command");
          break;      
      }
      
      bDisplayMessage = TRUE;
      DebugLineFeed();
    }
  }

  /*the status to add commands to the list*/
  if (u8Status == ADD_COMMAND_STATUS)
  {
    /*show instruction*/
    if (bDisplayMessage)
    {
      bDisplayMessage = FALSE;
      DebugPrintf(u8IntialMessage1);
      DebugPrintf(u8CreateMessage1);
      DebugPrintf(u8CreateMessage2);
      DebugPrintf(u8CreateMessage3);
      DebugPrintf(u8CreateMessage4);
      DebugPrintf(u8CreateMessage5);
      DebugPrintf(u8CreateMessage6);
      DebugPrintf("\n\r1:");
    }
    
    if (DebugScanf(u8EachChar) == 1)
    {        
      u8InputMessage[u8CommandLength++] = u8EachChar[0];
      
      /*Press 'Enter' to begin to check input command*/
      if (u8EachChar[0] == '\r')
      {
        bCheckInputIsOn = TRUE;
      }
      
      /*Press 'Backspace' to begin to delete chars*/
      if (u8EachChar[0] == '\b')
      {
        u8CommandLength -= 2;
      }
      
      /*Press 'Enter' on blank line to end*/
      if (u8InputMessage[0] == '\r')
      {
        memset(u8InputMessage,'\0',INPUT_COMMAND_SIZE);   //clear the command array
        
        bCheckInputIsOn = FALSE;
        bDisplayMessage = TRUE;
        
        u8CommandLength = 0;                              //return to the  first char
        u8Status = MAIN_INTERFACE_STATUS;                 //return to main interface
        
        DebugPrintf(u8IntialMessage1);
        DebugLineFeed();       
      }
    }
    
    /*begin to check the input command*/
    if (bCheckInputIsOn)
    {
      bCheckInputIsOn = FALSE;
      u8Status = CHECK_COMMAND_END_STATUS;        //go to the CHECK_COMMAND_END_STATUS when finish checking
      
      /*use 'for' to check the input command*/
      for (u8CheckIndex=0; u8CheckIndex<u8CommandLength; u8CheckIndex++)
      {
        /*separate the input command with '-'*/
        if (u8InputMessage[u8CheckIndex] == '-' ||
            u8InputMessage[u8CheckIndex] == '\r')
        {
          u8LinkCount++;
          u8CheckCount = (++u8CheckIndex);
        }
        
        /*put the chars before the first '-' in the u8CheckLed array*/
        if (u8LinkCount == 0)
        {
          u8CheckLed[u8CheckIndex] = u8InputMessage[u8CheckIndex];
        }
         
        /*put the chars before the second '-' in the u8CheckStartTime array*/
        if (u8LinkCount == 1)
        {
          /*check if the u8CheckLed in  the  u8LedMessage*/
          if (strstr((char*)u8LedMessage,(char*)u8CheckLed))
          {  
            /*check if the u8CheckStartTime is valid*/
            if (u8InputMessage[u8CheckIndex] >= 0x30 &&
                u8InputMessage[u8CheckIndex] <= 0x39)
            {
              //put the valid start time in the u8CheckStartTime
              u8CheckStartTime[u8CheckIndex-u8CheckCount] = u8InputMessage[u8CheckIndex];  
            }
            else
            { 
              //display the error when the u8CheckStartTime is invalid
              DebugPrintf("\t\t\t(Invalid character or space in start time)");
              bFindError = TRUE;
              break;
            }
          }
          else
          { 
            //display the error when the u8CheckLed is invalid
            DebugPrintf("\t\t\t(");
            DebugPrintf(u8CheckLed);
            DebugPrintf(" is not a valid LED)");
            bFindError = TRUE;
            break;
          }
        }
        
        /*put the chars after the second '-' in the u8CheckStopTime array*/
        if (u8LinkCount == 2)
        {  
          /*check if the u8CheckStopTime is valid*/
          if (u8InputMessage[u8CheckIndex] >= 0x30 &&
              u8InputMessage[u8CheckIndex] <= 0x39)
          {
            //put the valid start time in the u8CheckStopTime
            u8CheckStopTime[u8CheckIndex-u8CheckCount] = u8InputMessage[u8CheckIndex];
          }        
          else
          {
            //display the error when the u8CheckStopTime is invalid
            DebugPrintf("\t\t\t(Invalid character or space in stop time)");
            bFindError = TRUE;
            break;
          }
        }

        if (u8LinkCount == 3)
        {
          /*display the error when the '-' appear after u8CheckStopTime*/
          if (u8InputMessage[u8CheckIndex-1] == '-')
          {
            DebugPrintf("\t\t\t(Input a more '-' after ");
            DebugPrintf(u8CheckStopTime);
            DebugPrintf(")"); 
            bFindError = TRUE;
            break;
          }
          
          /*display the error when the u8CheckStartTime less than u8CheckStopTime 200ms*/
          if ((atoi((char*)u8CheckStopTime)-atoi((char*)u8CheckStartTime)) < 200)
          {
            DebugPrintf("\t\t\t(OFF-TIME must larger than ON-TIME 200ms)");
            bFindError = TRUE;
            break;
          }
          
          /*if the u8CheckLed is valid and specify the corresponding leds*/
          switch(u8CheckLed[0])
          {
            case 'R':
              aeUserList[u8AddCommandIndex].eLED = RED;
              break;
            case 'O':
              aeUserList[u8AddCommandIndex].eLED = ORANGE;
              break;
            case 'Y':
              aeUserList[u8AddCommandIndex].eLED = YELLOW;
              break;
            case 'G':
              aeUserList[u8AddCommandIndex].eLED = GREEN;
              break;
            case 'C':
              aeUserList[u8AddCommandIndex].eLED = CYAN;
              break;
            case 'B':
              aeUserList[u8AddCommandIndex].eLED = BLUE;
              break;
            case 'P':
              aeUserList[u8AddCommandIndex].eLED = PURPLE;
              break;
            case 'W':
              aeUserList[u8AddCommandIndex].eLED = WHITE;
              break;
            default:
              break;
          }
          
          /*add the command to the list and there are two structs added*/
          aeUserList[u8AddCommandIndex].bOn = TRUE;
          aeUserList[u8AddCommandIndex++].u32Time = atoi((char*)u8CheckStartTime);
          aeUserList[u8AddCommandIndex].eLED = aeUserList[u8AddCommandIndex-1].eLED;
          aeUserList[u8AddCommandIndex].bOn = FALSE;
          aeUserList[u8AddCommandIndex].u32Time = atoi((char*)u8CheckStopTime); 
          LedDisplayAddCommand(USER_LIST, &aeUserList[u8AddCommandIndex-1]);
          LedDisplayAddCommand(USER_LIST, &aeUserList[u8AddCommandIndex++]);

          bDisplayCheck = TRUE;          
          DebugLineFeed();
        }
      }
    }
  }
  
  /*the status when finish checking the command*/ 
  if (u8Status == CHECK_COMMAND_END_STATUS)
  {  
    /*check if the command is invalid*/
    if (bFindError)
    {
      /*turn on the BUZZER1*/
      PWMAudioSetFrequency(BUZZER1,1000);
      PWMAudioOn(BUZZER1);
      
      if (DebugScanf(u8EachChar) == 1)
      {
        /*press 'Enter' or 'Tab' to turn off the BUZZER1*/
        if (u8EachChar[0] == '\r' || u8EachChar[0] == '\t')
        {
          bDisplayCheck = TRUE; 
          bFindError = FALSE;
          
          DebugLineFeed();
          PWMAudioOff(BUZZER1);   //turn off the BUZZER1
          
          /*Press 'Tab' to modify the invalid command*/
          if (u8EachChar[0] == '\t')
          {
            bModifyCommand = TRUE;        
          }    
        }
        else
        {
          DebugPrintf("\n\rInput wrong character!  Please press 'Enter'or 'Tab'!");
        }
      }
    }
    if (bDisplayCheck)
    {
      bDisplayCheck = FALSE;      
      
      memset(u8CheckLed,'\0',CHECK_LED_SIZE);
      memset(u8CheckStartTime,'\0',CHECK_START_TIME_SIZE);
      memset(u8CheckStopTime,'\0',CHECK_STOP_TIME_SIZE);
    
      DebugPrintNumber((u8AddCommandIndex/2)+1);
      DebugPrintf(":");
      
      u8LinkCount = 0;
      u8Status = ADD_COMMAND_STATUS;        //return to main interface
      
      /*modify the invalid command*/
      if (bModifyCommand)
      {
        bModifyCommand = FALSE;       
        u8InputMessage[u8CommandLength-1] = '\0';
        DebugPrintf(u8InputMessage);        
        u8CommandLength -= 1;
      }
      /*reinput the command*/
      else
      {
        memset(u8InputMessage,'\0',INPUT_COMMAND_SIZE);
        u8CommandLength = 0;
      }
    }
  }
  
  /*the status to show current list commands*/
  if (u8Status == CURRENT_PROGRAM_STATUS)
  {
    u8DebugDelayTime++;
    if (bDisplayMessage)
    {
      u8TotalCommandIndex += u8AddCommandIndex;
      
      bDisplayMessage = FALSE;
      bShowCurrentList = TRUE;
      
      DebugPrintf(u8DisplayMessage0);
      DebugPrintNumber(u8TotalCommandIndex/2);
      DebugPrintf(u8DisplayMessage1);
      DebugPrintf(u8DisplayMessage2);
      DebugPrintf(u8DisplayMessage3);
      DebugLineFeed();     
    }
    
    /*display one list command "LED,ONTIME,OFFTIME" per 5ms*/
    if (u8DebugDelayTime == DEBUG_DELAY_TIME && bShowCurrentList)
    {
      u8DebugDelayTime = 0;
      if (u8ListEntry < (u8TotalCommandIndex/2))
      {
        LedDisplayPrintListLine(u8ListEntry);
        u8ListEntry++;
      }
      else
      {
        bShowCurrentList = FALSE; 
        u8ListEntry=(u8TotalCommandIndex-u8AddCommandIndex)/2;
        DebugPrintf(u8DisplayMessage3);
        DebugPrintf(u8DisplayMessage4);
      }
    }
     
    /*Press 'Enter' to display the commands new added*/
    if (DebugScanf(u8EachChar) == 1)
    {
      if (u8EachChar[0] == '\r')
      {
        u8DebugDelayTime = 0;
        u8Status = NEW_PROGRAM_STATUS;
        bDisplayMessage = TRUE;
      }
    }          
  }
  
  /*the status to new added current list commands*/
  if (u8Status == NEW_PROGRAM_STATUS)
  {
    u8DebugDelayTime++;
    if (bDisplayMessage)
    {
      bDisplayMessage = FALSE;
      bShowNewList = TRUE;
      
      DebugPrintf(u8DisplayMessage5);
      DebugPrintf(u8DisplayMessage6);
      DebugPrintNumber(u8AddCommandIndex/2);
      DebugPrintf(u8DisplayMessage7);
      DebugPrintf(u8DisplayMessage2);
      DebugPrintf(u8DisplayMessage3);
      DebugLineFeed();
    }
    
    /*display one list command "LED,ONTIME,OFFTIME" per 5ms*/
    if (u8DebugDelayTime == DEBUG_DELAY_TIME && bShowNewList)
    {
      u8DebugDelayTime = 0;
      if (u8ListEntry < (u8TotalCommandIndex/2))
      {
        LedDisplayPrintListLine(u8ListEntry);
        u8ListEntry++;
      }
      else
      {
        bShowNewList = FALSE; 
        u8AddCommandIndex = 0;
        DebugPrintf(u8DisplayMessage3);
        DebugPrintf(u8DisplayMessage8);
      }
    }
     
    /*Press 'Enter' to return to main interface*/
    if (DebugScanf(u8EachChar) == 1)
    {
      if (u8EachChar[0] == '\r')
      {
        bDisplayMessage = TRUE;
        u8DebugDelayTime = 0;
        u8Status = MAIN_INTERFACE_STATUS;
      }
    }   
  }
  
  /*the status to clear all current list commands*/
  if (u8Status == CLEAR_LIST_STATUS)
  {
    if (bDisplayMessage)
    {
      bDisplayMessage = FALSE;      
      u8CheckIndex = 0; 
      DebugPrintf(u8IntialMessage1);
      DebugPrintf(u8ClearMessage1);
      DebugPrintf(u8ClearMessage2);
    }
    
    /*input password to ensure and for security*/
    if (DebugScanf(u8EachChar) == 1)
    {
      if (u8CheckIndex <= 5)
      {
        u8EnsurePassword[u8CheckIndex++] = u8EachChar[0];
        
        if (u8CheckIndex == 6)
        {
          /* check if the password is correct*/
          if ( !strcmp( (char*)u8EnsurePassword, (char*)u8SetPassword) )
          {
            DebugPrintf("\t\tPassword is correct!");
            DebugPrintf("\n\rAre you sure to clear the list?   (Press Y/N:)");
            bEndStatus = TRUE;
          }
          else
          {
            DebugPrintf("\t\tPassword is wrong!");
            DebugPrintf(u8DisplayMessage8);
            bEndStatus = TRUE;
          }
        }    
      }
      
      if (bEndStatus)
      {
        /*Press 'Y' to ensure to clear all commands*/
        if (u8EachChar[0] == 'Y')
        {
          LedDisplayStartList();        //clear the List         
          bEndStatus = FALSE;
          bDisplayMessage = TRUE;
          
          u8Status = MAIN_INTERFACE_STATUS;
          u8AddCommandIndex = 0;
          u8TotalCommandIndex = 0;
          
          DebugPrintf(u8ClearMessage3);
          DebugPrintf(u8ClearMessage4);
          DebugPrintf(u8IntialMessage1);
          DebugLineFeed();
        }
        
        /*Press 'N' or the password is incorrect 
        then return to main interface to give up clearing*/
        if (u8EachChar[0] == 'N' || u8EachChar[0] == '\r')
        {
          bEndStatus = FALSE;
          bDisplayMessage = TRUE;
          u8Status = MAIN_INTERFACE_STATUS;
          DebugPrintf(u8IntialMessage1);
          DebugLineFeed();
        }
      }
    }
  }

  
} /* end UserApp1SM_Idle() */
                      
            
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
