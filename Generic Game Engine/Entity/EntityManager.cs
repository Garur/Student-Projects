using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using GameEngine.Scene;
using GameEngine.Components;

namespace GameEngine.Entities
{

    /// <summary>
    /// Responsible to keep track of all instanciated entities
    /// Creates entities through the entity factory
    /// Spawns the entities by default through the scene manager
    /// Only one instance of this class is allowed
    /// </summary>
    public class EntityManager : IEntityManager
    {
        //Limits the class to a single intance
        static bool _instantiated = false;

        //List containing all the entities created
        IList<IEntity> entityList;
        EntityFactory entityFactory;
        ISceneManager _sceneManager;
        IComponentMaster _componentManager;

        public EntityManager()
        {
            //Only one instance of Scene Manager is allowed.
            if (_instantiated)
                throw new InvalidOperationException("Entity Manager is already instanciated");

            _instantiated = true;
            
            entityList = new List<IEntity>();
            entityFactory = new EntityFactory();
        }

        ~EntityManager()
        {
            _instantiated = false;
        }

        public void Initialize()
        {
            _sceneManager = ServiceLocator.Instance.GetService<ISceneManager>();
            _componentManager = ServiceLocator.Instance.GetService<IComponentMaster>();
        }

        /// <summary>
        /// Calls the update method of all the entities currently spawned
        /// </summary>
        /// <param name="gameTime">gameTime variable received from Kernell</param>
        public void Update(GameTime gameTime)
        {
            for (int i = 0; i < entityList.Count; i++)
            {                
                //Only updates spawned entities
                if (entityList[i].isActive)
                {                    
                    entityList[i].Update(gameTime);
                }//Check if entities are to be terminated
                else if (entityList[i].toBeTerminated)
                {
                    Terminate(entityList[i]);
                }
            }
        }

        /// <summary>
        /// Rquests an Entity from the entity factory and adds it to the list of entities 
        /// Also spawns the entity by default through the scene manager 
        /// </summary>
        /// <typeparam name="T"> Type of the entity created</typeparam>
        /// <returns>The entity created</returns>
        public T RequestEntity<T>() where T : IEntity, new()
        {
            T e = entityFactory.CreateEntity<T>();            
            entityList.Add(e); 
            _sceneManager.SpawnEntity(e);
            e.Initialize();
            return e;
        }   

        /// <summary>
        /// Terminates an entity removing it from the game
        /// </summary>
        /// <param name="entityID">UID of the entity</param>
        public void Terminate(IEntity entity)
        {
            _sceneManager.RemoveEntity(entity.UID);
            _componentManager.RemoveComponents(entity.UID);
            entityList.Remove(entity);
            entity = null;
            
        }

        /// <summary>
        /// Search for a entity previously created
        /// </summary>
        /// <param name="entityName">Unique name of the entity to search for</param>
        /// <returns>Entity sought</returns>
        public IEntity GetEntity(string entityName)
        {
            IEntity entity = null;
            for (int i = 0; i < entityList.Count; i++)
            {
                if (entityList[i].UNAME == entityName)
                {
                    entity = entityList[i];
                }
            }
            if (entity == null)
            {
                Console.Write("Entity not found in entity list");
            }
            return entity;
        }

        /// <summary>
        /// Search for a entity previously created
        /// </summary>
        /// <param name="entityID">UID of the entity to search for</param>
        /// <returns>Entity that is sought</returns>
        public IEntity GetEntity(int entityID)
        {
            IEntity entity = null;
            for (int i = 0; i < entityList.Count; i++)
            {
                if (entityList[i].UID == entityID)
                {
                    entity = entityList[i];
                }
            }
            if (entity == null)
            {
                Console.Write("Entity not found in entity list");
            }
            return entity;
        }
    }
}
