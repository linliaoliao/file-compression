#pragma once
#include<assert.h>
#include<string>
#include<algorithm> 
//#define CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES

#define __DEBUG__

#include"HuffmanTree.h"

struct CharInfo
{
	char _ch;//存的字符
	long long _count;//某字符出现的次数
	string _code;//编码

	bool operator !=(const CharInfo& info)
	{
		return info._count != _count;
	}
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}
	bool operator <(const CharInfo& info)
	{
		return _count < info._count;
	}
};


class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
	//内部类    受访问限定符限制
	struct ConfigInfo//解压缩配置
	{
		char _ch;
		size_t _count;
	};

public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; ++i)//字符个数是确定的，宏可用可不用
		{
			_infos[i]._ch = i;
			_infos[i]._count = 0;
		}
	}

	//input.txt->input.txt.huffman

	void Compress(const char*file)
	{
		assert(file);

		//1.统计字符出现的次数
		FILE*fout = fopen(file, "rb");
		//if (fout == NULL)//判断文件是否为空 或者文件是否正确
		//	perror("Error opening file");//文件为空输出错误信息
		char ch = fgetc(fout);
		while (!feof(fout))
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}

		//2.构建huffman tree
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);

		//3.生成Huffman code（Huffman编码）
		string code;
		GetHuffmanCode(tree.GetRoot(), code);

		//4.压缩
		string compressFile = file;
		long long count = 0;
		compressFile += ".huffman";
		FILE*fin = fopen(compressFile.c_str(), "wb");//打开文件
		assert(fin);

		//写字符出现的次数到压缩文件---解压缩时候需要重建Huffman tree
		char arr[128];//arr存储整型转为字符串后的内容
		ConfigInfo info;
		for (size_t i = 0; i < 256; ++i)
		{
			if (_infos[i]._count >0)
			{
				//二进制形式写字符串

				info._ch = _infos[i]._ch;
				info._count = _infos[i]._count;
				fwrite(&info, sizeof(ConfigInfo), 1, fin);//


				////文本的方式写字符串   一个字符+次数
				//fputc(_infos[i]._ch, fin);
				//fputc(' ', fin);
				////整型转换成字符串
				//_itoa(_infos[i]._count, arr, 10);
				//fputs(arr, fin);
				//fputc('\n', fin);
			}
		}
		info._count = 0;//在最后加上一个0，用来分隔
		fwrite(&info, sizeof(ConfigInfo), 1, fin);

		fseek(fout, 0, SEEK_SET);//文件指针
		ch = fgetc(fout);//1.先关掉文件再打开文件  2.或者把文件指针设置到文件开始
		char value = 0;//表示值
		int pos = 0;//表示位
		//while (ch != EOF)//文件结束符，当文件指针到文件结束时，其值为非零值，否则为零。
		while (!feof(fout))
		{
			//...如何压缩？
			//满8位就写入一次...
			string& code = _infos[(unsigned char)ch]._code;//重要的是取字符的编码
			for (size_t i = 0; i < code.size(); ++i)//循环处理满8位就写入一次
			{
				
				if (code[i] == '0')
				{
					value &= ~(1 << pos);
				}
				else if (code[i] == '1')
				{
					value |= (1 << pos);
				}
				else
				{
					assert(false);
				}
				++pos;
				if (pos == 8)
				{
					count++;
					fputc(value, fin);
					value = 0;
					pos = 0;
				}
			}
			ch = fgetc(fout);
		}
		cout << "压缩字符数：" << count << endl;
		//补齐
		if (pos > 0)//表示还有位，补齐多余的位
		{
			fputc(value, fin);
		}
		fclose(fout);
		fclose(fin);
	}

	//input.txt.huffman->input.txt解压缩

	void GetHuffmanCode(Node* root)
	{
		if (root == NULL)
			return;
		if (root->_left == NULL
			&& root->_right == NULL)
		{
			//没有创建对象,没有拷贝，可读性变高
			string& code = _infos[(unsigned char)root->_w._ch]._code;

			Node* cur = root;
			Node* parent = cur->_parent;

			while (parent)//倒着走把编码存到code中
			{
				if (cur == parent->_left)
				{
					code.push_back('0');  //[]用来修改,但不能做插入
				}
				else
				{
					code.push_back('1');
				}
				cur = parent;
				parent = cur->_parent;
			}
			reverse(code.begin(), code.end());//迭代器code.begin()返回char*

			//字符的ASCII码//将编码存入到叶子结点的权值的字符中
			//_infos[root->_w._ch]._code = code;`
			//return;
		}
		GetHuffmanCode(root->_left);
		GetHuffmanCode(root->_right);
	}
	void GetHuffmanCode(Node* root, string code)//向左边走code+0,右边走code+1
	{
		if (root == NULL)
			return;
		if (root->_left == NULL
			&& root->_right == NULL)
		{
			//叶子
			//强转成有符号数，因为汉字的编码是负数编码，且一个汉字占两个字节
			_infos[(unsigned char)root->_w._ch]._code = code;
			return;
		}
		GetHuffmanCode(root->_left, code + '0');//不可以用pushback
		GetHuffmanCode(root->_right,code + '1');
	}

	//input.txt.huffman -> input.txt
	void UnCompress(const char*file)
	{
		assert(file);
		string uncompressFile = file;
		size_t pos = uncompressFile.rfind('.');//找'.'
		assert(pos != string::npos);
		uncompressFile.erase(pos, uncompressFile.size()-pos);

#ifdef __DEBUG__ //使用宏可以更好的将源文件和解压缩后的文件区分开，也便于调试
		uncompressFile += ".unhuffman";//解压缩后的生成文件
#endif


		FILE*fout = fopen(file, "rb");
		assert(fout);
		FILE*fin = fopen(uncompressFile.c_str(), "wb");
		assert(fin);

		//先读入配置信息---字符出现的次数
		ConfigInfo info;
		while (1)
		{
			fread(&info, sizeof(ConfigInfo), 1, fout);
			if (info._count == 0)
			{
				break;
			}
			else
			{
				_infos[(unsigned char)info._ch]._ch = info._ch;
				_infos[(unsigned char)info._ch]._count= info._count;
			}
		}
		

		//解压缩

		//1.重建huffman tree
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		Node*root = tree.GetRoot();
		Node*cur = root;
		long long fileSize = root->_w._count;

		//2.解压缩
		char value = fgetc(fout);
		//fseek(fout, 0, SEEK_SET);
		long long count = 0;
		//while (value != EOF)
		while (!feof(fout))
		{
			count++;
			for (int pos = 0; pos < 8; ++pos)
			{
				
				if (value & (1 << pos))//pos为1
					cur = cur->_right;
				else
					cur = cur->_left;

				if (cur->_left == NULL
					&& cur->_right == NULL)
				{
					fputc(cur->_w._ch,fin);
					cur = root;//还原一个字符
					
					if (--fileSize == 0)
					{
						break;//只有最后一个字符出来(结束)
					}
				}
			}
			value = fgetc(fout);
		}
		cout << "解压缩字符数：" << count << endl;
	}
private:
	CharInfo _infos[256];//在数组中找编码
};


void TestFileCompress()
{
	//FileCompress fc;
	////fc.Compress("input.txt");
	//fc.UnCompress("input.txt.huffman");

	FileCompress fc1;
	fc1.Compress("Taylor Henderson - Let Her Go.mp3");
	FileCompress fc2;
	fc2.UnCompress("Taylor Henderson - Let Her Go.mp3.huffman");
}
