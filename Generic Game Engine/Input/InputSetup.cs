using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using System.Collections.Generic;

namespace GameEngine.Input
{
    /// <summary>
    /// Responsible for defining and setting up the controls for the game
    /// for all the players
    /// </summary>
    class InputSetup
    {
        /// <summary>
        /// Setup the inputs. Gets called on Game1.Initialize()
        /// </summary>
        public static void SetupInputs()
        {
            //Initialize Input Dicitionries for all the players
            InputList.PlayerOneInput = new Dictionary<string, Keys>();
            InputList.PlayerTwoInput = new Dictionary<string, Keys>();
            InputList.PlayerThreeInput = new Dictionary<string, Keys>();
            InputList.PlayerFourInput = new Dictionary<string, Keys>();            
            //Setup Player One Input
            InputList.PlayerOneInput.Add("moveup", Keys.W);
            InputList.PlayerOneInput.Add("movedown", Keys.S);
            InputList.PlayerOneInput.Add("moveleft", Keys.A);
            InputList.PlayerOneInput.Add("moveright", Keys.D);
            //Setup Player Two Input
            InputList.PlayerTwoInput.Add("moveup", Keys.Up);
            InputList.PlayerTwoInput.Add("movedown", Keys.Down);
            InputList.PlayerTwoInput.Add("moveleft", Keys.Left);
            InputList.PlayerTwoInput.Add("moveright", Keys.Right);

            //Setup Player Three Input

            //Setup Player Four Input
        }

    }
}
