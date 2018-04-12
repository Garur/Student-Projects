using GameEngine.Components;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Entities;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using GameEngine.Rendering;

namespace GameEngine.Components
{
    class CSpriteRenderer : IComponent, IVisible
    { 
        IEntity _owner;
        Texture2D texture;
        Color color = Color.AntiqueWhite;    
        Vector2 position;
        Vector2 origin;
        Rectangle drawArea;
        bool isVisible = true;

        public IEntity Owner
        {
            get
            {
                return _owner;
            }

            set
            {
                _owner = value;
            }
        }

        public Texture2D Texture
        {
            get
            {
                return texture;
            }

            set
            {
                texture = value;
                drawArea = new Rectangle(0, 0, texture.Width, texture.Height);
                origin = new Vector2(texture.Width / 2, texture.Height / 2);
            }
        }

        public void Initialize(IEntity ownerEntity)
        {
            _owner = ownerEntity;

        }

        public void Update(GameTime gametime)
        {
            
        }

        public void Draw(SpriteBatch spriteBatch)
        {
            position = Owner.position;
            if(texture == null)
            {
                throw new ArgumentNullException("Texture is null");
            }

            spriteBatch.Draw(texture, position, drawArea, color, 0, origin, 1.0f, SpriteEffects.None, 0f);
        }

        public void SetColor(Color _color)
        {
            color = _color;
        }

        public Rectangle GetDrawArea()
        {
            return drawArea;
        }

        public void SetVisibility(bool _isVisible)
        {
            isVisible = _isVisible;
        }
    }
}
