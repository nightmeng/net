#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__

class socket_base{
	public:
		explicit socket_base();
		explicit socket_base(int fd);
		virtual ~socket_base();

		int fd();

		virtual void ievent() = 0;
		virtual void oevent() = 0;
		virtual void eevent() = 0;

		bool set_noblock();
		
		socket_base(const socket_base &) = delete;
		socket_base& operator=(const socket_base &) = delete;
	protected:
		int sock;
		int events;
};

#endif
