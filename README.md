# Touhou 6 - Embodiment of Scarlet Devil
- Video: [to be added]

# Introduction
A remake of Touhou 6 - Embodiment of Scarlet Devil using the SDL2 library, written in C++. Touhou is a bullet hell shoot 'em up video game series, where you shoot girl boss and evade their crazy-like abilities.

# 0. How to download

## 0.a Pre-compiled binary

## 0.b Compile from source code

# 1. How to start

Start by open the touhou.exe file. A window will pop-up, press enter to continue to the menu.
![image](https://github.com/shoraaa/touhou/assets/24241275/2c6dda76-5a04-4503-a9ea-77b65cad1cff)

Press enter to start the game.
![image](https://github.com/shoraaa/touhou/assets/24241275/2f0a9adb-f462-4c3f-a201-334fc5f2b93d)

# 2. Game mechanic
## 2.a Player 

### Movement
Control the player by using the Up/Down/Right/Left key. 
Slow down by using the Shift key.

### Shoot
Shoot the enemies by pressing the Z key. 

### Bomb
When you feel stuck, you can use the X key to spend 1 of your bomb, clearing every enemy bullet on-screen.

### Lives
Getting hit by your enemies's bullets or their body will cause your character to lose 1 health, and temporary being invicible for 5 secs, lose all your score and clearing every bullet on-screen.

### Power
Power-up your character by collecting the red item drop from enemy. There are 5 stages of power, each reached when player have at least 1/10/20/35/60 power. Dying will decrease your power by 10.

### Chi 
Increase your character by collecting the blue item drop from enemy. Every 100 chi equal to 1 live.

## 2.b Enemy

### Regular enemy
Regular enemies is small, weak entities that have low hp, and can be easily killed. However as the game progress, they will spawn faster and stronger.

### Boss
Once you reach specific stage, a boss will spawn. They have a lots of health, different movement and tons of crazy abilities. Prepare yourself by collecting as much power and chi as possible.

## 2.c Bullet (Particle)
Enemy will try to shoot bullet at you. Dodge them!

### Boss ability
Boss have different abilities that shoot bullet with different pattern. 

## 2.d Score
### Hitting/Killing enemy
Hitting and killing enemy will increase your score. Don't focus on just dodging, shoot more!

### Grazing
Graze is the concept of being as close to enemy bullet as possible, without hitting them. Grazing will reward you a lot of score, so try to do it without getting your player die!

### Dying
Losing lives will decrease player score, each decrease 10000 score. Losing the game give you choice to continue, but your score will be reset to 0.



