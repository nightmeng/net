#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <mutex>
#include <memory>

template<typename type>
class singleton{
	public:
		static singleton &instance(){
			if(nullptr == _instance){
				static std::mutex mutex;
				std::lock_guard<std::mutex> locker(mutex);
				if(nullptr == _instance){
					_instance = std::shared_ptr<singleton>(new singleton);
				}
			}
			return *_instance;
		}

		type *operator->(){
			return &object;
		}
		const type *operator->()const{
			return &object;
		}
	private:
		singleton(){}
		singleton(const singleton&) = delete;
		singleton &operator=(const singleton&) = delete;
	private:
		static std::shared_ptr<singleton> _instance;
		type object;
};

template <typename type>
std::shared_ptr<singleton<type>> singleton<type>::_instance = nullptr;

#endif
