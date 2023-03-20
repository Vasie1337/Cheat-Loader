# Cheat Loader
Repository with 4-parts (Driver, Client, Server, Panel)

**Driver(Windows):**
It uses a .data pointer in "_NtUserCreateDesktopEx_" "_win32kbase.sys_" as communication, its probably detected since the pointer will is pointing to our driver wich is ouside any valid module. Other then that it has just 2 options reading memory and getting the base address of the target.

**Client(Windows):**
The client has a simple login simple, (**Client** <-> **Server** communcation is super shit) and maps the driver from bytes(**Kdmapper** Lol). The **Fortnite** external has only Esp for now more features to come maybe. 

**Server(Linux):**
Handles the **client** login, the keys, expiration and the panel creating/deleting keys. Code needs to be fixed & cleaned.

**Panel(Windows):**
Just create and delete keys.

This is just a small project I work on sometimes, and I know that there are a lot of improvements possible ;)
