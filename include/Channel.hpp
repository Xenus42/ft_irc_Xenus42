#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <map>
# include <set>
# include <string>
# include <vector>

enum	ChannelModeFlags
{
	MODE_INVITE_ONLY = 1 << 0,      // +i
	MODE_TOPIC_RESTRICTED = 1 << 1, // +t
	MODE_HAS_KEY = 1 << 2,          // +k
	MODE_HAS_LIMIT = 1 << 3         // +l
};
enum	ClientChannelStatus
{
	STATUS_MEMBER = 1 << 0,
	STATUS_OPERATOR = 1 << 1
};

class	Client;
class	Server;

class Channel
{
  public:
	Channel();
	Channel(const std::string &name);
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);
	~Channel();
	// === Member management ===
	void addClient(Client *client);
	void removeClient(Client *client);
	bool isMember(Client *client) const;

	// === Operator management ===
	void addOperator(Client *client);
	void removeOperator(Client *client);
	bool isOperator(Client *client) const;

	// === Invitations ===
	void addInvited(Client *client);
	bool isInvited(Client *client) const;
	void removeInvited(Client *client);

	// === Topic ===
	void setTopic(const std::string &topic);
	const std::string &getTopic() const;

	// === Key ===
	void setKey(const std::string &key);
	const std::string &getKey() const;
	void removeKey();

	// === User limit ===
	void setUserLimit(int limit);
	int getUserLimit() const;
	void removeUserLimit();

	// === Modes ===
	void setMode(int flag);
	void unsetMode(int flag);
	bool hasMode(int flag) const;

	// === Utils ===
	const std::string &getName() const;
	size_t getClientCount() const;
	bool isEmpty() const;
	void broadcast(Server &server, const std::string &message,
		Client *exclude = NULL);
	std::string getMemberList() const;
	std::vector<Client *> getMembers() const;

  private:
	bool hasStatus(Client *client, int flag) const;
	void setStatus(Client *client, int flag);
	void unsetStatus(Client *client, int flag);

	std::string _name;
	std::string _topic;
	std::string _key;
	int _userLimit;
	int _modes;
	std::map<Client *, int> _status;
	std::set<Client *> _invited;
};

#endif
