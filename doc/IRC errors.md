## PASS

all replies: `:<servermame> <code> <nick> <...>`

| Code | Name | Message | Trigger |
|---|---|---|---|
| `464` | `ERR_PASSWDMISMATCH` | `:Password incorrect` | Wrong password |

***

## NICK

| Code | Name | Message | Trigger |
|---|---|---|---|
| `431` | `ERR_NONICKNAMEGIVEN` | `:No nickname given` | No parameter |
| `432` | `ERR_ERRONEUSNICKNAME` | `<extra-nick> :Erroneous nickname` | Invalid characters or length |
| `433` | `ERR_NICKNAMEINUSE` | `<extra-nick> :Nickname is already in use` | Nick already taken |

***

## USER

| Code | Name | Message | Trigger |
|---|---|---|---|
| `001` | `RPL_WELCOME` | `:Welcome to the IRC Network, <userprefix>` | Registration finalized |
| `002` | `RPL_YOURHOST` | `:Your host is <serverName>, running version 1.0` | Registration finalized |
| `003` | `RPL_CREATED` | `:This server was created <date>` | Registration finalized |
| `004` | `RPL_MYINFO` | `<serverName> 1.0 "" "itkol" ""` | Registration finalized |

***

## JOIN

| Code | Name | Message | Trigger |
|---|---|---|---|
| `471` | `ERR_CHANNELISFULL` | `<channel> :Cannot join channel (+l)` | User limit reached |
| `473` | `ERR_INVITEONLYCHAN` | `<channel> :Cannot join channel (+i)` | Channel is invite-only and client not invited |
| `475` | `ERR_BADCHANNELKEY` | `<channel> :Cannot join channel (+k)` | Wrong or missing key |

***

## PRIVMSG

| Code | Name | Message | Trigger |
|---|---|---|---|
| `411` | `ERR_NORECIPIENT` | `:No recipient given (PRIVMSG)` | No target specified |
| `412` | `ERR_NOTEXTTOSEND` | `:No text to send` | Empty message body |
| `404` | `ERR_CANNOTSENDTOCHAN` | `<channel> :Cannot send to channel` | Client not in channel |

***

## KICK

| Code | Name | Message | Trigger |
|---|---|---|---|
| `441` | `ERR_USERNOTINCHANNEL` | `<extra-nick> <channel> :They aren't on that channel` | Target not in channel |

***

## INVITE

| Code | Name | Message | Trigger |
|---|---|---|---|
| `443` | `ERR_USERONCHANNEL` | `<extra-nick> <channel> :is already on channel` | Target already in channel |
| `341` | `RPL_INVITING` | `<extra-nick> <channel>` | Sent to the inviting op |

***

## MODE

| Code | Name | Message | Trigger |
|---|---|---|---|
| `472` | `ERR_UNKNOWNMODE` | `<extra-modechar> :is unknown mode char to me` | Unrecognised mode flag |
| `324` | `RPL_CHANNELMODEIS` | `<channel> <modestring>` | Query with no params — returns current modes |

***

## Shared — sent by multiple commands

| Code | Name | Sent by |
|---|---|---|
| `462`	| `ERR_ALREADYREGISTERED`	| `:You may not reregister`				| PASS, USER |
| `482`	| `ERR_CHANOPRIVSNEEDED`	| `<channel>: You're not channel operator" | KICK, INVITE, TOPIC, MODE |
| `331` | `RPL_NOTOPIC`				| `<channel> :No topic is set` | JOIN, TOPIC |
| `332` | `RPL_TOPIC`				| `<channel> :<topic>` | JOIN, TOPIC |
| `353` | `RPL_NAMREPLY` | `<symbol> <channel> :<names>` | JOIN, NAMES |
| `366` | `RPL_ENDOFNAMES` |`<channel> :End of /NAMES list` | JOIN, NAMES |
| `401` | `ERR_NOSUCHNICK` | ` <nickname> :No such nick/channel` | PRIVMSG |

***

## One extra — always implement

| Code | Name | Message | Trigger |
|---|---|---|---|
| `451` | `ERR_NOTREGISTERED` | `:You have not registered` | Any command requiring registration sent before it's complete |

| `421` | `ERR_UNKNOWNCOMMAND` | `<command> :Unknown command` | Any unrecognised command from a registered client |
| `461` | `ERR_NEEDMOREPARAMS` | `<command> :Not enough parameters` | no enough params
