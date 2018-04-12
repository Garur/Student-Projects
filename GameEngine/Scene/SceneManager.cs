using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using System.Collections.Generic;
using GameEngine.Entities;
using GameEngine.Collisions;
using System;

namespace GameEngine.Scene
{
    /// <summary>
    /// Handle entities currently in the scene.
    /// Spawns entities and stores them in the scene graph
    /// Only one instance of this class is allowed
    /// </summary>
    public class SceneManager : ISceneManager
    {
        //Limits the class to a single intance
        static bool _instantiated = false;

        //Scene Graph reference containing all the entities on the scene
        SceneGraph sceneGraph;
        ICollisionManager collisionManager;
        IEntityManager entityManager;
        
        public SceneManager()
        {
            //Only one instance of Scene Manager is allowed.
            if (_instantiated)
                throw new InvalidOperationException("Scene Manager is already instanciated");

            _instantiated = true;
            sceneGraph = new SceneGraph(); 
        }

        ~SceneManager()
        {
            _instantiated = false;
        }
        /// <summary>
        /// Passes other managers as variables in order for 
        /// scene manager to be able to operate
        /// </summary>
        public void Initialize()
        {
            collisionManager = ServiceLocator.Instance.GetService<ICollisionManager>();
            entityManager = ServiceLocator.Instance.GetService<IEntityManager>();
        }
      
        /// <summary>
        /// Calls the scene graph to spawn an entity
        /// Initializes the entity
        /// </summary>
        /// <param name="entity">Entity to be spawned</param>
        /// <param name="position">Position to spawn the entity</param>
        public void SpawnEntity(IEntity entity)
        {
            
            entity.isActive = true;
            sceneGraph.SpawnEntity(entity.UID);            
            //If entity is collidable subscribe entity to collision events          
            ICollisionListener listener = entity as ICollisionListener;
            if (listener != null)
            {
                collisionManager.RegisterListener(listener);
            }            
        }    

        /// <summary>
        /// Removes an entity from the scene
        /// Based on its name
        /// </summary>
        /// <param name="entityName">Entity Name</param>
        public void RemoveEntity(string entityName)
        {
            IEntity entity = entityManager.GetEntity(entityName);
            entity.isActive = false;
            //If entity is collidable unsubscribe entity to collision events          
            ICollisionListener listener = entity as ICollisionListener;
            if (listener != null)
            {
                collisionManager.UnregisterListener(listener);
            }
            sceneGraph.RemoveEntity(entity.UID);
        }

        /// <summary>
        /// Removes an entity from the scene graph
        /// Based on its UID
        /// </summary>
        /// <param name="entityID">Entity Unique ID</param>
        public void RemoveEntity(int entityID)
        {            
            IEntity entity = entityManager.GetEntity(entityID);
            entity.isActive = false;
            //If entity is collidable unsubscribe entity to collision events          
            ICollisionListener listener = entity as ICollisionListener;
            if (listener != null)
            {
                collisionManager.UnregisterListener(listener);
            }
            sceneGraph.RemoveEntity(entityID);
        }        
    }
}