#pragma once

class Singleton{
	
public:
	~Singleton()=default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	
	static Singleton& getInstance(){
		static Singleton instance;
		return instance;
	}

private:
	Singleton()=default;	
};