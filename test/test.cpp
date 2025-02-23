#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <string>
// using namespace std;


int main(){
	std::printf("aa\n");
	std::string aa="aa";
	aa.append("azxc");
	std::printf("aa:%s\n",aa.c_str());
	std::cout<<"aaa"<<std::endl;
	return 0;
}