# Student-Projects
Source code of projects developed while studying Games and Apps Development.

The projects contained in this repository are projects where I was the main programmer, most of the code has been written by me while learning the languages and the technologies.
 
List of projects:
- Portal Seekers Game client (Full Source);
- Portal Seekers Matchmaking Server (Full Source);
- Generic Game Engine (Full Source).


## Portal Seekers Game Client (Full Source)

**Genre**: Online multiplayer asymmetric action game. 

**Technologies**: Devoloped in Unreal and C++. 

**Date**: Sep 2017 - Jan 2018 

**Team size**: 2 

**Game Description**: Portal Seekers can be played between two players (1vs1) and up to five (1vs4) where one
player controls a Guardian and the rest of the players control Seekers. The game is set in a maze-like and dense forest, the objective of the seekers is to escape the
forest. For that, they will need to use the portal. To open the portal, the seekers must charge and
activate the four pillars scattered across the forest. The pillars will need energy to be charged, for
that the seekers will have to bring orbs that spawn randomly across the map and take them to the
pillars. The Guardian’s objective is to prevent the Seekers from opening the portal and escape the
forest.

**My role**: I was responsible for all the network communication. P2P model with UDP connections for gameplay and client-server with TCP connection for matchmaking.


## Portal Seekers Matchmaking Server (Full Source)

**Genre**: Matchmaking server for the game Portal Seekers. 

**Technologies**: Devoloped in C++. 

**Date**: Sep 2017 - Jan 2018 

**Team size**: 1

**Description**:  Provides matchmaking services to support the initial connections between the players. 
The connection with the server remains active. In case the players cannot connect directly to each other,
due, for instance, of NAT configurations, the server provides a relay system.
Communication with the server is made in TCP for matchmaking and UDP for all gameplay messages.

**My role**: I am responsible for all the code. 


## Generic Game Engine (Full Source)

**Genre**: Entity Component Game Engine 

**Technologies**: Devoloped in Monogame and C#. 

**Date**: Jan 2018 - Present 

**Team size**: 1

**Description**: Currently in Development 

