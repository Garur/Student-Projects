using GameEngine.Collisions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using GameEngine.Components;
using GameEngine.Entities;

namespace GameEngine.Components
{
    /// <summary>
    /// Operates as the collision box of an entity
    /// When Created it gets registered by the collision Master in the collision Manager
    /// The collision manager checks if this collider collides with any other.
    /// All colliders are circles by default 
    /// </summary>
    public class CCollider : ICollidable
    {
        IEntity _owner;

        public CCollider (){
            //Default Collider Values;            
            radius = 10;  
        }

        public BodyType bodyType
        {
            get
            {
                return BodyType.circle;
            }
        }

        public bool isTrigger
        {
            get
            {
                return false;
            }
        }

        public Vector2 origin
        {
            get
            {
                return _owner.position;
            }
        }

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

        public float radius { get; set; }

        public void Initialize(IEntity ownerEntity)
        {
            _owner = ownerEntity;
            //Auto set radius is on by default
            AutoSetRadius();          
        }

        public void Update(GameTime gametime)
        {
            
        }

        /// <summary>
        /// Sets the radius of the the collider based 
        /// on the draw area of the Animator or SpriteRender for the first frame loaded
        /// The Radius will be half the height or the width depending on the one that is bigger
        /// It does not do anything if there is no animator or sprite renderer
        /// </summary>
        void AutoSetRadius()
        {
            CAnimator c = _owner.GetComponent<CAnimator>();
            if (c != null)
            {
                Rectangle rect = c.GetDrawArea();
                if (rect.Width > rect.Height)
                    radius = rect.Width / 2;
                else
                    radius = rect.Height / 2;
            }
            else
            {
                CSpriteRenderer s = _owner.GetComponent<CSpriteRenderer>();
                if (s != null)
                {
                    Rectangle rect = s.GetDrawArea();
                    if (rect.Width > rect.Height)
                        radius = rect.Width / 2;
                    else
                        radius = rect.Height / 2;
                }
            }
        }

        /// <summary>
        /// Set the radius to r 
        /// </summary>
        /// <param name="r">Value of radius</param>
        public void SetRadius(int r)
        {
            radius = r;
        }
    }
}
