using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;

namespace GameEngine.Components.Steering
{
    /// <summary>
    /// Restringes the movement of the vehicle to the screen size
    /// Calculates a force pointing inwards based on the position of the vehicle
    /// The futher the vehicle is out of the window screen the strong the force is
    /// </summary>
    class WallAvoidance : ISteeringBehavior
    {
        int mapOffset = -30;

        public Vector2 Calculate(CVehicle vehicle)
        {
            Vector2 force = new Vector2();
            float x_strenght = 0;
            float y_strenght = 0;

            //Checks if the vehicle has left the screen on the x coordinate
            //Set a target towards the window view
            if (vehicle.Owner.position.X <= -mapOffset)
            {
                force += new Vector2(1,0);
                x_strenght = Math.Abs(vehicle.Owner.position.X + mapOffset);
                force.X *= x_strenght;

            }
            else if (vehicle.Owner.position.X >= Game1.ScreenWidth + mapOffset)
            {
                force += new Vector2(-1, 0);
                x_strenght = Math.Abs(vehicle.Owner.position.X - Game1.ScreenWidth + mapOffset);
                force.X *= x_strenght;
            }
            //Checks if the vehicle has left the screen on the y coordinate
            //Set a target towards the window view
            if (vehicle.Owner.position.Y >= Game1.ScreenHeight + mapOffset)
            {
                force += new Vector2(0, -1);
                y_strenght = Math.Abs(vehicle.Owner.position.Y - Game1.ScreenWidth + mapOffset);
                force.Y *= y_strenght;

            }
            else if (vehicle.Owner.position.Y <= - mapOffset)
            {
                force += new Vector2(0, 1);
                y_strenght = Math.Abs(vehicle.Owner.position.Y + mapOffset);
                force.Y *= y_strenght;
            }            
            return force ;
        }

        public void OnStart()
        {
            
        }
    }
}
