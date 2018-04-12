using GameEngine.Beetles;
using GameEngine.Components;
using GameEngine.Entities;
using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;

namespace GameEngine.Collisions
{    
    /// <summary>
    /// Publisher of collision events and responsible to calculate 
    /// collisions between objects. Signals listeners when 
    /// OnCollisionEnter, Exit or Stay events occur 
    /// Registers Listeners (Entiies) and collidables (Components). 
    /// For an entity to listen to collision events it must own a ICollidable component
    /// </summary>
    public class CollisionManager : ICollisionManager
    {
        //Limits the class to a single intance
        static bool _instantiated = false;

        // Event Delegates
        delegate void CollisionHandler(Collision other);
        event CollisionHandler CollisionEnter;
        event CollisionHandler CollisionExit;
        event CollisionHandler CollisionStay;
        // List of ICollidable to check for collisions
        IList<ICollidable> collidables;
        //States which collidables belong to each Entity (IListeener), stores the index of the collider from his place in the collidables list
        IList<List<ICollidable>> links; 
        //Is equivalent a collision matrix, represents which objects are currently colliding with which other.
        IList<List<int>> currentStatus;

        public CollisionManager()
        {
            //Only one instance of Collision Manager is allowed.
            if (_instantiated)
                throw new InvalidOperationException("Scene Manager is already instanciated");
            _instantiated = true;

            collidables = new List<ICollidable>();
            links = new List<List<ICollidable>>();
            currentStatus = new List<List<int>>();
        }

        ~CollisionManager()
        {
            _instantiated = false;
        }

        /// <summary>
        /// Register the entity associated with listener
        /// Subscribes the Listeners to Enter and Exit Collision events
        /// At this stage the entities do not own a Collider Component
        /// Creates a list for each listener to link the respective ICollidables
        /// </summary>
        /// <param name="listener"></param>
        public void RegisterListener(ICollisionListener listener)
        {          
            //Adds a new Links list that contains the links of this listener to the colliders that it owns
            links.Add(new List<ICollidable>());
            //Subscribes Listener
            CollisionEnter += new CollisionHandler(listener.OnCollisionEnter);
            CollisionExit += new CollisionHandler(listener.OnCollisionExit);
            CollisionStay += new CollisionHandler(listener.OnCollisionStay);
        }       

        /// <summary>
        /// Register an ICollidable component in the collidables list 
        /// and associates it with the specific listeener
        /// Colliders might not have an entity Listeening, 
        /// they still have to be registered for collision detection for other entities
        /// </summary>
        /// <param name="collidable">ICollidable to register</param>
        public void RegisterCollidable(ICollidable collidable)
        {
            collidables.Add(collidable);
            int index = collidables.Count - 1;
            //For every collider create a status list representing it status with all other colliders
            currentStatus.Add(new List<int>());
            //Add the number of values to the list equal to the number of current colliers       
            for (int i = 0; i < collidables.Count - 1; i++)
            {
                currentStatus[index].Add(0);
            }
            //If there are no listeners subscribed -> links can't be created yet
            if (CollisionEnter == null)
                return;

            
            //Checks if the collidable belongs to an already registered entity
            Delegate[] list = CollisionEnter.GetInvocationList();            
            for (int i = 0; i < list.Length; i++)
            {
                IEntity entity = list[i].Target as IEntity;
                if (collidable.Owner == entity)
                {
                    //Links the collider with the listeener
                    links[i].Add(collidable);
                    break;
                }
            }
        }

        /// <summary>
        /// Unregisters listeners to collision events
        /// Removes collision info from the current status
        /// Removes links of the listener and collides from the links list
        /// This is called after all the colliders have been removed
        /// </summary>
        /// <param name="listener">Listener</param>
        public void UnregisterListener(ICollisionListener listener)
        {
            //Remove the listeener links from the links lists
            Delegate[] list = CollisionEnter.GetInvocationList();
            for (int i = 0; i < list.Length; i++)
            {
                ICollisionListener t_listener = list[i].Target as ICollisionListener;
                if (t_listener == listener)
                {
                    links.RemoveAt(i);
                    break;
                }
            }

            //Unsubscribe to events
            CollisionEnter -= new CollisionHandler(listener.OnCollisionEnter);
            CollisionExit -= new CollisionHandler(listener.OnCollisionExit);
            CollisionStay -= new CollisionHandler(listener.OnCollisionStay);

           
        }

        /// <summary>
        /// Unregisters a collider, stopping tracking collisions
        /// This is called before an entity is removed and listener are unregitered
        /// </summary>
        /// <param name="collidable">ICollidable to stop tracking</param>
        public void UnregisterCollidable(ICollidable collidable)
        {
            int index = collidables.IndexOf(collidable);         

            //Removes collision info from CurrentStatuslist for this specific collider
            for (int i = collidables.Count - 1; i >= 0; i--)
            {
                if (i == index)
                {
                    //When it reachs the index of the collider it means previous index have no reference to this collider
                    currentStatus.RemoveAt(index);
                    break;
                }
                currentStatus[i].RemoveAt(index);
            }   
            Delegate[] list = CollisionEnter.GetInvocationList();
            //Checks if the collidable belongs to a already registered entity
            for (int i = 0; i < list.Length; i++)
            {
                IEntity entity = list[i].Target as IEntity;
                if (collidable.Owner == entity)
                { 
                    //Links the collider with the listeener
                    links[i].Remove(collidable);
                    break;
                }
            }

            //Remove the collidable from the list
            collidables.Remove(collidable);
        }

        /// <summary>
        /// Update method for the collision manager, runs on Game1 instance
        /// </summary>
        public void Update()
        {
            CheckForCollisions();
        }

        /// <summary>
        /// Performs all the collision checks for the registered ICollidables
        /// </summary>
        void CheckForCollisions()
        {
            //Loops through all the collidables and check collision between i and j
            for (int i = collidables.Count - 1; i >= 0; i--)
            {                
                for (int j = collidables.Count - 1; j >= 0; j--)
                {
                    //When i is less than j, means that this collision has been checked already
                    if (i <= j)
                    {
                        continue;
                    }                    
                    
                    bool hasCollided = false;
                    //Check for collision between two Cirles             
                    if(collidables[i].bodyType == BodyType.circle && collidables[j].bodyType == BodyType.circle)
                    {
                        hasCollided = CheckCollisionCircle(i, j);
                    }
                    //If Colliders have collided
                    if (hasCollided)
                    {
                        //If currentStatus is zero, colliders were not colliding in previous frame
                        if (currentStatus[i][j] == 0)
                        {
                            //On Collision Enter Event
                            currentStatus[i][j] = 1;                                                        
                            NotifyOnCollisionEnter(GetListeenerIndex(collidables[i]), collidables[j]);
                            NotifyOnCollisionEnter(GetListeenerIndex(collidables[j]), collidables[i]);

                        }//If currentStatus is one, colliders were colliding in previous frame and still are
                        else if (currentStatus[i][j] == 1)
                        {
                            //Collision Stay event
                            NotifyOnCollisionStay(GetListeenerIndex(collidables[i]), collidables[j]);
                            NotifyOnCollisionStay(GetListeenerIndex(collidables[j]), collidables[i]);
                        }
                    }//If currentStatus is one, and not collision now, colliders were colliding in previous frame but stopped
                    else if (currentStatus[i][j] == 1)
                    {
                        //Collision Exit
                        currentStatus[i][j] = 0;
                        NotifyOnCollisionExit(GetListeenerIndex(collidables[i]), collidables[j]);
                        NotifyOnCollisionExit(GetListeenerIndex(collidables[j]), collidables[i]);
                    }
                }
            }            
        }
        /// <summary>
        /// Check for the collision between to ICollidables that have a circle body type
        /// If collission is detected Notify the entities 
        /// </summary>
        /// <param name="i">Index of the first ICollidable to check</param>
        /// <param name="j">Index of the second ICollidable to check</param>
        bool CheckCollisionCircle(int i, int j)
        {            
            float distance = Vector2.Distance(collidables[i].origin, collidables[j].origin);
            if (distance < collidables[i].radius + collidables[j].radius)
            {                
                return true;
            }
            return false;
        }

        /// <summary>
        /// Retrives the index of a listeener associated with a specific collider
        /// </summary>
        /// <param name="collidable">ICollidable to search correspondent listener</param>
        /// <returns>Returns the index of the IListeener, to which the collidable belongs</returns>
        int GetListeenerIndex(ICollidable collidable)
        {
            for (int i = 0; i < links.Count; i++)
            {
                for (int j = 0; j < links[i].Count; j++)
                {
                    if (links[i][j] == collidable)
                    {
                        return i;
                    }
                }
            }
            return -1;
        }

        /// <summary>
        /// Notifies one entities listenning onEnterCollision event
        /// It will only invoke collision events in the entity that 
        /// is involved in that specific collision
        /// Usefull for when only one side of the collision needs to be notified
        /// If there are no listeeners correspoding to that index, nothing happens
        /// </summary>
        /// <param name="index">index of the entity to signal. Index in the Delegate[] is the same in the links list</param>
        /// <param name="collidable">collider that collided with the entity</param>
        void NotifyOnCollisionEnter(int index, ICollidable collidable)
        {
            if (CollisionEnter != null)
            {
                //Console.WriteLine("OnCollisionEnter");
                Delegate[] list = CollisionEnter.GetInvocationList();
                //Check if the index of the entity is valid
                if(index < list.Length && index > -1)
                {
                    Collision collision = new Collision();
                    collision.other = collidable;
                    list[index].DynamicInvoke(collision);
                }                
            }
        }

        /// <summary>
        /// Notifies one entitiy listenning onExitCollision event
        /// It will only invoke collision events in the entity that 
        /// is involved in that specific collision
        /// Usefull for when only one side of the collision needs to be notified
        /// If there are no listeeners correspoding to that index, nothing happens
        /// </summary>
        /// <param name="index">index of the entity to signal. Index in the Delegate[] is the same in the links list</param>
        /// <param name="collidable">collider that collided with that entity</param>
        void NotifyOnCollisionExit(int index, ICollidable collidable)
        {
            if (CollisionExit != null)
            {
                Delegate[] list = CollisionExit.GetInvocationList();
                //Check if the index of the entity is valid
                if (index < list.Length && index > -1)
                {
                    Collision collision = new Collision();
                    collision.other = collidable;
                    list[index].DynamicInvoke(collision);
                }
            }
        }

        /// <summary>
        /// Notifies one entitiy listenning onCollisionStay event
        /// It will only invoke collision events in the entity that 
        /// is involved in that specific collision
        /// If there are no listeeners correspoding to that index, nothing happens
        /// </summary>
        /// <param name="index">index of the entity to signal. Index in the Delegate[] is the same in the links list</param>
        /// <param name="collidable">collider that collided with that entity</param>
        void NotifyOnCollisionStay(int index, ICollidable collidable)
        {
            if (CollisionStay != null)
            {
                //Console.WriteLine("OnCollisionStay");
                Delegate[] list = CollisionStay.GetInvocationList();
                if (index < list.Length && index > -1)
                {
                    Collision collision = new Collision();
                    collision.other = collidable;
                    list[index].DynamicInvoke(collision);
                }                    
            }
        }
    }
}
