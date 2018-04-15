using GameEngine.Entities;
using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GameEngine.Components
{
    interface IState 
    {
        IEntity Owner { get; set; }
        CStateMachine SMachine { get; set; }
                
        void Initialize(); //Called on the state is loaded
        void OnEnter(); //Called on the state is activated 
        void Update(GameTime gameTime); //Called every tick on the state is active
        void OnExit(); //Called when the state stops being active
    }
}
