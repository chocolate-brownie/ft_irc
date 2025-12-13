#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <arpa/inet.h>
#include <cstdio>   // IWYU pragma: keep
#include <cstdlib>  // IWYU pragma: keep
#include <cstring>  // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sstream> // IWYU pragma: keep
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector> // IWYU pragma: keep

#include "User.hpp"

class Channel 
{
	private:
		std::string				_name;
 		std::string				_topic;
		std::string				_key;
		std::vector<User>		_users;
		int						_userlimit;

		bool					_key_mode;
		bool					_invite_mode;
		bool					_topic_mode;
		bool					_limit_mode;

  		Channel(const Channel &other);
  		Channel &operator=(const Channel &other);
	public:
  		Channel(std::string name);
  		~Channel();
};


#endif
