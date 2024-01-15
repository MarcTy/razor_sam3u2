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
  MODIFY
} state;

typedef struct
{
  u8 u8confSet;
  state u8State;
  // Contains numbers between 1 - 3
  u16 u16Pass[PASS_SIZE];
  // Indexer for u16Pass
  u8 u8pp;
  // Pointer to elements in array
  u16 u16Entry[PASS_SIZE];
  // Indexer for u16Entry
  u8 u8ep;
} mach;

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

void initMach(mach *machine);

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
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
    // Just debug your logic, I think its gtg...
    static mach machine;
    static u32 u32dTimer = 0;
    static u8 u8passAllow = 0;
    
    
    if (!(machine.u8confSet)) {
      // Set initial machine state as well as default password if not been set
      initMach(&machine);
    } 
    
    // 3-Second timer for password entry
    if (u32dTimer < 3000) {
      u32dTimer++;
    }
    else if (u32dTimer == 3000) {
      // Prevent password modification
      LedToggle(RED);
      LedToggle(YELLOW);
      machine.u8State = LOCKED;
    }
    
    // Password modification
    if (WasButtonPressed(BUTTON3) && machine.u8State == MODIFY) {
      
      // Led configs
      LedOn(GREEN);
      LedOn(RED);
      LedBlink(GREEN, LED_2HZ);
      LedBlink(RED, LED_2HZ);
      ButtonAcknowledge(BUTTON3)
      
      //User password input
      while (!IsButtonPressed(BUTTON3) || (machine.u8pp < PASS_SIZE))
      {
        if (IsButtonPressed(BUTTON1)) {
          machine.u16Entry[machine.u8pp] = 1;
          machine.u8pp++;
        }
        else if (IsButtonPressed(BUTTON2)) {
          machine.u16Entry[machine.u8pp] = 2;
          machine.u8pp++;
        }
        else if (IsButtonPressed(BUTTON3)) {
          machine.u16Entry[machine.u8pp] = 3;
          machine.u8ep++;
        } 
      }
      
      // Ensure locked state
      LedOff(GREEN);
      LedBlink(RED, 0);
      machine.u8State = LOCKED; 
    }
    
    // Password testing
    if (machine.u8State == LOCKED)
    {
      // Take input
      while (!(IsButtonPressed(BUTTON3)) || (machine.u8ep < PASS_SIZE))
      {
        if (IsButtonPressed(BUTTON1)) {
          machine.u16Entry[machine.u8ep] = 1;
          machine.u8ep++;
        }
        else if (IsButtonPressed(BUTTON2)) {
          machine.u16Entry[machine.u8ep] = 2;
          machine.u8ep++;
        }
        else if (IsButtonPressed(BUTTON3)) {
          machine.u16Entry[machine.u8ep] = 3;
          machine.u8ep++;
        } 
      }
      
      //Check if the password entered was correct
      //First check if the indexers are the same value.
      //Same value == same password length
      if (machine.u8ep == machine.u8pp)
      {
        u8 u8isMatch = 1;
        for (size_t i = machine.u8pp; i > 0; i--)
        {
          if (machine.u16Pass[i] != machine.u16Entry[i]) {
            u8isMatch = 0;
            break;
          }
        }
        if (u8isMatch == 1) {
          // Correct
          machine.u8State = UNLOCKED;
          LedOn(GREEN);
          LedBlink(GREEN, LED_2HZ);
        }
        else {
          // Wrong
          machine.u8State = LOCKED;
          LedOn(RED);
          LedBlink(RED, LED_2HZ);
        }
      }
      
      // Wait for Led response
      while (1)
      {
        if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) ||
            IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3))
          break;
      }
      LedOff(GREEN);
      LedBlink(RED, 0);
           
    }
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */

// Sets initial password machine
void initMach(mach *machine)
{
  // Sets inital lED configs
  LedOff(RED);
  LedOn(YELLOW);
  
  // Sets default password as 1-2-3-0-0-0-0-0-0-0
  for (size_t i; i < PASS_SIZE; i++)
  {
    if (i <= 3)
      machine->u16Pass[i] = i + 1;
    else
      machine->u16Pass[i] = 0;
  }
  
  // Sets entry as zero for password input
  for (size_t i; i < PASS_SIZE; i++)
  {
    machine->u16Entry[i] = 0;
  }
  
  // Allow the user to initially modify password, it will be 
  // locked after 3 seconds
  machine->u8State = MODIFY;
  machine->u8confSet = 1;
  machine->u8ep = 0;
  machine->u8pp = 0;
}
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
