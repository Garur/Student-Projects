using Microsoft.Xna.Framework.Input;
using System.Collections.Generic;

namespace GameEngine.Input
{
    /// <summary>
    /// Saves the Input keys specific for all the players
    /// The string will identify the key code (can represent the use of the key)
    /// </summary>
    static class InputList
    {
        public static Dictionary<string, Keys> PlayerOneInput;
        public static Dictionary<string, Keys> PlayerTwoInput;
        public static Dictionary<string, Keys> PlayerThreeInput;
        public static Dictionary<string, Keys> PlayerFourInput;
    }
}
