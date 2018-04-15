using GameEngine.Entities;

namespace GameEngine.Collisions
{
    public interface ICollisionManager : IProvidable
    {        
        void Update();
        void RegisterListener(ICollisionListener listener);
        void UnregisterListener(ICollisionListener listener);
        void RegisterCollidable(ICollidable collidable);
        void UnregisterCollidable(ICollidable collidable);
    }
}
