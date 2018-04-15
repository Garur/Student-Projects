using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;

namespace GameEngine.Rendering
{
    public class Renderer : IRenderer
    {
        static bool _instantiated = false;
        IList<IVisible> visibles;

        public Renderer()
        {
            if (_instantiated)
                throw new InvalidOperationException("Renderer is already instanciated");

            _instantiated = true;
        }

        public void Initialize()
        {
            visibles = new List<IVisible>();
        }

        /// <summary>
        /// Adds a IVisible to the Drawbles list
        /// </summary>
        /// <param name="visible">reference to the objet to be added to the list</param>
        public void RegisterDrawable(IVisible visible)
        {
            visibles.Add(visible);
        }

        /// <summary>
        /// Removes the IVisible from Drawables list
        /// </summary>
        /// <param name="visible">reference to the objet to be removed from the list</param>
        public void UnregisterDrawble(IVisible visible)
        {
            visibles.Remove(visible);
        }

        /// <summary>
        /// Draws every Ivisibles registered
        /// Gets called in the main Game class
        /// </summary>
        /// <param name="spriteBatch">SpritBatch reference necessary to draw objects</param>
        public void Draw(SpriteBatch spriteBatch)
        {
            for( int i = 0; i < visibles.Count; i++)
            {
                visibles[i].Draw(spriteBatch);
            }
        }


    }
}
