## 💬 ft_irc: Internet Relay Chat

**Summary:** This project is about creating your own IRC server. You will use an actual IRC client to connect to your server and test it.

---

### 📜 Chapter I: Introduction

Internet Relay Chat (IRC) is a text-based communication protocol. It offers real-time messaging in public channels or private messages. Clients connect to servers to join channels, and servers can connect to form a network.

---

### ⚖️ Chapter II: General Rules

* **No Crashes!** 💥 Your program must *never* crash or quit unexpectedly. If it does, your grade is 0.
* **Makefile:** 🛠️ You must submit a `Makefile` with `$(NAME)`, `all`, `clean`, `fclean`, and `re` rules. It must compile your code with `c++` and the flags `-Wall -Wextra -Werror`.
* **C++98:** 🕰️ Your code must comply with the C++98 standard.
* **Forbidden Libraries:** 🚫 Any external library (like Boost) is forbidden.
* **Language:** ⌨️ Always prefer C++ features (like `<cstring>`) over their C equivalents (`<string.h>`).

---

### 💻 Chapter IV: Mandatory Part

#### Program Details

* **Name:** `ircserv`
* **Arguments:** `./ircserv <port> <password>`
    * `<port>`: The port number for incoming connections.
    * `<password>`: The connection password required by any client.
* **Allowed Functions:** You must use C++98. A specific list of C-style functions (like `socket`, `bind`, `listen`, `accept`, `poll`, `send`, `recv`, `fcntl`, etc.) is permitted.

#### Core Requirements

1.  **Handle Multiple Clients:** 👥 The server must handle multiple clients at the same time without hanging.
2.  **No Forking:** ⛔ `fork()` is prohibited for handling clients.
3.  **Non-Blocking I/O:** ⚡ All I/O operations must be non-blocking.
4.  **One `poll()` to Rule Them All:** 👑 You must use **only one `poll()`** (or equivalent like `select()`/`kqueue()`) to handle *all* I/O operations (reading, writing, listening).
    * **Warning:** If you `read/recv` or `write/send` without `poll()` (or equivalent) checking readiness first, your grade will be 0.
5.  **Reference Client:** 🖥️ You must choose an official IRC client (like `irssi`, `LimeChat`, etc.) as your reference. It must be able to connect and work with your server flawlessly.
6.  **Communication:** Must be done via TCP/IP.

#### Features to Implement

Your server must allow a client to:
* 🔑 **Authenticate** (using the password)
* 🏷️ **Set a Nickname**
* 👤 **Set a Username**
* 🚪 **Join a Channel**
* ✉️ **Send and Receive Private Messages**

All messages sent to a channel must be forwarded to every other client in that channel.

#### Operator Commands

You must have **channel operators** and **regular users**. Operators must have access to these commands:
* `KICK`: Eject a client from the channel.
* `INVITE`: Invite a client to an invite-only channel.
* `TOPIC`: Change or view the channel topic.
* `MODE`: Change the channel's modes.
    * `i`: Set/remove **Invite-only** status.
    * `t`: Set/remove **Topic** protection (only ops can change it).
    * `k`: Set/remove the channel **Key** (password).
    * `o`: Give/take channel **Operator** privilege to another user.
    * `l`: Set/remove the **User limit** for the channel.

---

### 🌟 Chapter V: Bonus Part

If (and only if) your mandatory part is *perfect*, you can add bonus features:
* **File Transfer** 📁
* **A Bot** 🤖

---

### 🗳️ Chapter VI: Submission & Evaluation

Submit your work to your Git repo. You are encouraged to create your own test programs to help you. Be prepared for the evaluators to use your chosen reference client to test your server.