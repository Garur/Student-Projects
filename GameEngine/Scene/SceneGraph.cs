using GameEngine.Entities;
using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;

namespace GameEngine.Scene
{
    class SceneGraph
    {
        IList<int> entityList;
        
        //entity ids
        public SceneGraph()
        {
            entityList = new List<int>();
        }

        /// <summary>
        /// Spawns an entity in the game world and set he entity active
        /// </summary>
        /// <param name="entity">Entity to spawn</param>
        /// <param name="position">Position where to spawn the entity</param>
        public void SpawnEntity(int entityID)
        {                       
            entityList.Add(entityID);          
        }

        /// <summary>
        /// Removes Entity from the Scene
        /// And sets the entity to inactive
        /// </summary>
        /// <param name="entityID">Entity to remove from scene</param>
        public void RemoveEntity(int entityID)
        {            
            if (entityList.Contains(entityID))
            {
                entityList.Remove(entityID);
            }           
        }
    }
}
