using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;

namespace GameEngine.Input
{
    /// <summary>
    /// Possesses an entity to be controlled by a specific player
    /// Instanciates a InputManager and registers the entity 
    /// to receive the events from the input related to specific player
    /// </summary>
    class PlayerControlller
    {        
        //Instance of the input manager that will publish the events
        InputManager inputManager;
        //Entity to be controlled by the player
        IControllable controllableEntity;       

        /// <summary>
        /// Initializes the input manager with the array of keys that 
        /// corresponds to the player with PlayerIndex received
        /// Registers the entity to listen to the events published
        /// by the inputManager
        /// </summary>
        /// <param name="playerIndex">Identifier of the player</param>
        /// <param name="_controllableEntity">reference to the entity that will be controlled</param>
        public PlayerControlller(PlayerIndex playerIndex, IControllable _controllableEntity)
        {
            Keys[] keys = GetKeys(playerIndex);

            inputManager = new InputManager(keys);
            controllableEntity = _controllableEntity;
            inputManager.KeyDownListenner(controllableEntity.OnKeyDown);
            inputManager.KeyUpListenner(controllableEntity.OnKeyUp);
            inputManager.KeyPressedListenner(controllableEntity.OnKeyPressed);
        }


        /// <summary>
        /// Takes control of another entity
        /// Unregisters the current Controllable entity
        /// And registers the new entity
        /// </summary>
        /// <param name="_controllableEntity">New entity to possess</param>
        public void Possess(IControllable _controllableEntity)
        {
            //Unregisters Current Entity
            inputManager.RemoveKeyDown(controllableEntity.OnKeyDown);
            inputManager.RemoveKeyUP(controllableEntity.OnKeyUp);
            inputManager.RemoveKeyPressed(controllableEntity.OnKeyPressed);
            //Registers new entity
            controllableEntity = _controllableEntity;
            inputManager.KeyDownListenner(controllableEntity.OnKeyDown);
            inputManager.KeyUpListenner(controllableEntity.OnKeyUp);
            inputManager.KeyPressedListenner(controllableEntity.OnKeyPressed);
        }

        /// <summary>
        /// Retreives the keys specific for a certain player
        /// </summary>
        /// <param name="playerIndex">Index of the player to retreive the keys</param>
        /// <returns></returns>
        Keys[] GetKeys(PlayerIndex playerIndex)
        {
            Keys[] keys;
            if (playerIndex == PlayerIndex.One)
            {
                keys = new Keys[InputList.PlayerOneInput.Count];
                InputList.PlayerOneInput.Values.CopyTo(keys, 0);
            }
            else if (playerIndex == PlayerIndex.Two)
            {
                keys = new Keys[InputList.PlayerTwoInput.Count];
                InputList.PlayerTwoInput.Values.CopyTo(keys, 0);
            }
            else if (playerIndex == PlayerIndex.Three)
            {
                keys = new Keys[InputList.PlayerThreeInput.Count];
                InputList.PlayerThreeInput.Values.CopyTo(keys, 0);
            }
            else
            {
                keys = new Keys[InputList.PlayerFourInput.Count];
                InputList.PlayerFourInput.Values.CopyTo(keys, 0);
            }
            return keys;
        }

        /// <summary>
        /// Updates the input manager to check for inputs
        /// </summary>
        public void Update()
        {
            inputManager.Update();
        }

        /// <summary>
        /// Unsubcribes the entity and nullifies the inputManager
        /// </summary>
        ~PlayerControlller()
        {
            inputManager.RemoveKeyDown(controllableEntity.OnKeyDown);
            inputManager.RemoveKeyUP(controllableEntity.OnKeyUp);
            inputManager.RemoveKeyPressed(controllableEntity.OnKeyPressed);
            inputManager = null;            
        }




    }
}
