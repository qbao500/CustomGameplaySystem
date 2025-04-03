# CustomGameplaySystem

This plugin is mostly my learning material about the Gameplay Ability System (GAS), Asset Manager, Game Features, Network Replication, and many more. It is built in Unreal Engine 5.4 and may get updated to 5.5 or later in the future. 

At this moment, the plugin is being updated every now and then. This README acts as an introduction, and I don't have any plans to write detailed documentation for it.

The goal of this is to gain more knowledge about Unreal Engine frameworks, especially the above ones. Moreover, this is a great chance to improve my skills in architecting systems so they can be modular and reusable as much as possible. 
Some of the codes are "inspired" by the Lyra project.

# Modules

This plugin consists of multiple modules (maybe more in the future) that I will briefly go through.

## 1. CustomGameplaySystem

The main module of this plugin and my ideology. Besides the boilerplate and extended framework of GAS, there are additional features that may or may not fit certain games, such as Hitbox, Level & Exp, Customized Projectile Component, Save/Load System, etc.

![image](https://github.com/user-attachments/assets/c92bce34-d993-49b9-a440-1803f831bb73)

## 2. CustomGameplayLibrary

A list of function libraries to help coding faster with more efficiency. Many of them are also exposed for Blueprint usage.

![image](https://github.com/user-attachments/assets/5da9640a-91ab-4055-b530-2e53c3884d91)

## 3. ModularGameplayActors

"Borrowed" from Lyra, a good list of based Actors that support both Modular Gameplay and Game Features plugin.

![image](https://github.com/user-attachments/assets/8a824216-52cd-402b-8544-39d32b87ff81)

## 4. CustomGameplaySystemEditor

The main Editor module of this plugin. Currently, it checks certain settings if they have proper values. If not, it will give prompts to fix them.

![image](https://github.com/user-attachments/assets/664077e0-cb6c-4699-9093-447613fd2000)

# TODO

 - Object Pooling
 - Component Visualizers
 - (to be updated)
