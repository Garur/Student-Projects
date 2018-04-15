using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameEngine.Entities;
using Microsoft.Xna.Framework;
using GameEngine.Resources;
using Microsoft.Xna.Framework.Graphics;
using GameEngine.Rendering;

namespace GameEngine.Components
{
    /// <summary>
    /// Draws Animations for an entity
    /// </summary>
    class CAnimator : IComponent, IVisible
    {
        IEntity _owner;
        IResourceManager resourceManager;
        //State Machine that manages the animation states
        AnimationStateMachine ASMachine;
        //Current texture of the current frame to be drawn
        Texture2D texture;
        //Color of the texture (white by default)
        Color color = Color.AntiqueWhite;
        //Coordinates in the spritesheet to draw the current frame
        Rectangle drawArea;
        //Orientation of the sprite
        float rotationAngle;
        //Position of where to draw the sprite
        Vector2 position;
        //Center/ Origin of the texture, can also be seen as the rotation axis
        Vector2 origin;
        //States if the animator is currently drawing textures
        bool isVisible = true;

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

        public Texture2D Texture
        {
            get
            {
                return texture;
            }

            set
            {
                texture = value;
            }
        }        

        public void Initialize(IEntity ownerEntity)
        {
            ASMachine = new AnimationStateMachine();
            _owner = ownerEntity;
            resourceManager = ServiceLocator.Instance.GetService<IResourceManager>();
            ASMachine.Initialize(_owner);
            
        }

        /// <summary>
        /// Loads an animation based on the name of the texture previouly from a spritesheet. Texture should have been 
        /// previously loaded in the resource manager. Set the number of rows and columns 
        /// and choose the row that contains the animation 
        /// Animation are loaded from a spritesheet from a row
        /// </summary>
        /// <param name="animationName">Name to be given to the state and the animation</param>
        /// <param name="textureName">Name of the texture file/ spritesheet</param>
        /// <param name="numRows">Number of rows that the spritesheet has</param>
        /// <param name="numCols">Number of cols that the spritesheet has</param>
        /// <param name="row">Row of the animation </param>
        /// <returns>Returns the state created that hold the animation</returns>
        public IAnimationState LoadAnimation(string animationName, string textureName, int numRows, int numCols, int row)
        {
            //Creates a new animation
            IAnimationState animation = ASMachine.LoadState<SpritsheetAnimation>(animationName);
            //Get the texture/ spritesheet fro mthe resources
            texture = resourceManager.GetTexture(textureName);

            //Calculates the width and height of each frame
            int width = texture.Width / numCols;
            int height = texture.Height / numRows;
            //Calculates the pixels of the specific row to get the various frames
            int y = (height) * (row - 1);            

            List<Rectangle> drawAreas = new List<Rectangle>();
            int x = 0;
            //Gets all the textures of a specific row
            while (true)
            {               
                drawAreas.Add(new Rectangle(x,y,width, height));
                x += width;
                if (x + width >= texture.Width)
                    break;
            }            
            //Loads all the animation frames found into the animation
            SpritesheetAnimationFrames animationFrames = new SpritesheetAnimationFrames();
            animationFrames.drawAreas = drawAreas.ToArray();
            animationFrames.spriteSheet = texture;
            animationFrames.origin = new Vector2(drawAreas[0].Width / 2, drawAreas[0].Height / 2);
            animation.LoadFrames(animationFrames);
            return animation;
        }

        /// <summary>
        /// Loads a sprite animation based on the name of the files
        /// The texture files need to be loaded previously
        /// The names of the animation frame files should begin with the same string 
        /// and add the number of the frame separated with an underscore
        /// </summary>
        /// <param name="textureName">Identifier of the texture files of each frame</param>
        /// <returns>Returns the created Animation State</returns>
        public IAnimationState LoadAnimation(string animationName, string textureName)
        {
            //Create the animation state
            IAnimationState animation = ASMachine.LoadState<SpriteAnimation>(textureName);
            //Get the initial texture for the first frame
            Texture2D frame = resourceManager.GetTexture(textureName);
            if (frame == null)
            {
                frame = resourceManager.GetTexture(textureName + "_0");
            }

            List<Texture2D> frames = new List<Texture2D>();
            int i = 1;
            //Adds the various frames until there are no more frames to add
            while (frame != null)
            {
                frames.Add(frame);
                frame = resourceManager.GetTexture(textureName + "_" + i);
                i++;
            }

            if (frames.Count == 0)
            {
                Console.WriteLine("No frames were found with name: " + textureName);
            }
            //Loads the franes into the animation
            SpriteAnimationFrames animationFrames = new SpriteAnimationFrames();
            animationFrames.frames = frames.ToArray();
            animation.LoadFrames(animationFrames);
            return animation;
        }
        
        /// <summary>
        /// Updates the state machine
        /// </summary>
        /// <param name="gametime"></param>
        public void Update(GameTime gametime)
        {
            ASMachine.Update(gametime);
            AnimationFrame frame = GetCurrentFrame();
            //if there is not frame to draw -> set the visibility to false
            if (frame == null)
            {
                isVisible = false;
                return;
            }
            //Sets the current frame variable to be used to draw on the screen
            texture = frame.Texture;
            drawArea = frame.DrawArea;

            //Sets the rotation of the sprite accordingly to the orientation of the entity
            if (Owner.orientation.X < 0)
            {
                rotationAngle = (float)Math.Abs(Math.Atan2(Owner.orientation.X, Owner.orientation.Y));
            }
            else
            {
                rotationAngle = (float) -Math.Atan2(Owner.orientation.X, Owner.orientation.Y);
            }
                        
            origin = frame.origin;           
        }

        public void Draw(SpriteBatch spriteBatch)
        {
            position = Owner.position;
            if (isVisible)
            {
                spriteBatch.Draw(texture, position, drawArea, color, rotationAngle, origin, 1.0f, SpriteEffects.None, 0f);
            }
                    
        }

        /// <summary>
        /// Call the state machine and stes current state to the param specified
        /// </summary>
        /// <param name="stateName">Name of the state to transition to</param>
        public void SetState(string stateName)
        {
            ASMachine.TransitionState(stateName);
        }

        /// <summary>
        /// Returns the current texture of the animation from the current state active
        /// Checks if there is a cuurent active state.
        /// Returns null if no state is found
        /// </summary>
        /// <returns>Texture2D reference </returns>
        public AnimationFrame GetCurrentFrame()
        {            
            if(ASMachine.currentState != null)
            {
                return ASMachine.currentState.GetCurrentFrame();
            }else
            {
                return null;
            }
            
        }

        /// <summary>
        /// Sets the color of the animation texture
        /// </summary>
        public void SetColor(Color _color)
        {
            color = _color;
        }

        /// <summary>
        /// Gets the current draw area of the current frame
        /// </summary>
        /// <returns></returns>
        public Rectangle GetDrawArea()
        {
            return GetCurrentFrame().DrawArea;
        }

        /// <summary>
        /// Toogle the drawing of the animation on and off
        /// </summary>
        public void SetVisibility(bool _isVisible)
        {
            isVisible = _isVisible;
        }
    }
}
