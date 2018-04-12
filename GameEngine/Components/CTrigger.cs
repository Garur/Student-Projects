using GameEngine.Collisions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Entities;
using Microsoft.Xna.Framework;

namespace GameEngine.GameEngine
{
    class CTrigger : ICollidable
    {
        IEntity _owner;


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
                return true;
            }
        }

        public Vector2 normal
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        public Vector2 origin
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
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

        public float radius
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        public void Initialize(IEntity ownerEntity)
        {
            _owner = ownerEntity;
        }

        public void Update(GameTime gametime)
        {
            
        }
    }
}
