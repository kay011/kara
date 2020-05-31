/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/5/28
 */

#pragma once

class noncopyable{
protected:
    noncopyable(){}
    ~noncopyable(){}

private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
    
};
