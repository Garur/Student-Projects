using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Entities;
using Microsoft.Xna.Framework;
using GameEngine.Collisions;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Components
{
    /// <summary>
    /// Applies physics to the entity through its position
    ///  
    /// </summary>
    class CPhysicsBody : IComponent
    {
        IEntity _owner;

        public Vector2 velocity;
        public Vector2 acceleration;        
        public float inverseMass;
        public float restitution;
        public float damping;
        //If true PhysicsBody will control the orientation of the entity
        public bool controlOrientation = true;
        float curSpeed;


        public IEntity Owner
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

        public CPhysicsBody()
        {
            velocity = new Vector2();
            acceleration = new Vector2();
        }

        public void Initialize(IEntity ownerEntity)
        {
            Owner = ownerEntity;
            //Sets default values for a rigidbody
            inverseMass = 0.5f;
            restitution = 1;
            damping = 0.9f;
        } 
               
        /// <summary>
        /// Applies physics to the entity 
        /// </summary>
        public void Update(GameTime gametime)
        {
            //Apply damping
            velocity *= damping; 
            //Apply accelaration
            velocity += acceleration; 
            //Apply the velocity to the entity position
            Owner.position += velocity;
            //Reset accelaration
            acceleration = new Vector2();

            //Sets the orientation of the entity
            if (controlOrientation)
            {
                curSpeed = velocity.Length();
                if (curSpeed > 0)
                {
                    Owner.orientation = velocity / curSpeed;
                }
            }
            
        }

        /// <summary>
        /// Applies a force to the body 
        /// The foce vector is multiplied by the mass and added to the accelaration
        /// </summary>
        /// <param name="force"></param>
        public void ApplyForce(Vector2 force)
        {
            acceleration += (force * inverseMass);
        }
        
        /// <summary>
        /// Applies an impulse to the body 
        /// the force is multiplied by restitution and added directly to the velocity
        /// </summary>
        /// <param name="force">Force vector to apply</param>
        public void ApplyImpulse(Vector2 force)
        {
            velocity += force * restitution;
        }
    }
}
