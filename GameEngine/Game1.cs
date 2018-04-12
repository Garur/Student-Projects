using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using GameEngine.Entities;
using GameEngine.Scene;
using GameEngine.Resources;
using GameEngine.Collisions;
using GameEngine.Input;
using GameEngine.Components;
using GameEngine.Rendering;

using GameEngine.Game_2;
using System;
using GameEngine.Beetles;

namespace GameEngine
{
    /// <summary>
    /// This is the main type for your game.
    /// </summary>
    public class Game1 : Game
    {
        private static Game1 instance;
        public static Game1 Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new Game1();
                }
                return instance;
            }
        }

        //States if the game should be displayed in full screen or not
        bool isFullScreen = false;
        public static int ScreenWidth;
        public static int ScreenHeight;  
        //Random object to be used throughtout the game      
        public static Random random = new Random();

        //Services
        GraphicsDeviceManager graphics;
        SpriteBatch spriteBatch;
        MouseInput mouseInput; 
        IEntityManager entityManager;
        IRenderer renderer;
        ICollisionManager collisionManager;
        IComponentMaster componentMaster;
        IGameMode gameMode;
        ServiceLocator services;

        //Reference to the background texture
        Texture2D backgroundTex;

        private Game1()
        {
            graphics = new GraphicsDeviceManager(this);
            graphics.IsFullScreen = isFullScreen;

            if (isFullScreen)
            {
                Window.IsBorderless = true;
                Window.Position = new Point(0, 0);
            }
            

            mouseInput = new MouseInput();
            Content.RootDirectory = "Content";

            //Initialize all the services
            services = ServiceLocator.Instance;
            entityManager = services.StartService<EntityManager>();
            collisionManager = services.StartService<CollisionManager>();
            renderer = services.StartService<Renderer>();
            componentMaster = services.StartService<ComponentMaster>();
            services.StartService<SceneManager>();            
            services.StartService<ResourceManager>();

        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            if (isFullScreen)
            {  
                //Set Fullscreen
                graphics.PreferredBackBufferWidth = GraphicsDevice.DisplayMode.Width;
                graphics.PreferredBackBufferHeight = GraphicsDevice.DisplayMode.Height;
                graphics.ApplyChanges();
                //Set Screen width and height 
                ScreenWidth = GraphicsDevice.DisplayMode.Width;
                ScreenHeight = GraphicsDevice.DisplayMode.Height;
            }
            else
            {
                graphics.PreferredBackBufferWidth = 1200;
                graphics.PreferredBackBufferHeight = 600;
                graphics.ApplyChanges();
                ScreenWidth = graphics.PreferredBackBufferWidth;
                ScreenHeight = graphics.PreferredBackBufferHeight;
            }
          
           

            //Initialize Managers
            spriteBatch = new SpriteBatch(GraphicsDevice);
            services.GetService<IResourceManager>().Initialize(Content);
            services.GetService<IEntityManager>().Initialize();
            services.GetService<ISceneManager>().Initialize();
            services.GetService<IComponentMaster>().Initialize();
            services.GetService<IRenderer>().Initialize();
            //Setup controls inputs for all the players
            InputSetup.SetupInputs();

            //Set the current gameMode
            gameMode = new BeetlesGameMode();

           

            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            gameMode.LoadResources(services.GetService<IResourceManager>());            
            gameMode.Initialize();
        }
        

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// game-specific content.
        /// </summary>
        protected override void UnloadContent()
        {
            services.GetService<IResourceManager>().UnloadAll();
        }       

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed || Keyboard.GetState().IsKeyDown(Keys.Escape))
                Exit();

            ScreenWidth = GraphicsDevice.Viewport.Width;
            ScreenHeight = GraphicsDevice.Viewport.Height;

            mouseInput.Update();                        
            entityManager.Update(gameTime);
            collisionManager.Update();
            componentMaster.Update(gameTime);
            gameMode.Update(gameTime);

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);
            //Draw Background
            spriteBatch.Begin(SpriteSortMode.Deferred, null, SamplerState.LinearWrap, null, null);
            spriteBatch.Draw(backgroundTex, new Vector2(0, 0), new Rectangle(0, 0, GraphicsDevice.Viewport.Width, GraphicsDevice.Viewport.Height), Color.White, 0, Vector2.Zero, 1f, SpriteEffects.None, 0);
            spriteBatch.End();

            spriteBatch.Begin();
            renderer.Draw(spriteBatch);
            spriteBatch.End();

            base.Draw(gameTime);
        }

        public void SetBackground(Texture2D background)
        {
            backgroundTex = background;
        }
    }
}
