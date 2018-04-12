using GameEngine.Components;
using GameEngine.Resources;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;

namespace GameEngine.Entities
{
    public interface IEntity
    { 
        int UID { get; set; }
        string UNAME{ get; set; }
        Vector2 position { get; set; }
        Vector2 orientation { get; set; }
        bool isActive { get; set; }
        bool toBeTerminated { get; set; }
        void Initialize();
        void Update(GameTime gameTime);        
        void TerminateSelf();

        T GetComponent<T>() where T : IComponent;
        T AddComponent<T>() where T : IComponent, new();
        
    }
}
