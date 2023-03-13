#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <vector>

using namespace std;

class User {
	private:
		int _sd;
		bool _pass;
		bool _user;
		int registred;
		string _host;
		string _password;
		string _nickname;
		string _username;
		string _cmdBuffer;
		string _replyBuffer;
		bool _isQuiting;

	public:
		User(void);
		User(const User& user);
		User& operator=(const User& user);

        User(int fd, const string& host);
		~User();
		
		int getFd(void) const;
		const string& getHost(void) const;
		const string& getPassword(void) const;
		const string getNickname(void) const;
		const string getSource(void) const;
		const string& getUsername(void) const;
		bool getAuth(void) const;
		const string& getCmdBuffer(void) const;
		const string& getReplyBuffer(void) const;
		bool getIsQuiting(void) const;
		bool getIsUser(void) const;
		bool getIsPass(void) const;
		int  getRegistred(void) const;

		void setPassword(const string& pwd);
		void setNickname(const string& nickname);
		void setUsername(const string& username);
		void setAuth(void);
		void setIsQuiting(void);
		void setIsUser(void);
		void setIsPass(void);
		void setRegistred(void);

		void setCmdBuffer(const string& src);
		void clearCmdBuffer(void);
		void setReplyBuffer(const string& src);
		void clearReplyBuffer(void);
		void addToCmdBuffer(const string& src);
		void addToReplyBuffer(const string& src);

};

#endif
