
/**
 *   The purpose of this test is to trigger error conditions
 *   of the state machine and verify that they behave as expected.
 *   
 */

#if defined(_MSC_VER)
   //Warning about: identifier was truncated to '255' characters in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
// Warning about: constructor of the state machine receiving a pointer to this
// from a constructor. This is not a problem in this case, since the state
// machine constructor is not using the pointer, just storing it internally.
#pragma warning( disable : 4355 )
#endif

#include "itkLogger.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"

#include <iostream>


namespace igstk
{


class Tester1
{
public:

  typedef StateMachine< Tester1 >                         StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  typedef StateMachineType::StateType                     StateType;
  typedef StateMachineType::InputType                     InputType;
  typedef StateMachineType::StateIdentifierType           StateIdentifierType;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;
  
  igstkFriendClassMacro( StateMachine< Tester1 > );

  igstkTypeMacro( Tester1, None );

  Tester1():m_StateMachine(this)
    {
    // Set the state descriptors
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.AddState( m_OneQuarterCredit, "OneQuarterCredit" );
    m_StateMachine.AddState( m_TwoQuarterCredit, "TwoQuarterCredit" );
    m_StateMachine.AddState( m_ThreeQuarterCredit, "ThreeQuarterCredit" );

    // Set the input descriptors
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );

    const ActionType NoAction = 0;

    // Programming the machine
    m_StateMachine.AddTransition( m_IdleState,          m_QuarterInserted, m_OneQuarterCredit,    NoAction );
    m_StateMachine.AddTransition( m_OneQuarterCredit,   m_QuarterInserted, m_TwoQuarterCredit,    NoAction );
    m_StateMachine.AddTransition( m_TwoQuarterCredit,   m_QuarterInserted, m_ThreeQuarterCredit,  NoAction );

    m_StateMachine.SelectInitialState( m_IdleState );

    // Finish the programming and get ready to run
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester1() {}

  void triggerError1()
  {
    // NOTE that on purpose the m_ThisStateDoesntExist was not added as a state
    // with AddState() */
    m_StateMachine.SelectInitialState( m_ThisStateDoesntExist );
  }

  /** Declarations needed for the Logging */
  igstkLoggerMacro();

private:

  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;
  StateType m_OneQuarterCredit;
  StateType m_TwoQuarterCredit;
  StateType m_ThreeQuarterCredit;
  StateType m_ThisStateDoesntExist;
  
  /** List of Inputs */
  InputType m_QuarterInserted;

};





class Tester2
{
public:

  typedef StateMachine< Tester2 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;
  
  igstkFriendClassMacro( StateMachine< Tester2 > );

  igstkTypeMacro( Tester2, None );

  Tester2():m_StateMachine(this)
    {
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester2() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    // On purpose we didn't set m_QuarterInserted with AddInput();
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
 
  /** Declarations needed for the Logging */
  igstkLoggerMacro();

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};




class Tester3
{
public:

  typedef StateMachine< Tester3 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;
  
  igstkFriendClassMacro( StateMachine< Tester3 > );

  igstkTypeMacro( Tester3, None );

  Tester3():m_StateMachine(this)
    {
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );
    // On purpose NOT calling : m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester3() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
 
  /** Declarations needed for the Logging */
  igstkLoggerMacro();

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};



class Tester4
{
public:

  typedef StateMachine< Tester4 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;
  
  igstkFriendClassMacro( StateMachine< Tester4 > );

  igstkTypeMacro( Tester4, None );

  Tester4():m_StateMachine(this)
    {
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );

    // On purpose invode SetReadyToRun() twice.
    m_StateMachine.SetReadyToRun();
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester4() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
 
  /** Declarations needed for the Logging */
  igstkLoggerMacro();

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};


class Tester5
{
public:

  typedef StateMachine< Tester5 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer        ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;
  
  igstkFriendClassMacro( StateMachine< Tester5 > );

  igstkTypeMacro( Tester5, None );
  
  Tester5():m_StateMachine(this)
    {
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );

    // On purpose NOT adding any transition: for testing error condition.
    // const ActionType NoAction = 0;
    // m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester5() {}

  void InvokeUndefinedTransition() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
 
  /** Declarations needed for the Logging */
  igstkLoggerMacro();

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};


class Tester6
{
public:

  typedef StateMachine< Tester6 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;
  
  igstkFriendClassMacro( StateMachine< Tester6 > );

  igstkTypeMacro( Tester6, None );

  Tester6():m_StateMachine(this)
    {
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.AddState( m_ChangeMindState, "ChangeMindState" );
    m_StateMachine.SelectInitialState( m_IdleState );

    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    m_StateMachine.AddInput( m_Cancel, "Cancel");
    const ActionType NoAction = 0;
   
    std::cout << "TEST: On purpose NOT adding any transition for input m_Cancel" << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );

    std::cout << "TEST: On purpose Adding a Transition for a non existing state " << std::endl;
    m_StateMachine.AddTransition( m_NonRegisteredState, m_QuarterInserted, m_IdleState, NoAction );
    
    std::cout << "TEST: On purpose Adding a Transition for a non existing input " << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_NonRegisteredInput, m_IdleState, NoAction );

    std::cout << "TEST: On purpose Adding a Transition for a non existing new state " << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_NonRegisteredState, NoAction );

    std::cout << "TEST: On purpose Adding a Transition for a {State,Input} pair " << std::endl;
    std::cout << "      for which a transition ALREADY exists " << std::endl;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_ChangeMindState, NoAction );

    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester6() {}

  void InvokeUndefinedStateInputTransition() 
    {
    std::cout << "Invoking Cancel" << std::endl;
    m_StateMachine.PushInput( m_Cancel );
    m_StateMachine.ProcessInputs();
    }
 
  /** Declarations needed for the Logging */
  igstkLoggerMacro();

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;
  StateType m_NonRegisteredState;
  StateType m_ChangeMindState;

  /** List of Inputs */
  InputType m_QuarterInserted;
  InputType m_Cancel;
  InputType m_NonRegisteredInput;

};


class Tester7
{
public:

  typedef StateMachine< Tester7 >                     StateMachineType;
  typedef StateMachineType::TMemberFunctionPointer    ActionType;
  typedef StateMachineType::StateType                 StateType;
  typedef StateMachineType::InputType                 InputType;
  typedef StateMachineType::StateIdentifierType       StateIdentifierType;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;
  
  igstkFriendClassMacro( StateMachine< Tester7 > );

  igstkTypeMacro( Tester7, None );

  Tester7():m_StateMachine(this)
    {
    m_StateMachine.AddState( m_IdleState, "IdleState" );
    m_StateMachine.AddInput( m_QuarterInserted, "QuarterInserted" );
    const ActionType NoAction = 0;
    m_StateMachine.AddTransition( m_IdleState, m_QuarterInserted, m_IdleState, NoAction );
    // On purpose NOT invoking SelectInitialState() before calling SetReadyToRun()
    // COMMENTED OUT ON PURPOSE:  m_StateMachine.SelectInitialState( m_IdleState );
    m_StateMachine.SetReadyToRun();
    }

  virtual ~Tester7() {}

  void InsertChange() 
    {
    std::cout << "Insert Change" << std::endl;
    m_StateMachine.PushInput( m_QuarterInserted );
    m_StateMachine.ProcessInputs();
    }
 
  /** Declarations needed for the Logging */
  igstkLoggerMacro();

private:
  StateMachineType   m_StateMachine;

  /** List of States */
  StateType m_IdleState;

  /** List of Inputs */
  InputType m_QuarterInserted;

};

} // namespace igstk



int igstkStateMachineErrorsTest( int, char * [] )
{

  std::cout << "Construct the State Machine Tester" << std::endl;
  igstk::Tester1  tester1;

  std::cout << std::endl << std::endl;
  std::cout << "Trigger error conditions on purpose" << std::endl;

  // The following call are designed for testing the error conditions of the
  // state machine. 

  std::cout << "Invoking as Initial state a state that doesn't exist." << std::endl;
  tester1.triggerError1();

  std::cout << "Invoking SetReadyToRun() (in constructor) without parent class connected." << std::endl;
  igstk::Tester2 tester2;

  std::cout << "Invoking ProcessInputs() without parent class connected." << std::endl;
  tester2.InsertChange();

  std::cout << "Invoking ProcessInputs() without having called SetReadyToRun() ." << std::endl;
  igstk::Tester3 tester3;
  tester3.InsertChange();

  std::cout << "Invoking  SetReadyToRun() twice." << std::endl;
  igstk::Tester4 tester4;
  tester4.InsertChange();

  std::cout << "Invoking  ProcessInputs() in a state without transitions defined." << std::endl;
  igstk::Tester5 tester5;
  tester5.InvokeUndefinedTransition();

  std::cout << "Invoking  ProcessInputs() in a state,input pair without transitions defined." << std::endl;
  igstk::Tester6 tester6;
  tester6.InvokeUndefinedStateInputTransition();

  std::cout << "Invoking  SetReadyToRun() without having called SetInitialState()" << std::endl;
  igstk::Tester7 tester7;
  tester7.InsertChange();


  // The following call 
  return EXIT_SUCCESS;

}

