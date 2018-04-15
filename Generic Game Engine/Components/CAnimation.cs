using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Game_Engine_1.Entity;
using Microsoft.Xna.Framework;
using Game_Engine_1.AI.AIComponents;
using Microsoft.Xna.Framework.Graphics;

namespace Game_Engine_1.Components
{
    class CAnimation : IComponent
    {
        IEntity _owner;
        AnimationStateMachine ASMachine;

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

        public void Initialize(IEntity ownerEntity)
        {
            ASMachine = new AnimationStateMachine();
            ASMachine.Initialize(_owner);
        }
        /// <summary>
        /// Loads an animation based on the name of the texture previouly loads
        /// in the resource manager. If the file is a 
        /// </summary>
        /// <param name=""></param>
        public void LoadSpriteSheetAnimation(string textureName)
        {

        }
        /// <summary>
        /// Loads an animation based on the name of the name of the texture files
        /// The texture files need to be loaded previously
        /// The names of the animation frame files should begin with the same string 
        /// and add the number of the frame separated with an underscore
        /// </summary>
        /// <param name="textureName"></param>
        public void LoadAnimation(string textureName)
        {
            IAnimationState animation = ASMachine.LoadState<SpriteAnimation>(textureName);
            Texture2D frame = _owner.GetResources().GetTexture(textureName);
            if(frame == null)
            {
                frame = _owner.GetResources().GetTexture(textureName + "_0");
            }

            List<Texture2D> frames = new List<Texture2D>();
            int i = 1;
            while(frame != null)
            {
                frames.Add(frame);
                frame = _owner.GetResources().GetTexture(textureName + "_" + i);
                i++;
            }

            if (frames.Count == 0)
            {
                Console.WriteLine("Not frames were found with name: " + textureName);
            }
            animation.LoadFrames(frames.ToArray());
        }


        public void Update(GameTime gametime)
        {
            
        }



    }
}
