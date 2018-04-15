using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using GameEngine.Entities;

namespace GameEngine.Components.Steering
{
    /// <summary>
    /// Calculates a force necessary to flee from a target
    /// </summary>
    class Flee : ISteeringBehavior
    {
        //Target to evade
        IEntity target;

        public void Initialize(IEntity _target)
        {
            target = _target;
        }

        /// <summary>
        /// Calculates the vector pointing away from the target and multiplies by the intended force
        /// </summary>
        /// <param name="vehicle">Vehicle that the calculation affects</param>
        /// <returns>Returns the force necessary to evade</returns>
        public Vector2 Calculate(CVehicle vehicle)
        {            
            Vector2 force = Vector2.Normalize(vehicle.Owner.position - target.position) * vehicle.maxForce;
            return force;
        }

        public void OnStart()
        {
            
        }
    }
}
