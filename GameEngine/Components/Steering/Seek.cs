using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using GameEngine.Entities;

namespace GameEngine.Components.Steering
{
    /// <summary>
    /// Calculates the force necessary to go towards a target
    /// </summary>
    class Seek : ISteeringBehavior
    {
        //Target entity to seek
        IEntity target;

        public void Initialize(IEntity _target)
        {
            target = _target;
        }

        /// <summary>
        /// Calculates the vector pointing to the target and multiplies by the intended force
        /// </summary>
        /// <param name="vehicle">Vehicle that the calculation affects</param>
        /// <returns>Returns the force necessary to seek</returns>
        public Vector2 Calculate(CVehicle vehicle)
        {
            Vector2 force = Vector2.Normalize(vehicle.Owner.position - target.position) * - vehicle.maxForce;
            return force;
        }

        public void OnStart()
        {
            
        }

        public void SetTarget(IEntity _target)
        {
            target = _target;
        }
    }
}
