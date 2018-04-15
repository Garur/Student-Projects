using GameEngine.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Entities
{
    class EntityFactory
    {
        //ID of the next entity to be created
        static int nextID;

        public EntityFactory()
        {
            nextID = 0;
        }

        /// <summary>
        /// Creates a Game Entity and adds it to the list of entities 
        /// Sets the UID and the UName of the entity.
        /// Also spawns the entity
        /// </summary>
        /// <typeparam name="T"> Type of the entity created</typeparam>
        /// <returns>The entity created</returns>
        public T CreateEntity<T>() where T : IEntity, new()
        {
            T e = new T();
            //Set UID
            e.UID = nextID;
            nextID++;
            //Set UNAME
            Type type = typeof(T);
            e.UNAME = type.ToString() + "_" + e.UID.ToString();            
            return e;
        }

    }
}
