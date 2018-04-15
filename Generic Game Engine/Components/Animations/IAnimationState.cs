using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Components
{
    /// <summary>
    /// Interface for animation states to be loaded in the animation state machine
    /// </summary>
    public interface IAnimationState
    {
        AnimationStateMachine ASMachine { get; }
       
        float Speed { get; set; } //Speed of the animation
        string Name { get; } //Name of the state/ animation
        bool IsRunning { get; set; } //States if the state is running
        bool IsLooping { get; set; } //States if the state should loop
        void Initialize(string _stateName, AnimationStateMachine _asm);
        void Update(GameTime gametime);
        void OnEnter();
        void OnExit();
        void PlayAnimation(); //Sets the animation running from the beggining
        void LoadFrames(IAnimationFrames framesStruct); //Loads the frames of the animation
        AnimationFrame GetCurrentFrame(); //Gets the current frame of the animation
        Texture2D GetCurrentTexture(); //Gets the current Texture of the animation
    }
}
