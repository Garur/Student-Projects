using GameEngine.Components;
using Microsoft.Xna.Framework;

namespace GameEngine.Components.Steering
{
    interface ISteeringBehavior
    {
        //Calculates a force for the specific behavior
        Vector2 Calculate(CVehicle vehicle);
        //Runs we the behavior becomes active. Useful to reset some values of the behaviour
        void OnStart();
    }
}
