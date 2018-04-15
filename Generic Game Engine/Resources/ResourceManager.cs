using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;

namespace GameEngine.Resources
{
    /// <summary>
    /// Responsible to load and manage all the resources needed for the game
    /// Manages Audio and Image Files, loads them from the Content folder
    /// </summary>
    public class ResourceManager : IResourceManager
    {
        //Limits the class to a single intance
        static bool _instantiated = false;
        //Contains the reference to the textures used in game
        IDictionary<string, Texture2D> textures;
        //Contains the reference to the sound effects used in game
        IDictionary<string, SoundEffect> soundClips;
        //Reference to the content instance used to get the files from the content folder
        ContentManager content;

        public ResourceManager()
        {
            //Only one instance of Scene Manager is allowed.
            if (_instantiated)
                throw new InvalidOperationException("Resource Manager is already instanciated");

            _instantiated = true;
            textures = new Dictionary<string, Texture2D>();
            soundClips = new Dictionary<string, SoundEffect>();
        }

        ~ResourceManager()
        {
            _instantiated = false;
        }
        /// <summary>
        /// Initializes the necessary logic for the Manager to run
        /// </summary>
        /// <param name="_content">Reference to the content object class where the files are loaded from</param>
        public void Initialize(ContentManager _content)
        {
            content = _content;
        }

        /// <summary>
        /// Loads a image file from the content folder
        /// </summary>
        /// <param name="textureName">Name of the texture file to be loaded</param>
        public Texture2D LoadTexture(string textureName)
        {
            //Console.WriteLine(textureName);
            Texture2D texture = content.Load<Texture2D>(textureName);
            textures.Add(textureName, texture);
            return texture;
        }
        /// <summary>
        /// Loads a sound file fromthe content folder
        /// </summary>
        /// <param name="soundName">Name of the sound file to be loaded</param>
        public SoundEffect LoadSound(string soundName)
        {
            SoundEffect soundClip = content.Load<SoundEffect>(soundName);
            soundClips.Add(soundName, soundClip);
            return soundClip;
        }
        /// <summary>
        /// Searchs for a texture by name
        /// </summary>
        /// <param name="textureName">Name of the texture</param>
        /// <returns>Returns the </returns>
        public Texture2D GetTexture(string textureName)
        {
            Texture2D texture = null;
            textures.TryGetValue(textureName, out texture);
            if(texture == null)
            {
                //throw new NullReferenceException(textureName + " not found. Ensure that texture has been loaded");
            }
            return texture;
        }

        /// <summary>
        /// Searchs for a sound clip by name
        /// </summary>
        /// <param name="soundName">Name of the sound file</param>
        /// <returns>Returns the </returns>
        public SoundEffect GetSound(string soundName)
        {
            SoundEffect sound = soundClips[soundName];
            if (sound == null)
            {
                Console.WriteLine("Sound Clip File Not Found");
            }
            return sound;
        }

        /// <summary>
        /// Unloads all the game resources 
        /// </summary>
        public void UnloadAll()
        {
            textures.Clear();
            soundClips.Clear();
        }

    }
}
