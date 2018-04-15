using GameEngine.Entities;
using Microsoft.Xna.Framework;
using System;

/// <summary>
/// This behaviour was adapted from a behaviour previously developed in a past semester
/// </summary>
namespace GameEngine.Components.Steering
{
    /// <summary>
    /// Calculates a force away from the chaser. 
    /// Checks the movement of the chaser to move away from where it will be 
    /// </summary>
    class Evade : ISteeringBehavior
    {
        // Reference of the physics body chasing this vehicle
        CPhysicsBody chaser;

        /// <summary>
        /// Sets a new chaser
        /// </summary>
        /// <param name="_chaser">Entity chasing this vehicle</param>
        public void SetChaser(IEntity _chaser)
        {
            chaser = _chaser.GetComponent<CPhysicsBody>();
            if(chaser == null)
            {
                throw new Exception("Chaser must contain a physics body component");
            }
        }

        /// <summary>
        /// Calculates a force away from the chaser
        /// </summary>
        /// <param name="vehicle">Reference for the vehicle</param>
        /// <returns>Returns the force calculated to be applied to the vehicle</returns>
        public Vector2 Calculate(CVehicle vehicle)
        {
            //Vector moving away from the chaser
            Vector2 fromChaser = chaser.Owner.position - vehicle.Owner.position;
            Vector2 force = new Vector2();
            
            //Angle to check if both vehicle are facing each other
            float relativeDir = Vector2.Dot(vehicle.orientation, chaser.Owner.orientation);
            //if they are facing each other, go straight to evader -> Acos(18º) = -0.95f
            if (relativeDir < -0.95f)
            {
                force = Vector2.Normalize(vehicle.Owner.position - chaser.Owner.position) * -vehicle.maxForce;
                return force;
            }
            //Else calculate lookAhead distance 
            float lookAhead = fromChaser.Length() / (chaser.velocity.Length() + vehicle.maxSpeed);
            // Calculates the target position to move away from the evader
            // Based on the position of the chaser, its orientation and lookAhead distance
            Vector2 target = chaser.Owner.position + (chaser.Owner.orientation * lookAhead * 2);
            //Calculates the force to move to that target
            force = Vector2.Normalize(vehicle.Owner.position - target) * vehicle.maxForce;
            return force;
        }

        public void OnStart()
        {
            
        }
    }
}
