using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Components.Steering
{
    /// <summary>
    /// Keeps a record of the last n forces and calculates the average, returning it
    /// Can be used to smooth the direction of the forces applied to a vehicle
    /// Produces the effect of lerping
    /// The more samples it keeps track, the slower the effect will be
    /// </summary>
    class DirectionSmoother
    {
        //Array with the history of the last vectors
        Vector2[] history;
        //index of the current sample, keeps track of the last vector added into the array
        int currSample;
                      
        public DirectionSmoother(int numSamples, Vector2 startingForce)
        {
            //initializes the array and sets a default force to all its elements
            //starting force should be the first force applied to the vehicle
            history = new Vector2[numSamples];
            for (int i = 0; i < history.Length; i++)
            {
                history[i] = startingForce;
            }
        }

        /// <summary>
        /// Receives a vector (can be a force or a direction)
        /// Adds it to the history array and calculates the average from all the forces recorded
        /// </summary>
        /// <param name="force">New force applied to the vehicle</param>
        /// <returns>Returns the average force calculated from the history </returns>
        public Vector2 UpdateSmooth(Vector2 force)
        {
            history[currSample] = force;
            currSample++;
            if (currSample >= history.Length)
            {
                currSample = 0;
            }
            Vector2 average = new Vector2();
            for (int i = 0; i < history.Length; i++)
            {
                average += history[i];
            }
            average /= history.Length;
            return average;
        }
    }
}
