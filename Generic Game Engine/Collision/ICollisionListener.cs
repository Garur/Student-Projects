using GameEngine.Components;
using GameEngine.Entities;

namespace GameEngine.Collisions
{
    /// <summary>
    /// Entities implementing this interface 
    /// Receive events of collisions. They must have atleast one ICollidable component  
    /// </summary>
    public interface ICollisionListener
    {
        void OnCollisionEnter(Collision collision);
        void OnCollisionExit(Collision collision);
        void OnCollisionStay(Collision collision);
    }
}
