using GameEngine.Collisions;
using GameEngine.Entities;
using Microsoft.Xna.Framework;

namespace GameEngine.Scene
{
    public interface ISceneManager : IProvidable
    {
        void Initialize();
        void SpawnEntity(IEntity entity);
        void RemoveEntity(string entityName);
        void RemoveEntity(int entityID);
    }
}
