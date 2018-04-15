using GameEngine.Entities;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Components
{
    /// <summary>
    /// Interface for all the Components
    /// Components should be initialized in entity derived classes, 
    /// initialized and updated in the entity methods, so they can perform
    /// their expected behaviour
    /// </summary>
    public interface IComponent
    {
        IEntity Owner { get; set; }
        void Initialize(IEntity ownerEntity);
        void Update(GameTime gametime);
    }
}
