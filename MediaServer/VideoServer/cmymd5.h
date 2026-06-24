#ifndef CMYMD5_H
#define CMYMD5_H

#include<iostream>

using namespace std;
#include<string.h>
#include<cryptopp/md5.h>
#include<cryptopp/hex.h>
typedef unsigned char byte;
string getMD5(byte* buf,int len);
string getFileMD5(string path);
//class CMyMD5
//{
//public:
//    CMyMD5();
//};

#endif // CMYMD5_H
