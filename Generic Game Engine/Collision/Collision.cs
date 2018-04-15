using GameEngine.Entities;
using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Collisions
{
    /// <summary>
    /// Contains the data resulted from a collision
    /// The collision manager signals ICollisionListeners when a collision occurs
    /// and passes in the parameters a collision object
    /// </summary>
    public class Collision : EventArgs
    {        
        public ICollidable other; //Other collidable that collided with the listener that received event signal
    }
}
