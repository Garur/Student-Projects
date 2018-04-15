using System;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using GameEngine.Collisions;
using GameEngine.Resources;
using GameEngine.Components;
using System.Collections.Generic;

namespace GameEngine.Entities
{
    /// <summary>
    /// Basic game object 
    /// Has a position and orientation in the world
    /// Can be a spawned into the world 
    /// Can have components to apply different game behaviours
    /// All game objects should inherit from this class
    /// </summary>
    abstract class Entity : IEntity 
    {

        Vector2 _orientation = new Vector2(0, 1); //orientation of the entity in the world (unit vector)
        bool _isActive = false;
        bool _toBeTerminated = false;

        public int UID { get; set; } 
        public string UNAME { get; set; } 
        public Vector2 position { get; set; } 

        public bool isActive 
        {
            get
            {
                return _isActive;
            }

            set
            {
                _isActive = value;
            }
        }

        bool IEntity.toBeTerminated
        {
            get
            {
                return _toBeTerminated;
            }

            set
            {
                _toBeTerminated = value;
            }
        }

        public Vector2 orientation
        {
            get
            {
                return _orientation;
            }

            set
            {
                _orientation = Vector2.Normalize(value);
            }
        }

        public abstract void Initialize();
        public abstract void Update(GameTime gameTime);
        public void TerminateSelf()
        {
            _isActive = false;
            _toBeTerminated = true;
        }

        /// <summary>
        /// Adds a component to the entity
        /// </summary>
        /// <typeparam name="T">Type of the component to be added to the entity</typeparam>
        /// <returns>returns the component created in the factory</returns>
        public T AddComponent<T>() where T : IComponent, new()
        {
            IComponentMaster componentMaster = ServiceLocator.Instance.GetService<IComponentMaster>();
            T c = componentMaster.RequestComponent<T>(this);
            return c;
        }

        /// <summary>
        /// Searches for a component and returns it
        /// </summary>
        /// <typeparam name="T">Type of the component to search</typeparam>
        /// <returns>Instance of the component to search</returns>
        public T GetComponent<T>() where T: IComponent
        {
            IComponentMaster componentMaster = ServiceLocator.Instance.GetService<IComponentMaster>();
            T c = componentMaster.GetComponent<T>(UID);
            return c;
        }

        /// <summary>
        /// Removes a component from the entity and deletes it
        /// </summary>
        /// <param name="component"></param>
        public void RemoveComponent(IComponent component)
        {
            IComponentMaster componentMaster = ServiceLocator.Instance.GetService<IComponentMaster>();
            componentMaster.RemoveComponent(component);
            component = null;
        }
    }
}
