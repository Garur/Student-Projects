using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Components
{

    public interface IAnimationFrames { }

   /// <summary>
   /// Animation Frames data of a sprite animation
   /// SpriteAnimationFrames objects are used to pass information an animation
   /// from animator to the animation state
   /// </summary>
    class SpriteAnimationFrames : IAnimationFrames
    {
        public Texture2D[] frames;
    }

    /// <summary>
    /// Animation Frames data of a spritesheet animation
    /// SpritesheetAnimationFrames objects are used to pass information an animation
    /// from animator to the animation state
    /// </summary>
    class  SpritesheetAnimationFrames : IAnimationFrames
    {
        public Rectangle[] drawAreas;
        public Texture2D spriteSheet;
        public Vector2 origin;  
    }

    /// <summary>
    /// Contains information about a single frame in an animation
    /// </summary>
    public class AnimationFrame
    {
        public Texture2D Texture { get; set; }
        public Rectangle DrawArea { get; set; }
        public Vector2 origin;
        

    }

}
