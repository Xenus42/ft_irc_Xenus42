## Authentication / Registration



`PASS <password>` — Authenticate with the server password (required before anything else)
```cpp
requireArgsNum(args, 1, "PASS <password>");
requireWord(args, 0, "password");
```

`NICK <nickname>` — Set or change your nickname
```cpp
requireArgsNum(args, 1, "NICK <nickname>");
requireWord(args, 0, "nickname");
```

`USER <username>` — Set the username/realname
```cpp
requireArgsNum(args, 1, "USER <username>");
requireWord(args, 0, "username");
```
## Channel Interaction
`JOIN #<channel>` — Join a channel
`JOIN #<channel> <password>` — Join a password-protected channel
```cpp
requireArgsNum(args, 1, 2, "JOIN #<channel>\nJOIN #<channel> <password>");
requireChannel(args, 0, "channel name");
optionalWord(args, 1, "channel password");
```

`PRIVMSG #<channel> <message_w/out_spaces>` — Send a message without spaces to a channel
`PRIVMSG <othernick> <messages_w/out_space>` — Send a private without spaces message to a user
`PRIVMSG #<channel> :<message w spaces>` — Send a message with spaces to a channel
`PRIVMSG <othernick> :<messages w space>` — Send a private with spaces message to a user
```cpp
requireArgsNum(args, 2, "PRIVMSG #<channel> <message>\n"
"PRIVMSG #<channel> :<long message>\n"
"PRIVMSG <othernick> <message>\n"
"PRIVMSG <othernick> :<long message>"
);
try
{
    requireChannel(args, 0, "channel name");
}
catch (...)
{
    requireWord(args, 0, "other nickname");
}
requireStr(args, 1, "message");
```
## Channel Operator Commands
### These are only usable if you have operator privileges:

`KICK #<channel> <username>` — Eject a user from the channel
```cpp
requireArgsNum(args, 2, "KICK #<channel> <username>");
requireChannel(args, 0, "channel name");
requireWord(args, 1, "other username");
```

`INVITE <othernick> #<channel>` — Invite a user to the channel
```cpp
requireArgsNum(args, 2, "INVITE <othernick> #<channel>");
requireWord(args, 0, "other nickname");
requireChannel(args, 1, "channel name");
```

`TOPIC #<channel> <topic_w/out_spaces>` — Set the channel topic without spaces
`TOPIC #<channel> :<topic w spaces>` — Set the channel topic with spaces
`TOPIC #<channel>` — View the current channel topic
```cpp
requireArgsNum(args, 1, 2, "TOPIC #<channel> <topic>"
"TOPIC #<channel> <long topic>"
"TOPIC #<channel>");
requireChannel(args, 0, "channel name");
optionalParam(args, 1, "topic description");
```

`MODE #<channel> +i` — Set channel to invite-only
`MODE #<channel> -i` — Remove invite-only
`MODE #<channel> +t` — Restrict TOPIC to operators only
`MODE #<channel> -t` — Remove that restriction
`MODE #<channel> +k <password>` — Set a channel password
`MODE #<channel> -k` — Remove the channel password
`MODE #<channel> +o <othernick>` — Give operator privilege to a user
`MODE #<channel> -o <othernick>` — Remove operator privilege
`MODE #<channel> +l <positive_int>` — Set a user limit on the channel
`MODE #<channel> -l` — Remove the user limit
```cpp
requireArgsNum(args, 2, 3, "MODE #<channel> <topic>");
requireChannel(args, 0, "channel name");
requireMode(args, 1, "channel mode");

// +k
requireArgsNum(args, 3, "");
requireWord(args, 2, "channel password");
// +o -o
requireArgsNum(args, 3, "");
requireWord(args, 2, "other nickname");
// +l
requireArgsNum(args, 3, "");
requirePosInt(args, 2, "user limit");
```