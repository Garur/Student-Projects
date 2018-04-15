using GameEngine.Resources;
using Microsoft.Xna.Framework;

namespace GameEngine
{
    interface IGameMode
    {
        //Create entities
        void Initialize();
        void Update(GameTime gametime);
        //Loads all the necessary resources for the game
        void LoadResources(IResourceManager resourceManager);
    }
}
