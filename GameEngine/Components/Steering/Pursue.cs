using Microsoft.Xna.Framework;
using GameEngine.Entities;
using System;

/// <summary>
/// This behaviour was adapted from a behaviour previously developed in a past semester
/// </summary>
namespace GameEngine.Components.Steering
{
    /// <summary>
    /// Chases a target and tries to anticipate it by calculating where the target will be
    /// </summary>
    class Pursue : ISteeringBehavior
    {
        //Targer to chase
        CPhysicsBody evader;
        //Minimum distance to pursue, else change to seek
        float minDistance = 140;       

        /// <summary>
        /// Calculates the force need to pursue the target 
        /// </summary>
        /// <param name="vehicle">Reference to the vehicle being effected by the behavior</param>
        /// <returns>Returns the force necessary to cpurseu the target</returns>
        public Vector2 Calculate(CVehicle vehicle)
        {
            Vector2 force = new Vector2();
            //Distance from the target
            float dist = (evader.Owner.position - vehicle.Owner.position).Length();
            //If distance is small goes directly to the evader (seek)
            if (dist < minDistance)
            {
                force = Vector2.Normalize(vehicle.Owner.position - evader.Owner.position) * -vehicle.maxForce;
                return force;
            }   
            //if they are facing each other, go straight to target (saves other calculation)
            float relativeDir = Vector2.Dot(vehicle.orientation, evader.Owner.orientation);
            //relative direction is the angle between the orientation of the to vehicles 
            //if lesser than -18º it means the vehicle are facing each other                    
            if (relativeDir < -0.95f)
            {
                force = Vector2.Normalize(vehicle.Owner.position - evader.Owner.position) * -vehicle.maxForce;
                return force;
            }
            //Calculates lookAhead distance
            float lookAhead = dist / (evader.velocity.Length() + vehicle.maxSpeed);
            //Calculates target position to anticipate the movement of the target
            //Uses evader position, direction and lookAhead distance to generate the position
            Vector2 target = evader.Owner.position + (evader.Owner.orientation * lookAhead * 2);
            //Calculates the force needed to reach that target
            force = Vector2.Normalize(vehicle.Owner.position - target) * - vehicle.maxForce;
            return force;
        }

        /// <summary>
        /// Sets a new target to pursue
        /// </summary>
        /// <param name="_evader">New target</param>
        public void SetTarget(IEntity _evader)
        {
            evader = _evader.GetComponent<CPhysicsBody>();
            if (evader == null)
            {
                throw new Exception("Evader must contain a physics body component");
            }
        }

        public void OnStart()
        {
            
        }
    }
}
