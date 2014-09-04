irc-web-bot
===========

Web server allowing browser chat clients to connect to a certain irc channel via a channel bot.

How to use it:
--------------
In server sub-directory:
- edit the config.ini file to fit your needs
- make the project using the Makefile
- start the server ("./wsServer" on a linux command line)
Now the server is started, connected to the irc-channel and listens for incomming websocket connections on wsServerPort

You can then start a web client by opening the chat.html from the main directory in a browser that supports websockets.
If you change the configuration, you have to remake it using "make config".
