using Microsoft.Xna.Framework.Input;
using System;

namespace GameEngine.Input
{
    /// <summary>
    /// Event Arguments passed on the InputManager handler
    /// </summary>
    public class InputArgs : EventArgs
    {
        //Key that was registered in the event
        public Keys key;
        public InputArgs(Keys _key)
        {
            key = _key;
        }
    }
    
}
