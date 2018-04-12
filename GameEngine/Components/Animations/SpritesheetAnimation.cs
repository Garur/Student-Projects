using GameEngine.Components;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Components
{
    /// <summary>
    /// Animation state made off various areas of a spritesheet 
    /// </summary>
    class SpritsheetAnimation : IAnimationState
    {    
        //Name that identifies this state in the state machine
        string stateName;
        //Array of textures/ frames
        Texture2D[] frames;
        //Coordinates of the areas in the spritesheet to draw the various frames
        Rectangle[] drawAreas;
        //Reference to the current animation frame
        AnimationFrame currentFrame;
        //Timer to check when the current frame changes
        float time;
        //Miliseconds between each frame
        float interval = 84;
        //current index of the frame being played
        int frameIndex;
        //states if the animation is playing
        bool isRunning = true;
        //When true the animation will restart when it gets to the end. If false it only play once
        bool isLooping = true;
        //Reference to the state machine to which this state belongs
        public AnimationStateMachine ASMachine { get; set; }

        public string Name
        {
            get
            {
                return stateName;
            }
        }
        public float Speed { get; set; }

        public bool IsRunning
        {
            get
            {
                return isRunning;
            }

            set
            {
                isRunning = value;
            }
        }

        public bool IsLooping
        {
            get
            {
                return isLooping;
            }

            set
            {
                isLooping = value;
            }
        }

        public void Initialize(string _stateName, AnimationStateMachine _asm)
        {
            currentFrame = new AnimationFrame();
            stateName = _stateName;
            ASMachine = _asm;
            Speed = 1; //set default speed
        }

        /// <summary>
        /// If the animation is not running do not run update
        /// Checks when it is time to change the current frame
        /// If the animation does not loop it will run only once
        /// </summary>
        /// <param name="gametime">Gametime object from the kernel</param>
        public void Update(GameTime gametime)
        {
            if (!IsRunning)
                return;

            //Transition between frames
            time += (float)gametime.ElapsedGameTime.TotalMilliseconds;
            //Used to check how many animation frames should be processed
            float difference = time * Speed;
            //state how many frames should be added
            int framesToAdd = 1;
            if (difference > interval)
            {
                //Set texture to next frame          
                currentFrame.DrawArea = drawAreas[frameIndex];
                frameIndex++;
                //Reset the timer
                time = 0f;
                //Calculate how much frames should be added to the animation
                framesToAdd = (int)(difference / interval);
                //The loop makes sure that the correct number of frames will be added
                //Some frames may be bypassed to keep the animation running at the correct speed
                for (int i = 0; i < framesToAdd; i++)
                {
                    frameIndex += i;
                    if (frameIndex >= drawAreas.Length)
                    {
                        frameIndex = 0;
                    }
                }
            }

            if (!IsLooping)
                IsRunning = false;

        }

        /// <summary>
        /// Animation starts playing as soon has the state is active
        /// </summary>
        public void OnEnter()
        {
            PlayAnimation();
        }

        public void OnExit()
        {
            
        }
        /// <summary>
        /// Loads the frames of the animation
        /// </summary>
        /// <param name="framesStruct">Struct containing all the information needed to load the frames</param>
        public void LoadFrames(IAnimationFrames frames)
        {
            SpritesheetAnimationFrames animationFrames = (SpritesheetAnimationFrames)frames;            
            currentFrame.Texture = animationFrames.spriteSheet;
            currentFrame.DrawArea = animationFrames.drawAreas[0];
            currentFrame.origin = animationFrames.origin;
            drawAreas = animationFrames.drawAreas;
            frameIndex = 0;
        }

        /// <summary>
        /// Plays an animation from the beginning
        /// </summary>
        public void PlayAnimation()
        {
            isRunning = true;
            frameIndex = 0;
        }

        /// <summary>
        /// Getter for the current active frame
        /// </summary>
        public AnimationFrame GetCurrentFrame()
        {
            return currentFrame;
        }

        /// <summary>
        /// Returns the texture of the current animation state
        /// </summary>
        public Texture2D GetCurrentTexture()
        {
            return currentFrame.Texture;
        }
    }
}
