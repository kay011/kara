#pragma once

class Singleton{
	
public:
	~Singleton()=default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	// 局部静态变量
	static Singleton& getInstance(){
		static Singleton instance;
		return instance;
	}

private:
	Singleton()=default;	
};