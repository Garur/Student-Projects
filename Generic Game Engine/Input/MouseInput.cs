using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Input
{
    /// <summary>
    /// Responsible to track the state of mouse input
    /// Provides information when muose buttons have been pressed, 
    /// realased, are being pressed, mouse position    /// 
    /// </summary>
    public class MouseInput
    {
        static MouseState newState;
        static MouseState oldState;

        //True means that the state has been checked on this frame
        //Mouse state can only be updated once per frame, this variable makes sure of that       
        static bool stateFlag;

        //Reset the state flag -> meaning that the state can be updated this frame
        public void Update()
        {
            stateFlag = false;
        }

        /// <summary>
        /// Returns the current position of the mouse
        /// </summary>
        /// <returns></returns>
        public static Vector2 GetMousePosition()
        {
            newState = Mouse.GetState();
            return new Vector2(newState.X, newState.Y);
        }

        /// <summary>
        /// Toggle the mouse cursor on and off
        /// if IsMouseVisible is true, sets it to false, hidding the mouse cursor and vice versa
        /// </summary>
        public static void ToggleMouse()
        {
            if (Game1.Instance.IsMouseVisible)
            {
                Game1.Instance.IsMouseVisible = false;
            }else
            {
                Game1.Instance.IsMouseVisible = true;
            }
        }

        /// <summary>
        /// States if the right button has been pressed on current frame
        /// </summary>
        /// <returns>True if the right button just got pressed</returns>
        public static bool IsRightButtonDown()
        {
            CheckState();
            if (newState.RightButton == ButtonState.Pressed)
                if (oldState.RightButton == ButtonState.Released)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            else
                return false;
        }

        /// <summary>
        /// States if the right button stopped being pressed in the last frame
        /// </summary>
        /// <returns>True if the button stopped being pressed</returns>
        public static bool IsRightButtonUp()
        {
            CheckState();
            if (newState.RightButton == ButtonState.Released)
            {
                if (oldState.RightButton == ButtonState.Pressed)
                {
                    return true;
                }else
                {
                    return false;
                }                    
            }
            else
            {
                return false;
            } 
        }

        /// <summary>
        /// States if the right button is currently being pressed.
        /// </summary>
        /// <returns>True if button was already being pressed in the previous frame and is still being pressed </returns>
        public static bool IsRightButtonPressed()
        {
            CheckState();
            if (newState.RightButton == ButtonState.Pressed)
            {
                if (oldState.RightButton == ButtonState.Pressed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// States if the left button has been pressed on current frame
        /// </summary>
        /// <returns>True if the left button just got pressed</returns>
        public static bool IsLeftButtonDown()
        {
            CheckState();
            if (newState.LeftButton == ButtonState.Pressed)
            {
                if (oldState.LeftButton == ButtonState.Released)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }            
            return false;
        }

        /// <summary>
        /// States if the left button has been released in current frame
        /// </summary>
        /// <returns>True if the left button just got released</returns>
        public static bool IsLeftButtonUp()
        {
            CheckState();
            if (newState.LeftButton == ButtonState.Released)
            {
                if (oldState.LeftButton == ButtonState.Pressed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
                return false;
        }

        /// <summary>
        /// States if the left button is currently being pressed.
        /// </summary>
        /// <returns>True if button was already being pressed in the previous frame and is still being pressed </returns>
        public static bool IsLeftButtonPressed()
        {
            CheckState();
            if (newState.LeftButton == ButtonState.Pressed)
            {
                if (oldState.LeftButton == ButtonState.Pressed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
                return false;
        }

        /// <summary>
        /// States if the middle button has been pressed in current frame
        /// </summary>
        /// <returns>True if the middle button just got pressed</returns>
        public static bool IsMiddleButtonDown()
        {
            CheckState();
            if (newState.MiddleButton == ButtonState.Pressed)
            {
                if (oldState.MiddleButton == ButtonState.Released)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
                return false;
        }

        /// <summary>
        /// States if the middle button has been released in current frame
        /// </summary>
        /// <returns>True if the middle button just got released</returns>
        public static bool IsMiddleButtonUp()
        {
            CheckState();
            if (newState.MiddleButton == ButtonState.Released)
            {
                if (oldState.MiddleButton == ButtonState.Pressed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
                return false;
        }

        /// <summary>
        /// States if the middle button is currently being pressed.
        /// </summary>
        /// <returns>True if button was already being pressed in the previous frame and is still being pressed </returns>
        public static bool IsMiddleButtonPressed()
        {
            CheckState();
            if (newState.MiddleButton == ButtonState.Pressed)
            {
                if (oldState.MiddleButton == ButtonState.Pressed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
                return false;
        }

        /// <summary>
        /// Makes sure the state of the mouse is not updated more than once per frame
        /// The stateFlag gets reseted in the start of the frame and is only reseted again in the next
        /// </summary>
        static void CheckState()
        {
            if (!stateFlag)
            {
                oldState = newState;
                newState = Mouse.GetState();
                stateFlag = true;
            }
        }
    }
}
