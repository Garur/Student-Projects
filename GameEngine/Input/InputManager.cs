using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using System;
using System.Linq;

namespace GameEngine.Input
{
    /// <summary>
    /// Publisher of Input events
    /// Register KeyDown, KeyUp and KeyPressed listenners
    /// Receives an array of keys that will be tracking
    /// And notifies the subscribers currently listening
    /// depending on the previousState and newState of the KeyBoard
    /// InputManager is created on the player controller 
    /// </summary>
    class InputManager
    {
        //Events
        public event EventHandler<InputArgs> KeyDown;
        public event EventHandler<InputArgs> KeyUp;
        public event EventHandler<InputArgs> KeyPressed;
                
        //Previous State Frame
        KeyboardState oldState;
        //Current frame state
        KeyboardState newState;
        //Array with keys that manager will be tracking
        Keys[] keysToTrack;

        public InputManager(Keys[] _keysToTrack)
        {
            keysToTrack = _keysToTrack;
            oldState = Keyboard.GetState();
        }

        //Register Listenners
        public void KeyDownListenner(EventHandler<InputArgs> handler) { KeyDown += handler; }
        public void KeyUpListenner(EventHandler<InputArgs> handler) { KeyUp += handler; }
        public void KeyPressedListenner(EventHandler<InputArgs> handler) { KeyPressed += handler;}
        //Unregister Listenners
        public void RemoveKeyUP(EventHandler<InputArgs> handler) { KeyUp -= handler; }
        public void RemoveKeyDown(EventHandler<InputArgs> handler){ KeyDown -= handler; }
        public void RemoveKeyPressed(EventHandler<InputArgs> handler) { KeyPressed -= handler; }
        //Event calls
        protected virtual void OnKeyDown(InputArgs data) { KeyDown(this, data); }
        protected virtual void OnKeyUp(InputArgs data) { KeyUp(this, data); }
        protected virtual void OnKeyPressed(InputArgs data) { KeyPressed(this, data); }

        /// <summary>
        /// Checks for input changes, runs on the playerController that
        /// has instanciated this manager
        /// </summary>
        public void Update()
        {    
            //Updates this frames state
            newState = Keyboard.GetState();           

            // look for changes in input data for every key of array
            for (int i = 0; i < keysToTrack.Length; i++)
            {
                if (newState.IsKeyDown(keysToTrack[i]))
                {
                    // If not down last update, key has just been pressed.
                    if (!oldState.IsKeyDown(keysToTrack[i]))
                    {
                        InputArgs data = new InputArgs(keysToTrack[i]);                        
                        if (KeyDown != null)
                            OnKeyDown(data);
                    }
                    else
                    {                     
                        //Key was pressed on last frame and still is
                        InputArgs data = new InputArgs(keysToTrack[i]);
                        if (KeyPressed != null)
                            OnKeyPressed(data);
                    }
                }
                else if (oldState.IsKeyDown(keysToTrack[i]))
                {
                    //Key was released
                    InputArgs data = new InputArgs(keysToTrack[i]);                    
                    if (KeyUp != null)
                        OnKeyUp(data);
                }
            }
            //Saves state for next frame
            oldState = newState;
        }  
    }
}
