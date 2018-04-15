using GameEngine.Entities;
using Microsoft.Xna.Framework;

namespace GameEngine.Components
{ 
    public interface IComponentMaster : IProvidable
    {
        void Initialize();
        void Update(GameTime gameTime);
        T RequestComponent<T>(IEntity owner) where T : IComponent, new();
        T GetComponent<T>(int entityID) where T : IComponent;
        void RemoveComponents(int entityID);
        void RemoveComponent(IComponent component);
    }
}
