using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Resources
{
    interface IResourceManager : IProvidable
    {
        void Initialize(ContentManager _content);
        Texture2D LoadTexture(string textureName);
        SoundEffect LoadSound(string textureName);
        Texture2D GetTexture(string textureName);
        SoundEffect GetSound(string soundName);
        void UnloadAll();
    }
}
