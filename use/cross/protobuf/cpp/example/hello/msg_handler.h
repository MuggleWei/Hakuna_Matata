#ifndef MSG_HANDLER_H_
#define MSG_HANDLER_H_

#include <map>
#include <memory>
#include <functional>
#include <google/protobuf/message.h>

class ProtoMessageHandler
{
	class MsgCallback
	{
	public:
		virtual void OnMessage(std::shared_ptr<google::protobuf::Message> &msg) {}
	};

	template<typename T>
	class TMsgCallback : public MsgCallback
	{
		typedef std::function<void(std::shared_ptr<T>&)> callbackFunc;

	public:
		TMsgCallback(const callbackFunc& callback)
			: MsgCallback()
			, callback_(callback)
		{}

		virtual void OnMessage(std::shared_ptr<google::protobuf::Message> &msg)
		{
			std::shared_ptr<T> derived_msg = std::dynamic_pointer_cast<T>(msg);
			if (derived_msg && callback_)
			{
				callback_(derived_msg);
			}
		}

	private:
		callbackFunc callback_;
	};


public:
	ProtoMessageHandler()
		: default_callback_(nullptr)
	{}

	template<typename T>
	void RegisterDefault(const std::function<void(std::shared_ptr<T>&)>& callback)
	{
		if (default_callback_)
		{
			delete(default_callback_);
		}
		default_callback_ = new TMsgCallback<T>(callback);
	}

	template<typename T>
	void Register(const std::function<void(std::shared_ptr<T>&)>& callback)
	{
		TMsgCallback<T> *tcallback = new TMsgCallback<T>(callback);
		std::string msg_type_name = T::descriptor()->full_name();
		callbacks_.insert(std::pair<std::string, MsgCallback*>(msg_type_name, tcallback));
	}

	template<typename T>
	void OnMessage(std::shared_ptr<T>& msg)
	{
		auto it = callbacks_.find(msg->GetTypeName());
		if (it != callbacks_.end())
		{
			it->second->OnMessage(msg);
		}
		else
		{
			if (default_callback_)
			{
				default_callback_->OnMessage(msg);
			}
		}
	}

private:
	std::map<std::string, MsgCallback*> callbacks_;
	MsgCallback *default_callback_;
};

#endif
