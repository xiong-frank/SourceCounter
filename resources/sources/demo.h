#pragma once

// define class
class user
{
    int age;
    std::string name;
    
    void setAge(int n /* = 10 */) { age = n; }
    void setName(const std::string& str)
    {
        // the size of "str" must be less 20
        name = str;        
    }
    
    void show() const
    { /*
       * print name
       */ std::cout << "name:" << name << std::endl; /*
       * print age 
       */ std::cout << "age(/*1-80*/):" << age << std::endl;
    }
    
};  // class user
