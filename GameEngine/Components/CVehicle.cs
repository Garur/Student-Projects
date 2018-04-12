using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Entities;
using GameEngine.Components.Steering;

namespace GameEngine.Components
{
    /// <summary>
    /// Manages all the steering behaviours within an entity
    /// Keeps a list of active behaviours
    /// and applies the combined force to the entity PhysicsBody Component
    /// </summary>
    class CVehicle : IComponent
    { 
        //Owner of vehicle component
        IEntity _owner;        
        //Reference to the physics body
        CPhysicsBody physicsBody;

        //List of steering behaviours
        IList<ISteeringBehavior> behaviors;
        //Max Force Magnitude allowed in each frame (needs to be public to be used by the behaviours)
        public float maxForce = 1f;
        //Max Speed of the vehicle, gets applied after the move force is applied 
        public float maxSpeed =0.1f;
        //Min speed to for the smooth direction to be active
        float minSmoothSpeed = 0.1f;

        //Current speed of the vehicle 
        float curSpeed;
        //States if 
        bool useSmoothing = true;
        //Reference to the direction smoother utility class
        DirectionSmoother directionSmoother;
        //Number of samples of the direction smoother
        int numSamples = 10;

        //Reference to Update gametime object to be use by steering behaviours
        public GameTime GameTime; 

        
        public IEntity Owner //Entity that to which the vehicle belongs to
        {
            get
            {
                return _owner;
            }

            set
            {
                _owner = value;
            }
        }
        
        public Vector2 velocity //Velocity of the vehicle
        {
            get
            {
                return physicsBody.velocity;
            }
        }
                
        public Vector2 orientation //Orientation of the vehicle
        {
            get
            {
                return Owner.orientation;
            }
            set
            {
                Owner.orientation = value;
            }
        }

        public void Initialize(IEntity ownerEntity)
        {
            Owner = ownerEntity;
            //Gets the physics body from the owner
            physicsBody = Owner.GetComponent<CPhysicsBody>();
            if(physicsBody == null)
            {
                throw new InvalidOperationException("Owner must have a physics body in order to initialize Vehicle Component"); 
            }

            orientation = Owner.orientation;
            behaviors = new List<ISteeringBehavior>();
            //Initializes the Direction Smoother
            directionSmoother = new DirectionSmoother(numSamples, new Vector2());          
        }

        /// <summary>
        /// Calculates the steering force from all the behaviors in the list
        /// Limits the resulting force to maxForce and applies the force
        /// Limits the maxSpeed of the vehicle and saves the orientation
        /// </summary>
        /// <param name="gametime"></param>
        public void Update(GameTime gametime)
        {
            //Copies the game time so it can be used in the steering behaviours
            GameTime = gametime;
            //Calculate Steering Force fro each of the active behaviours
            Vector2 force = new Vector2();                       
            for (int i = 0; i < behaviors.Count; i++)
            {
                force += behaviors[i].Calculate(this);
            }
            //Applies smoothing to the direction of the force applied
            if (useSmoothing && curSpeed > minSmoothSpeed)
            {
                force = directionSmoother.UpdateSmooth(force);
            }
            //Limits the max force applied to the vehicle
            if (force.Length() > maxForce)
            {
                force = Vector2.Normalize(force) * maxForce; 
            }
            //Applies the calculated force from all the behaviors to the vehicle physicBody
            physicsBody.ApplyForce(force);
            //Updates the curSpeed variable
            curSpeed = velocity.Length();    
            //Limits the max speed
            if (curSpeed > maxSpeed)
            {
                physicsBody.velocity = (velocity / curSpeed) * maxSpeed;
            }            
        }           
        /// <summary>
        /// Adds a new steering behavior to the list of behaviors
        /// </summary>
        /// <param name="behavior">Reference to the behavior to be added</param>
        public void AddBehaviour(ISteeringBehavior behavior)
        {
            if(behavior == null)
            {
                throw new NullReferenceException("Please initialize the behavior");
            }
            if (behaviors.Contains(behavior))
                return;
            behaviors.Add(behavior);
            behavior.OnStart();
        }

        /// <summary>
        /// Remove steering behavior from the list of behaviors
        /// </summary>
        /// <param name="behavior">Reference to the behavior to be removed</param>
        public void RemoveBehaviour(ISteeringBehavior behavior)
        {
            if (!behaviors.Contains(behavior))
                return;
            behaviors.Remove(behavior);
        }

        /// <summary>
        /// Setup the properties of the vehicle
        /// </summary>
        /// <param name="_maxForce">Max force that can be applied to the vehicle</param>
        /// <param name="_maxSpeed">Max speed that the vehicle can go</param>
        /// <param name="_useSmoothing">Apply smothing to the vehicle</param>
        public void SetupProperties(float _maxForce, float _maxSpeed, bool _useSmoothing)
        {
            maxForce = _maxForce;
            maxSpeed = _maxSpeed;
            useSmoothing = _useSmoothing;
        }

        /// <summary>
        /// Sets up the properties of the vehicle
        /// </summary>
        /// <param name="_maxForce">Max force that can be applied to the vehicle</param>
        /// <param name="_maxSpeed">Max speed that the vehicle can go</param>
        /// <param name="minSmoothSpeed">Minimum speed for the smooth direction to become active</param>
        /// <param name="_useSmoothing">Apply smothing to the vehicle</param>
        public void SetupProperties(float _maxForce, float _maxSpeed, float _minSmoothSpeed, bool _useSmoothing)
        {
            maxForce = _maxForce;
            maxSpeed = _maxSpeed;
            useSmoothing = _useSmoothing;
            minSmoothSpeed = _minSmoothSpeed;
        }

        /// <summary>
        /// Toogle the useSmoothing boolean
        /// activating or dectivating the smooth behaviour for the vehicle
        /// </summary>
        public void ToggleSmoothing()
        {
            if (useSmoothing)
                useSmoothing = false;
            else
                useSmoothing = true;
        }

        /// <summary>
        /// Checks if a type of behavior is currently active in the vehicle
        /// </summary>
        /// <typeparam name="T">Type of the steering behaviour to perform the check</typeparam>
        /// <returns>Returns true if there is a behaviour of that type in the behaviour list, returns false otherwise</returns>
        public bool IsBehaviourActive<T>() where T : ISteeringBehavior
        {
            Type t = typeof(T);
            for(int i = 0; i < behaviors.Count; i++)
            {
                if (behaviors[i].GetType() == t)
                {
                    return true;
                }
            }
            return false;
        }

        
        /// <summary>
        /// Finds a behaviour in the behavior list by type and returns it
        /// /// </summary>
        /// <typeparam name="T">Type of the behavior to search</typeparam>
        /// <returns>The behaviour of type T, returns null if there is no behavior of that type currently active</returns>
        public ISteeringBehavior GetBehaviour<T>() where T : ISteeringBehavior
        {
            Type t = typeof(T);
            for (int i = 0; i < behaviors.Count; i++)
            {
                if (behaviors[i].GetType() == t)
                {
                    return behaviors[i];
                }
            }
            return null;
        }

    }
}
