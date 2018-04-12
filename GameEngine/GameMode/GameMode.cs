using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Resources;
using Microsoft.Xna.Framework;
using GameEngine.Scene;
using GameEngine.Entities;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine
{
    abstract class GameMode : IGameMode
    {
        protected ISceneManager sceneManager;
        protected IEntityManager entityManager;

        public virtual void Initialize()
        {
            sceneManager = ServiceLocator.Instance.GetService<ISceneManager>();
            entityManager = ServiceLocator.Instance.GetService<IEntityManager>();
        }

        public void SetBackground(Texture2D background)
        {
            Game1.Instance.SetBackground(background);
        }

        public abstract void LoadResources(IResourceManager resourceManager);
        public abstract void Update(GameTime gametime);
    }
}
