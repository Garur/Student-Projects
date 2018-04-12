using GameEngine.Components;
using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Collisions
{
    /// <summary>
    /// Components that should collide with others should implement 
    /// the ICollidable inferface. The collision manager registers ICollidables
    /// and calculates the collisions between all registered collidables
    /// </summary>
    public interface ICollidable : IComponent
    {
        bool isTrigger { get; } //CURENTLY NOT IN USE
        float radius { get; set; } //Radius of the collidable
        Vector2 origin { get; } //Positon of the collidable
        BodyType bodyType { get; } //Shape of the collidable
    }
    public enum BodyType
    {
        circle,
        line
    }
}
