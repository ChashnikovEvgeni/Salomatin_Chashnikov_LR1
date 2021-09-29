#pragma once

class Session
{
public:
	int id;
	string name;
	queue<Message> messages;
	clock_t  time;
	CCriticalSection cs;
	Session(int _id, string _name, clock_t _time)
		:id(_id), name(_name), time(_time)
	{
	}

	void add(Message& m)
	{
		CSingleLock lock(&cs, TRUE);
		messages.push(m);
	}

	void send(CSocket& s)
	{
		CSingleLock lock(&cs, TRUE);
		if (messages.empty())
		{
			Message::send(s, id, MR_BROKER, MT_NODATA);
		}
		else
		{
			messages.front().send(s);
			messages.pop();
		}
	}
};

