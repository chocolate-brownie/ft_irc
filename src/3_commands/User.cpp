#include "../../includes/User.hpp"

User::User(int fd) : _userSocketFd(fd), _isOperator(false), _isRegistered(false) {}

User::~User() {}

bool	User::isOperator() const
{
	return (_isOperator);
}

bool	User::isRegistered() const
{
	return (_isRegistered);
}

void	User::setOperator(bool val)
{
	_isOperator = val;
}


void	User::setRegistered(bool val)
{
	_isRegistered = val;
}