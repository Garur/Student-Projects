using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace GameEngine.Components.Steering
{
    /// <summary>
    /// Generates a new random target to move to, every interval between "minTime" and "maxTime" 
    /// The target will be generated in the circunference of a circle 
    /// When vehicle gets to the target, it will get a new one even if the interval has not passed.
    /// </summary>
    class Wander : ISteeringBehavior
    {
        //Random instance to generate random values
        Random random;
        //Used to generate a new target every N seconds
        float timer;
        //Random target generated
        Vector2 target = new Vector2();
        //Time for the next target to be generated, it value is between minTime and maxTime
        int timeInterval;

        /* CUSTOMIZABLE PROPERTIES */
        //Radius of the circle where the target point will be generated
        float radius = 50;
        //Distance of the circle origin from the vehicle
        float distance = 1;
        //Minimum distance that the vehicle will get to the target before searching for new target
        int minDistanceToTarget = 10;
        //Time between searches (max and min)
        int minTime = 1;
        int maxTime = 4;
       

        public Wander()
        {
            random = Game1.random;
            timer = maxTime;
        }

        public void OnStart()
        {
           
        }

        /// <summary>
        /// Calculates the steering force for wander. Generates a random point in a circunference
        /// And subtracts the vehicle position, resulting in a direction vector
        /// </summary>
        /// <param name="vehicle">Vehicle affected by the steering force</param>
        /// <returns>Force Vector2 of calculated behaviour</returns>
        public Vector2 Calculate(CVehicle vehicle)
        {
            //Gets the Elapsed game time from the last frame and adds it to the timer
            timer += (float) vehicle.GameTime.ElapsedGameTime.TotalSeconds;
            //Calculates the distance of the vehicle from the current target
            float dist = Vector2.DistanceSquared(vehicle.Owner.position, target);

            //If distance is less than minDistanceToTarget -> Set timer equal to max timer to get a new target 
            if ( dist <=  minDistanceToTarget)
            {
                timer = maxTime;
            }
            
            if (timer >= timeInterval)
            {
                //Generates an angle from 0 to 360 degrees
                double angle = random.NextDouble() * Math.PI * 2;
                //Generates X and Y coordinates on the circunference
                target.X = (float)Math.Cos(angle) * radius;
                target.Y = (float)Math.Sin(angle) * radius;
                //Converts the generated coordinates to world position and adds the distance from the vehicle
                target += vehicle.Owner.position + (Vector2.Normalize(vehicle.orientation) * distance);
                //Resets timer
                timer = 0;
                //set new interval time for the next change of target
                timeInterval = random.Next(minTime, maxTime);
            }
            //returns the vector pointing from position to target generated
            return target - vehicle.Owner.position;            

        }

       
    }
}
