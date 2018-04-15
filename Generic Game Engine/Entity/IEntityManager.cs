using GameEngine.Components;
using GameEngine.Scene;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Entities
{
    public interface IEntityManager : IProvidable
    {
        void Initialize();
        void Update(GameTime gameTime);
        T RequestEntity<T>() where T : IEntity, new();
        void Terminate(IEntity entity);
        IEntity GetEntity(string UNAME);
        IEntity GetEntity(int UID);        
    }
}
