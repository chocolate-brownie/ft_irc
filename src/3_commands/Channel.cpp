#include "../../includes/Channel.hpp"


Channel::Channel(std::string name) : _name(name), _userlimit(0)
{
	_key_mode = false;
	_invite_mode = false;
	_topic_mode = false;
	_limit_mode = false;
}

Channel::~Channel() {}

