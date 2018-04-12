using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Input
{    
    /// <summary>
    /// Any entity implementing IControllable interface will received
    /// events of KeyDown, KeyUp, and KeyPressed
    /// Events are added and removed from the handler on player controller
    /// </summary>
    interface IControllable
    {
        void OnKeyDown(object source, InputArgs args);
        void OnKeyUp(object source, InputArgs args);
        void OnKeyPressed(object source, InputArgs args);
    }
}
