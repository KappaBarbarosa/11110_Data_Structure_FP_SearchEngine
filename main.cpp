#define FILE_EXTENSION ".txt"
#include <fstream>
#include <set>
#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <map>
#include <queue>
#include <algorithm>
#include <chrono>
// #include <unistd.h>
int  c, j;
using namespace std;
class ACNode
{
public:
	char character;
	ACNode *children[26];
	ACNode *fail;
	bool isEOF;
	ACNode(char c) : character(c), fail(nullptr)
	{
		for (int i = 0; i < 26; i++)
			children[i] = NULL;
		isEOF = false;
	}
};

class ACAutomaton
{
private:
	ACNode *root;
	ACNode *suffixroot;

public:
	ACAutomaton() : root(new ACNode('\0')), suffixroot(new ACNode('\0')) {}
	int index(char c) { return c >= 'a' ? c - 'a' : c - 'A'; }
	void insert(char *pattern, bool issuffix)
	{
		int len = strlen(pattern);
		// printf("s = %s len = %d ",pattern, len);
		if (issuffix)
		{
			ACNode *curr = suffixroot;
			for (int k = len - 1; k >= 0; k--)
			{
				int c = index(pattern[k]);
				if (!curr->children[c])
					curr->children[c] = new ACNode('a' + c);
				curr = curr->children[c];
			}
			curr->isEOF = true;
		}
		else
		{
			ACNode *curr = root;
			for (int k = 0; k < len; k++)
			{
				int c = index(pattern[k]);

				if (!curr->children[c])
					curr->children[c] = new ACNode('a' + c);
				curr = curr->children[c];
			}
			curr->isEOF = true;
		}
	}
	bool search(const char *text)
	{
		ACNode *curr = root;
		int len = strlen(text);
		//printf("\nsearch");
		for (int i = 1; i < len - 1; i++)
		{
			if (!isalpha(text[i])) continue;
 
			int c = index(text[i]);
			if (curr->children[c]) // 如果有找到字
			{
				curr = curr->children[c];
			}
			else
				return false;
		}
		if (curr->isEOF) // 為底
			return true;
		return false;
	}
	bool search_prefix(const char *prefix, bool issuffix)
	{
		int len = strlen(prefix);
		//printf("\nsearh ");
		if (issuffix)
		{
			ACNode *curr = suffixroot;
			for (int i = len - 1; i >= 1; i--)
			{
				//printf("want find: %c !\n",prefix[i]);
				if (!isalpha(prefix[i])) continue;
				//printf("%c",prefix[i]);
				int c = index(prefix[i]);
				if (!curr->children[c])
					return false;
				curr = curr->children[c];
			}
			return true;
		}
		else
		{
			ACNode *curr = root;
			for (int i = 0; i < len; i++)
			{
				//printf("want find: %c !\n",prefix[i]);
				if (!isalpha(prefix[i])) continue;
				//printf("%c",prefix[i]);
				int c = index(prefix[i]);
				if (!curr->children[c])
					return false;
				curr = curr->children[c];
				//printf("found: %c !\n",prefix[i]);
			}
			return true;
		}
	}

	bool Serach(const char *text)
	{
		if (text[0] == '"')
		{
			return search(text);
		}
		else if (text[0] == '*')
		{
			return search_prefix(text, true);
		}
		else
			return search_prefix(text, false);
	}
};

// string parser : output vector of strings (words) after parsing
map <string, vector<int>> dp;
vector<char *> word_parse(vector<char *> tmp_string, ACAutomaton *co)
{
	vector<char *> parse_string;
	for (auto &word : tmp_string)
	{
		char  str[128] = {'\0'};
		int len = strlen(word);
		int ncount = 0;
		for (int i = 0; i < len; i++)
		{
			if (isalpha(word[i]))
				 str[ncount++] = word[i];
		}
		parse_string.emplace_back( str);
		co->insert( str, false);
		co->insert( str, true);
	}
	return parse_string;
}
vector<char *> split(char *str, const string &delim)
{
	vector<char *> res;
	if ("" == str)
		return res;
	// 先將要切割的字串從string型別轉換為char*型別
	char *s = (char *)malloc(sizeof(char) * 1024);
	strcpy(s, str);
	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());
	char *p = strtok(s, d);
	while (p)
	{
		res.push_back(p); // 存入結果陣列
		p = strtok(NULL, d);
	}
	return res;
}
char *Getword(FILE *fi)
{
	char *s = (char *)malloc(sizeof(char) *1024);
	if (fgets(s, 1024, fi) != NULL) return s;
	s[0] = '\0';
	return s;
}
int main(int argc, char *argv[])
{
	auto start = chrono::steady_clock::now();
	// ios_base::sync_with_stdio(false);
	// cin.tie(0);
	string data_dir = argv[1] + string("/");
	char ttmp[20];
	strcpy(ttmp, data_dir.c_str());
	string query = string(argv[2]);
	string output = string(argv[3]);
	string file, title_name, tmp;
	vector<char *> tmp_string;
	int ii = 0;
	vector<ACAutomaton *> ac;
	vector<char *> Title;
	vector<int> size;
	int flag;
	//fo.open(output);

	FILE *fp, *fc;
	char buffer[1024];
	while (1)
	{
		string str = to_string(ii) + ".txt";
		char tmp[100];
		strcpy(tmp, ttmp);
		strcat(tmp, str.c_str());

		fp = fopen(tmp, "r");
		if (fp == NULL)
			break;
		flag = 0;
		// GET CONTENT LINE BY LINE
		ACAutomaton *co = new ACAutomaton();
		char *Tmp;
		while (true)
		{
			Tmp = Getword(fp);
			if (Tmp[0] == '\0')
				break;
			if (!flag)
				Title.push_back(Tmp),size.push_back(strlen(Tmp)), flag = 1;
			tmp_string = split(Tmp, " ");
			vector<char *> content = word_parse(tmp_string, co);
		}
		ac.push_back(co);
		fclose(fp);
		ii++;
	}
	fp = fopen(query.c_str(), "r");
	fc = fopen(output.c_str(),"w");
	while (true)
	{
		char *Tmp = Getword(fp);
		//fo << Tmp;
		if (Tmp[0] == '\0')
			break;
		vector<int> ans(ii);
		tmp_string = split(Tmp, " ");

		for (int i = 0; i < tmp_string.size(); i++)
		{ // 一句query
			if (tmp_string[i][0] !='+' && tmp_string[i][0] !='/')
			{
				auto s = tmp_string[i];
				vector<int> co;
				if (dp.find(s) == dp.end())
				{
					for (j = 0; j < ii; j++)
					{
						if (ac[j]->Serach(s))
						{
							co.push_back(j);
							if (i == 0)
								ans[j] = 1;
							else if (tmp_string[i - 1][0] == '/')
								ans[j] = 1;
						}
						else
						{
							if (i == 0)
								ans[j] = 0;
							else if (tmp_string[i - 1][0] == '+')
								ans[j] = 0;
						}
					}
					if (!co.size())
						co.push_back(-1); // 代表查過但找不到
					dp[s] = co;
				}
				else
				{ 
					vector<int> &d = dp[s];
					c = 0;
					for (int j = 0; j < ii; j++)
					{
						if (i == 0)
						{ // 第一個字就直接插入
							if (d[c] == -1 || j != d[c])
								ans[j] = 0;
							else
								ans[j] = 1, c++;
						}
						else
						{
							if (tmp_string[i - 1][0] == '+')
							{
								if (j == d[c])
								{
									if (ans[j] == 0 || d[c] == -1)
										ans[j] = 0; // 如果前面在j沒有符合 或是這個字沒有找到
									c++;
								}
								else
									ans[j] = 0;
							}
							else
							{
								if (d[c] == -1)
									break; // 找不到就維持原樣
								if (j == d[c])
									ans[j] = 1, c++; // 有的再更新
							}
						}
					}
				}
			}
		}
		int ct = 0;
		for (int j = 0; j < ii; j++)
		{
			if (ans[j]) fwrite(Title[j], size[j] , 1, fc );
			else ct++;
		}
		if (ct == ii) fwrite("Not Found!\n" ,11, 1, fc );
	}
	fclose(fp);
	return 0;
}
