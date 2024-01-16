/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/

#define PASS_SIZE 10

/* Struct */
typedef enum
{
  LOCKED,
  UNLOCKED,
  MODIFY,
} state;

static struct
{
  u8 u8confSet;
  state u8State;
  // Contains numbers between 1 - 3
  u32 u32Pass[PASS_SIZE];
  // Indexer for u32Pass
  u32 u32pp;
  // Pointer to elements in array
  u32 u32Entry[PASS_SIZE];
  // Indexer for u32Entry
  u32 u32ep;
} machine;

/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  
  // Set initial machine state as well as default password if not been set
  // Sets inital lED configs
  LcdCommand(LCD_CLEAR_CMD);
  LedOff(RED);
  LedOn(YELLOW);
      
  // Sets default password as 1-2-3-0-0-0-0-0-0-0
  for (size_t i = 0; i < PASS_SIZE; i++) {
    if (i < 3)
      machine.u32Pass[i] = (int)i + 1;
    else
      machine.u32Pass[i] = 0;
  }
      
  // Sets entry as zero for password input
  for (size_t i = 0; i < PASS_SIZE; i++) {
    machine.u32Entry[i] = 0;
  }
      
  // Allow the user to initially modify password, it will be 
  // locked after 3 seconds
  machine.u8State = MODIFY;
  machine.u8confSet = 1;
  machine.u32ep = 0;
  machine.u32pp = 3;
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Runs every 1 ms*/ 
static void UserApp1SM_Idle(void)
{
    static u32 u32dTimer = 0;
    u8 au8Munlock[] = "Correct password.";
    u8 au8Mlock[] = "Wrong password.";
    u8 au8Minput[] = "Enter your password.";
        
    // 3-Second timer for password entry
    if ((u32dTimer < 3000) && !WasButtonPressed(BUTTON3)) {
       u32dTimer++;
    }
    else if (u32dTimer == 3000) {
       // Prevent password modification
       LedOn(RED);
       LedOff(YELLOW);
       machine.u8State = LOCKED;
    }
    
    // Password modification (JUST THIS ONE LEFT!!! :))
    if (IsButtonPressed(BUTTON3) && machine.u8State == MODIFY) {
      
      // Led configs
      LedOff(YELLOW);
      LedBlink(GREEN, LED_8HZ);
      LedBlink(RED, LED_8HZ);
      // Entry message
      LcdCommand(LCD_CLEAR_CMD);
      LcdMessage(LINE1_START_ADDR, au8Minput);
    }
    if (WasButtonPressed(BUTTON3) && machine.u8State == MODIFY) {
      // User password input
      if (IsButtonPressed(BUTTON0)) {
        machine.u32Entry[machine.u32pp] = 1;
        machine.u32pp++;
      }
      else if (IsButtonPressed(BUTTON1)) {
        machine.u32Entry[machine.u32pp] = 2;
        machine.u32pp++;
      }
      else if (IsButtonPressed(BUTTON2)) {
        machine.u32Entry[machine.u32pp] = 3;
        machine.u32ep++;
      } else if (IsButtonPressed(BUTTON3)) {
        ButtonAcknowledge(BUTTON3);
        // Ensure locked state
        LedOff(GREEN);
        LedBlink(RED, LED_0HZ);
        machine.u8State = LOCKED; 
      }
    }
    // Password testing
    else if (machine.u8State == LOCKED) {
      LedOff(BLUE);
      LedOff(GREEN);
      LedOff(YELLOW);
      LedOff(PURPLE);
      
      // Take input
      if (WasButtonPressed(BUTTON3)) {
        ButtonAcknowledge(BUTTON3);
        
        // Test protocols
        u8 u8isMatch = 1;
        if (machine.u32ep == machine.u32pp) {
          for (size_t i = machine.u32pp; i > 0; i--) {
            if (machine.u32Pass[i] != machine.u32Entry[i]) {
              u8isMatch = 0;
              break;
            }
          }
        }
        else {
          u8isMatch = 0;
        }
        // Clear the entry array to accept new input in next iteration
        for (size_t i = 0; i < PASS_SIZE; i++) {
          machine.u32Entry[i] = 0;
        }
        machine.u32ep = 0;
        if (u8isMatch == 1) {
          // Correct
          machine.u8State = UNLOCKED;
          LedBlink(GREEN, LED_8HZ);
          LcdCommand(LCD_CLEAR_CMD);
          LcdMessage(LINE1_START_ADDR, au8Munlock);
        }
        else {
          // Wrong
          machine.u8State = LOCKED;
          LedBlink(RED, LED_8HZ);
          LcdCommand(LCD_CLEAR_CMD);
          LcdMessage(LINE1_START_ADDR, au8Mlock);
        }
      }
      
      else if (WasButtonPressed(BUTTON0)) {
        // This doesn't increment the pointer?
        machine.u32Entry[machine.u32ep] = 1;
        machine.u32ep++;
        LedOn(YELLOW);
        ButtonAcknowledge(BUTTON0);
      }
      else if (WasButtonPressed(BUTTON1)) {
        machine.u32Entry[machine.u32ep] = 2;
        machine.u32ep++;
        LedOn(PURPLE);
        ButtonAcknowledge(BUTTON1);
      }
      else if (WasButtonPressed(BUTTON2)) {
        machine.u32Entry[machine.u32ep] = 3;
        machine.u32ep++;
        LedOn(GREEN);
        ButtonAcknowledge(BUTTON2);
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
