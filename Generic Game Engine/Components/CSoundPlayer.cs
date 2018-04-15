using System;
using GameEngine.Entities;
using GameEngine.Resources;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;

namespace GameEngine.Components
{
    /// <summary>
    /// Used to play sound effects
    /// </summary>
    class CSoundPlayer : IComponent
    {
        //Reference to the owner entity of this component
        IEntity _owner;
        //Reference to the resorces manager to get the sounds objects 
        IResourceManager resources;
        SoundEffect clip;

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
            Owner = ownerEntity;
            resources = ServiceLocator.Instance.GetService<IResourceManager>();
        }

        public virtual void Update(GameTime gametime) { }


        /// <summary>
        /// Plays the current clip
        /// </summary>
        public void Play()
        {            
            clip.Play();
        }

        /// <summary>
        /// Loads a different sound for and stores reference in the clip variable
        /// Sounds have to be previously loaded in the resource manager through the game mode load function
        /// </summary>
        /// <param name="clipName">Name of the sound in the files to set</param>
        public void SetClip(string clipName)
        {
            clip =  resources.GetSound(clipName);

        }

        
    }
}
