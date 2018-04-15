using GameEngine.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Components
{
    /// <summary>
    /// Responsible for creating Components and Initialized them
    /// </summary>
    class ComponentFactory
    {
        /// <summary>
        /// Creates a component of type T and adds it to the list
        /// </summary>
        /// <typeparam name="T">Type of the IComponent to be created</typeparam>
        /// <param name="owner">Entity that ownes the the IComponent </param>
        /// <returns>Return the component</returns>
        public T CreateComponent<T>(IEntity owner) where T : IComponent, new()
        {
            T c = new T();            
            c.Initialize(owner);
            return c;
        }
    }
}
