using GameEngine.Entities;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Components
{
    /// <summary>
    /// Manages a type of IComponent for all the entities that own that type of component.
    /// Updates and draws all the components of that specific type
    /// Creates and Initializes components
    /// adds and removes components from components List.
    /// Factory gets created in the Component Manager class
    /// </summary>
    class ComponentManager
    {
        //List that contains all the references for specific component
        public IList<IComponent> components;
        //States the type of IComponent that instance of factory holds
        public Type factoryType;

        public void Initialize(Type type)
        {
            factoryType = type;
            components = new List<IComponent>();
        }

        /// <summary>
        /// Calls update method for all components in list
        /// </summary>
        /// <param name="gameTime"></param>
        public void Update(GameTime gameTime)
        {
            for (int i = 0; i < components.Count; i++)
            {
                components[i].Update(gameTime);
            }
        }

        /// <summary>
        /// Removes a component from the component list 
        /// </summary>
        /// <param name="component"></param>
        public void AddComponent(IComponent component)
        {
            components.Add(component);
        }

        /// <summary>
        /// Removes a component from the component list 
        /// </summary>
        /// <param name="component">Component to be removed </param>
        public void RemoveComponent(IComponent component)
        {
            components.Remove(component);
            component = null;
        }

        /// <summary>
        /// Returns a component based on the UID of the owner entity. 
        /// </summary>
        /// <typeparam name="T">Type of the component to return</typeparam>
        /// <param name="entityID">UID of the owner entity</param>
        /// <returns>Returns the component if it is found</returns>
        public T GetComponent<T>(int entityID)
        {            
            for(int i = 0; i < components.Count; i++)
            {
                if(components[i].Owner.UID == entityID)
                {
                    return (T) components[i];
                }
            }
            return default(T);
        }

    }
}
