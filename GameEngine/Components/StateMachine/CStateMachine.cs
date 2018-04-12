using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using GameEngine.Entities;
using GameEngine.Components;

/// <summary>
/// This class was based on a class previouly 
/// created by me in previous modules and adapted to
/// fit current requirements 
/// </summary>
namespace GameEngine.Components
{
    /// <summary>
    /// Manages the states. Works has a subcomponent of a entity.
    /// Loads states similar to factory method, keeps a dictionary with all the states
    /// that can only have one of each type. Performs the transition between the states.
    /// Keeps a reference to the owner entity to pass it to the states
    /// </summary>
    class CStateMachine : IComponent
    {
        //Dictionary of loaded states
        Dictionary<Type, IState> statesList;
        //Current active state rference and globalState reference
        IState currentState, globalState; 
        public IEntity Owner { get; set; }

        public void Initialize(IEntity ownerEntity)
        {
            Owner = ownerEntity;
            statesList = new Dictionary<Type, IState>();
        }

        /// <summary>
        /// Calls the update function for the current state and the global state
        /// Global state update is called first. If a trasintion happens in the global state 
        /// the current state will be the new one already on the current tick
        /// </summary>
        /// <param name="gametime"></param>
        public void Update(GameTime gametime)
        {
            if (globalState != null)
            {
                globalState.Update(gametime);
            }
            if (currentState != null)
            {
                currentState.Update(gametime);
            }
        }
        
        /// <summary>
        /// Loads a state based on the T specified
        /// Inserts it on the State Dictionary and initialize it
        /// </summary>
        /// <typeparam name="T">Type of the state to be loaded</typeparam>
        /// <returns>Returns the reference for animation state loaded</returns>
        public T LoadState<T>() where T : IState, new()
        {
            T s;
            Type type = typeof(T);

            //Checks if the state has already been loaded
            if (statesList.ContainsKey(type))
            {                
                return s = (T)statesList[type];
            }
            else
            {
                s = new T();
            }
            statesList.Add(type, s);
            s.Owner = Owner;
            s.SMachine = this;
            s.Initialize();
            return s;
        }

        /// <summary>
        /// Performs the transition between current state and new state
        /// </summary>
        /// <param name="stateName">Name of the new state to transition to </param>
        public void SetState<T>() where T : IState
        {
            Type stateType = typeof(T);
            IState s;

            //Checks if the state is loaded
            if (statesList.ContainsKey(stateType))
            {
                s = statesList[stateType];
            }
            else
            {
                Console.WriteLine(stateType.ToString() 
                    + " : Failed to activated because it was not loaded");
                return;
            }
            //Checks if the state to activate isn't already active
            if (s == currentState)
            {
                return;
            }
            //Performs the OnExit method on the previous state
            if (currentState != null)
            {
                currentState.OnExit();
            }
            currentState = s;            
            s.OnEnter();
        }

        /// <summary>
        /// Sets a new global state 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        public void SetGlobalState<T>() where T : IState
        {
            Type stateType = typeof(T);
            IState s;

            //Checks if the state has been loaded
            if (statesList.ContainsKey(stateType))
            {
                s = statesList[stateType];
            }
            else
            {
                Console.WriteLine(stateType.ToString()
                    + " : Failed to activated global because it was not loaded");
                return;
            }
            //Checks if the global state isn't already loaded
            if (s == globalState)
            {
                return;
            }
            //Call the OnExit method on the previous state
            if (globalState != null)
            {
                globalState.OnExit();
            }
            globalState = s;
            s.OnEnter();
        }

        /// <summary>
        /// Returns the current active state
        /// </summary>
        public IState GetCurrentState()
        {
            return currentState;
        }

        /// <summary>
        /// Checks if the state of type T is the currently active state in the state machine
        /// Returns true if yes
        /// </summary>
        public bool IsStateActive<T>()
        {
            if(currentState.GetType() == typeof(T))
                return true;
            else
                return false;
        }
    }
}
