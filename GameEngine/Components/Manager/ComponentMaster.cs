using GameEngine.Entities;
using GameEngine.Rendering;
using GameEngine.Components;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Collisions;

namespace GameEngine.Components
{
    /// <summary>
    /// Manages all the Component Managers. Calls the update methods of the managers.
    /// Ask the component factory to create components and returns it to the entity that requested the component
    /// Keeps a list of managers. Creates and destroyes managers based on the component necessity of entities
    /// Also registers and unregisters ICollidables and IVisibles
    /// </summary>
    public class ComponentMaster : IComponentMaster
    {
        static bool _instantiated = false;
        //List of Managers instanciated
        IList<ComponentManager> managersLists;    
        //Renderer reference to register the drawable components    
        IRenderer renderer;
        //Reference to the collision manager to register the collidable components
        ICollisionManager collisionManager;
        ComponentFactory componentFactory;

        public ComponentMaster()
        {
            //Only one instance of Component Manager is allowed.
            if (_instantiated)
                throw new InvalidOperationException("Component Manager is already instanciated");

            _instantiated = true;
            managersLists = new List<ComponentManager>();
        }
        
        public void Initialize()
        {
            componentFactory = new ComponentFactory();
            renderer = ServiceLocator.Instance.GetService<IRenderer>();
            collisionManager = ServiceLocator.Instance.GetService<ICollisionManager>();
        }

        /// <summary>
        /// Calls the Update method in all the managers
        /// </summary>
        public void Update(GameTime gameTime)
        {
            for(int i = 0; i < managersLists.Count; i++)
            {
                managersLists[i].Update(gameTime);
            }
        }
        /// <summary>
        /// Creates a component for that T type of IComponent
        /// Gets the manager of type T components and calls the factory to create a component
        /// </summary>
        /// <typeparam name="T">Type of IComponent to create</typeparam>
        /// <param name="owner">Owner entity to which the IComponent belongs</param>
        /// <returns>The instance of the created component</returns>
        public T RequestComponent<T>(IEntity owner) where T : IComponent, new()
        {
            //Create the component
            T c = componentFactory.CreateComponent<T>(owner);
            //Get the manager of type T
            ComponentManager manager = GetManager(typeof(T));
            manager.AddComponent(c);

            //Check if component is drawable -> register it
            IVisible drawable = c as IVisible;
            if (drawable != null)
            {
                renderer.RegisterDrawable(drawable);
            }
            //Check if component is collidable -> register it
            ICollidable collidable = c as ICollidable;
            if (collidable != null)
            {
                collisionManager.RegisterCollidable(collidable);
            }

            return c;
        }

        /// <summary>
        /// Searches for a Component Manager Instance depending on the Type type
        /// If the manager for that specific type does not exists 
        /// a new manager is created
        /// </summary>
        /// <param name="type">Type of the IComponent of the Manager</param>
        /// <returns>Returns the manager instance</returns>
        ComponentManager GetManager(Type type)
        {            
            ComponentManager manager;
            for (int i = 0; i < managersLists.Count; i++)
            {
                manager = managersLists[i];
                if(manager.factoryType == type)
                {
                    return manager;
                }
            }
            manager = new ComponentManager();
            manager.Initialize(type);
            managersLists.Add(manager);
            return manager;
        }

        /// <summary>
        /// Searchs for a specific IComponent. Gets the manager that has components of type  
        /// If the IComponent is not found returns null
        /// </summary>
        /// <typeparam name="T">Type of the IComponent to search for</typeparam>
        /// <param name="entityID">UID of entity that owns IComponent of type T</param>
        /// <returns>The IComponent of type T with the owner's ID specified</returns>
        public T GetComponent<T>(int entityID) where T: IComponent
        {
            ComponentManager manager;
            Type type = typeof(T);
            for (int i = 0; i < managersLists.Count; i++)
            {
                manager = managersLists[i];
                if (manager.factoryType == type)
                {
                    return manager.GetComponent<T>(entityID);
                }     
            }

            return default(T);
        }

        /// <summary>
        /// Removes all IComponents from the managers, that belong to a specific entity
        /// This method is called when an entity is destroyed
        /// </summary>
        /// <param name="entityID">UID of the entity owner of the components</param>
        public void RemoveComponents(int entityID)
        {
            ComponentManager manager;
            IComponent component;
            //Checks all managers to see where the entity has components associated 
            for (int i = 0; i < managersLists.Count; i++)
            {
                manager = managersLists[i];
                for (int j = 0; j < manager.components.Count; j++)
                {
                    component = manager.components[j];
                    if (component.Owner.UID == entityID)
                    {
                        //Check if component is drawable -> unregister it
                        IVisible drawable = component as IVisible;
                        if (drawable != null)
                        {
                            renderer.UnregisterDrawble(drawable);
                        }
                        //Check if component is collidable -> unregister it
                        ICollidable collidable = component as ICollidable;
                        if (collidable != null)
                        {
                            collisionManager.UnregisterCollidable(collidable);
                        }
                        //When a component associated with the entity is found remove it from the manager
                        manager.components.Remove(component);
                    }
                }               
            }
        }

        /// <summary>
        /// Deletes a specific component from its factory 
        /// If component is drawable or collidable, also unregisters it
        /// </summary>
        /// <param name="component">Compoment to remove</param>
        public void RemoveComponent(IComponent component)
        {
            ComponentManager factory;
            Type t = component.GetType();

            //Check if component is drawable -> unregister it
            IVisible drawable = component as IVisible;
            if (drawable != null)
            {
                renderer.UnregisterDrawble(drawable);
            }
            //Check if component is collidable -> unregister it
            ICollidable collidable = component as ICollidable;
            if (collidable != null)
            {
                collisionManager.UnregisterCollidable(collidable);
            }

            //Loops through all the manager to find the component
            for (int i = 0; i < managersLists.Count; i++)
            {
                factory = managersLists[i];                
                //Removes component from its factory
                if (factory.factoryType == t)
                {
                    factory.RemoveComponent(component);
                    break;
                }
            }
        }
    }
}
