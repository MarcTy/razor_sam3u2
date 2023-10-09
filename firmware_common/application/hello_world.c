/*!*********************************************************************************************************************
@file hello_world.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

/led blinking light test/

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
- void Hello_WorldInitialize(void)
- void Hello_WorldRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>Hello_World"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32Hello_WorldFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Hello_World_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Hello_World_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 Hello_World_u32Timeout;                           /*!< @brief Timeout counter used across states */


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
@fn void Hello_WorldInitialize(void)


@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void Hello_WorldInitialize(void)
{
  HEARTBEAT_OFF();
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    Hello_World_pfStateMachine = Hello_WorldSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    Hello_World_pfStateMachine = Hello_WorldSM_Error;
  }

} /* end Hello_WorldInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void Hello_WorldRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void Hello_WorldRunActiveState(void)
{
  Hello_World_pfStateMachine();

} /* end Hello_WorldRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

static u16 pwr(u16 val, u16 power)
{
  u16 n = val;
  if (power == 0)
  {
    return 1;
  }
  for(u16 x = 0; x < power; x++)
  {
    n *= n;
  }
  return n;
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void Hello_WorldSM_Idle(void)
{
    static u16 u16Counter = 0;
    static u16 u16Iter = 0;
    
    u16 u16PWR = pwr(10, u16Iter);
    
    u16Counter += u16PWR;
    static bool IS_LED = FALSE; 
    
    if (u16Counter == U16_COUNTER_PERIOD_MS)
    {
      u16Counter = 0;
      u16Iter++;
      if (u16PWR == U16_COUNTER_PERIOD_MS)
      {
        u16Iter = 0;
      }
      if (IS_LED == FALSE)
      {
         HEARTBEAT_OFF();
         IS_LED = TRUE;
      }
      else
      {
        HEARTBEAT_ON();
        IS_LED = FALSE;
      }
    }
    
} /* end Hello_WorldSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void Hello_WorldSM_Error(void)          
{
  
} /* end Hello_WorldSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
