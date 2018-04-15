using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GameEngine.Components
{
    /// <summary>
    /// Animation state made off various individual sprites/ frames 
    /// </summary>
    class SpriteAnimation : IAnimationState
    {
        //Name that identifies this state in the state machine
        string stateName;
        //Array of textures/ frames
        Texture2D[] frames;
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
            int framesToAdd = 1;
            if (time * Speed > interval)
            {       
                //Set texture to next frame          
                currentFrame.Texture = frames[frameIndex];
                frameIndex++;
                //Reset the timer
                time = 0f;
                
                //Calculate the next frame index
                framesToAdd = (int)(difference / interval);
                //The loop makes sure that the correct number of frames will be added
                //Some frames may be bypassed to keep the animation running at the correct speed
                for (int i = 0; i < framesToAdd; i++)
                {                    
                    frameIndex += i;
                    if (frameIndex >= frames.Length)
                    {
                        frameIndex = 0;
                        //If does is not looping, it will not run again  
                        if (!IsLooping)
                            IsRunning = false;
                    }
                }             
            }

            
        }

        /// <summary>
        /// Loads the frames of the animation
        /// </summary>
        /// <param name="framesStruct">Struct containing the array with the textures for each frame</param>
        public void LoadFrames(IAnimationFrames framesStruct)
        {
            SpriteAnimationFrames animationFrames = (SpriteAnimationFrames)framesStruct;
            frames = animationFrames.frames;
            currentFrame.Texture = frames[0];
            currentFrame.DrawArea = new Rectangle(0,0,currentFrame.Texture.Width, currentFrame.Texture.Height);
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
        /// Return the current animation frame
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
