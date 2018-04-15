using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace GameEngine.Components.Steering
{

    /// <summary>
    /// Similar to wander, but retricts the area where the vehicle will wander 
    /// to a circle of radius "limitRadius" with the origin on "center"
    /// Default area is the screen window
    /// </summary>
    class CenterWander : ISteeringBehavior
    {
        //Reference to the random generator initialized in Game1 
        Random random;
        //Counting timer used to activated the behavior
        float timer;
        //Random target generated
        Vector2 target = new Vector2();
        //Radius of the circle where the target point will be generated
        float radius = 100;
        //Distance of the circle origin from the vehicle
        float distance = 0; //zero means that a point can be generated around the target in any direction
        //Center point to wander around
        Vector2 center;
        //Limit maximum range of the wander
        float limitRadius;

        //Minimum and Max Time between searching for a target 
        int minTime = 1;
        int maxTime = 4;
        int timeToChange;
        
        public CenterWander()
        {
            //Gets a refenrence to the random object from Game1
            random = Game1.random;
            //Sets the behavior to get a new target on first update
            timer = maxTime;
            
            //Set default wander area -> Whole screen with center in the middle of the screen
            center = new Vector2(Game1.ScreenWidth / 2, Game1.ScreenHeight / 2);
            if(Game1.ScreenWidth > Game1.ScreenHeight)
            {
                limitRadius = Game1.ScreenWidth / 2;
            }else
            {
                limitRadius = Game1.ScreenHeight/ 2;
            }
        }

        public void OnStart()
        {
            //Every time the behaviour is activated -> get new target
            timer = maxTime;

        }

        /// <summary>
        /// Calculates the steering force for wander. Generates a random point in a circunference
        /// And subtracts the vehicle position, resulting in a direction vector
        /// </summary>
        /// <param name="vehicle">Vehicle affected by the force</param>
        /// <returns>Force calculated as a Vector2</returns>
        public Vector2 Calculate(CVehicle vehicle)
        {
            timer += (float)vehicle.GameTime.ElapsedGameTime.TotalSeconds;
            float dist = Vector2.DistanceSquared(vehicle.Owner.position, target);
            if (dist <= 30)
            {
                timer = maxTime;
            }
            //If it is time to get new target 
            if (timer >= timeToChange)
            {
                //Get a new angle to point the vehicle towards
                double angle = CalculateAngle(vehicle);
                target.X = (float)Math.Cos(angle) * radius;
                target.Y = (float)Math.Sin(angle) * radius;
                //Convert the generated point to the world position
                target += vehicle.Owner.position + (Vector2.Normalize(vehicle.orientation) * distance);

                //Reset the timer
                timer = 0;
                //set new value for the next change of target
                timeToChange = random.Next(minTime, maxTime);
            }
            //return the vector that points to the generated target from the vehicle
            return target - vehicle.Owner.position;

        }

        /// <summary>
        /// Generates a new angle base on the position of the vehicle relative 
        /// to the center position to where the vehicle should be wandering
        /// If the vehicle is close to the center position generate a random angle
        /// Else get the angle that points in the direction of the center
        /// </summary>
        /// <param name="vehicle"></param>
        /// <returns>the angle needed to calculate a new target</returns>
        double CalculateAngle(CVehicle vehicle)
        {
            double angle;
            //Gets the vector pointing from the vehicle to the center position 
            Vector2 direction = vehicle.Owner.position - new Vector2(Game1.ScreenWidth / 2, Game1.ScreenHeight / 2);
            //Calculates the distance from vehicle to center
            float dist = direction.Length(); //TODO: optimize

            //If the vehicle distance from the center is bigger than the allowed radius
            if (dist > limitRadius)
            {
                //Get the angle that points to the center point of wander
                double relativeAngle = Math.Atan(direction.Y / direction.X);
                //Give 15 degrees of randomness to the angle
                relativeAngle += random.NextDouble() * (0.261799);
                //If vehicle is on the second half of the screen regarding the x-axis
                if (vehicle.Owner.position.X <= Game1.ScreenWidth/ 2)
                {
                    return relativeAngle;
                }
                else
                {
                    return relativeAngle + Math.PI;
                }                
            }
            else
            {
                //Else return a random angle
                return angle = random.NextDouble() * Math.PI * 2;
            }           
            
        }

        /// <summary>
        /// Setup properties for to customize the behaviour
        /// </summary>
        /// <param name="_radius">Radius of the circunference where the target will be generated</param>
        /// <param name="_distance">Distance that the circle will be from the vehicle</param>
        /// <param name="_center">Origin of the wander circle</param>
        /// <param name="_limitRadius">Maximum range that the vehicle can go</param>
        /// <param name="_minTime">Minimum time before getting a new target </param>
        /// <param name="_maxtime">Maximum time before getting a new target </param>
        public void SetupProperties(float _radius, float _distance, Vector2 _center, float _limitRadius, int _minTime, int _maxtime)
        {
            radius = _radius;
            distance = _distance;
            center = _center;
            limitRadius = _limitRadius;
            minTime = _minTime;
            maxTime = _maxtime;
        }
    }
}
