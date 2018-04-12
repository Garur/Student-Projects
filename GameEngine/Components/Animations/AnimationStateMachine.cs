using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Entities;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Components
{
    /// <summary>
    /// State machine for animations. 
    /// Stores a list of animations and performs the necessary logic 
    /// to transition from one state to the other
    /// </summary>
    public class AnimationStateMachine
    {
        IEntity _owner;
        //States Dictionary
        IDictionary<string, IAnimationState> statesList;
        //Current active nimation state
        public IAnimationState currentState;

        public IEntity Owner
        {
            get
            {
                return _owner;
            }

            set
            {
                _owner = value;
            }
        }

        public void Initialize(IEntity ownerEntity)
        {
            ownerEntity = _owner;
            statesList = new Dictionary<string, IAnimationState>();
        }

        /// <summary>
        /// Calls the update function for the current state, running the animation of the current state
        /// </summary>
        /// <param name="gametime"></param>
        public void Update(GameTime gametime)
        {
            if (currentState != null)
            {
                currentState.Update(gametime);
            }
        }

        /// <summary>
        /// Loads a state based on the T specified
        /// Inserts it on the State Dictionary and 
        /// </summary>
        /// <typeparam name="T">Type of the state to be loaded</typeparam>
        /// <param name="stateName">Name of the state to be loaded and that will identify the instance</param>
        /// <returns>Returns the reference for animation state loaded</returns>
        public T LoadState<T>(string stateName) where T : IAnimationState, new()
        {
            T s;
            Type type = typeof(T);

            //Checks if the state has already been loaded
            if (statesList.ContainsKey(stateName))
            {                
                return s = (T)statesList[stateName];
            }
            else
            {
                s = new T();
            }
            statesList.Add(stateName, s);
            s.Initialize(stateName, this);            
            return s;
        }
        

        /// <summary>
        /// Performs the transition between current state and new state
        /// </summary>
        /// <param name="stateName">Name of the new state to transition to </param>
        public void TransitionState(string stateName)
        {
            IAnimationState animationState;
            //Checks if Dict contains the state specified
            if (statesList.ContainsKey(stateName))
            {
                animationState = statesList[stateName];                
            }
            else
            {
                Console.WriteLine(stateName + " : Failed to activated because it was not loaded");
                return;
            }

            //Checks if the state is already loaded
            if (animationState == currentState)
            {
                return;
            }
            //Performs onExit if there is a current state already
            if (currentState != null)
            {
                currentState.OnExit();
            }
            currentState = animationState;
            animationState.OnEnter();
        }

        /// <summary>
        /// Checks if an animation state is currently active
        /// </summary>
        /// <param name="state">Name of the state to check</param>
        /// <returns>Returns true is state specified is active</returns>
        public bool IsStateActive(string state)
        {
            if (currentState.Name == state)
            {
                return true;
            }
            else
            {
                return false;
            }
        } 
    }
}

