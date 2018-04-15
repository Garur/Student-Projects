using GameEngine;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Rendering
{
    interface IRenderer : IProvidable
    {
        void RegisterDrawable(IVisible drawable);
        void UnregisterDrawble(IVisible drawable);
        void Initialize();
        void Draw(SpriteBatch spriteBatch);
    }
}
