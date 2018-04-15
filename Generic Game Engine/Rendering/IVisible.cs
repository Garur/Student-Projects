using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Rendering
{
    public interface IVisible
    {
        Texture2D Texture { get; set; }
        void SetVisibility(bool isVisible);
        void SetColor(Color _color);
        void Draw(SpriteBatch spriteBatch);
    }
}
