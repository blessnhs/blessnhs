// PacketGenerator.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

char *Array[] = {	"class",
					"int",
					"char",
					"unsigned char",
					"short int",
					"long",
					"long int",
					"unsinged int",
					"float",
					"double",
					"long double",
					"string",
					"list<",
					"map<",
					"vector<",
					"hash_map<",
					"DWORD",
					"WORD",
					"__int64",
				};

std::list<std::string>	varlist;

int FindWord(char *str,int size)
{
	//printf("str:[%s]size:[%d]\n\n",str,size);
	for(int i=0;i<19;i++)
	{
		for(int j=0;j<size;j++)
		{
			if(strcmp(str+j,Array[i]) == 0)
			{
				return i;
			}
		}
	}
	return -1;
}

void GetName(char *arg,char *output)
{
	int size = strlen(arg);
	bool begin = false;
	int j=0;
	for(int i=0;i<size;i++)
	{
		if(begin == true)
		{
			output[j++] = arg[i];
		}
		
		if(arg[i] == ' ')
		{
			begin = true;
		}
	}
	
	for(int j=0;j<255;j++)
	{
		if(output[j] == ';')
			output[j] = NULL;
	}
}

void MakeHeader(char *classname)
{
	FILE *fp = NULL;

	char filename[256];
	memset(filename,0,sizeof(filename));

	sprintf(filename,"%s.h",classname);

	fp = fopen(filename,"w");  


	if(fp == NULL)
	{
		printf("create file %s error\n",filename);
		return ;
	}

	fprintf(fp,"#pragma once\n");
	fprintf(fp,"\n");
	fprintf(fp,"#include \"stdafx.h\"\n\n",classname);

	fprintf(fp,"class %s;\n",classname);
	fprintf(fp,"SERIALIZE_DEFINE_TAG(%s, eTagUserClass);\n",classname);
	fprintf(fp,"SERIALIZE_DECLARE_PUTGET(%s);\n",classname);
	fprintf(fp,"\n");

	fprintf(fp,"class %s\n",classname);
	fprintf(fp,"{\n");
	fprintf(fp,"\npublic:\n");
	
	fprintf(fp,"\t%s(void){}\n",classname);
	fprintf(fp,"\t~%s(void){}\n\n",classname);

	std::list<std::string>::iterator it;

	for( it = varlist.begin(); it != varlist.end(); it++ )
	{
		char name[256];
		memset(name,0,sizeof(name));

		std::string  str = *it;
		fprintf(fp,"\t%s\n",str.c_str());
	}
	
	fprintf(fp,"\tvoid SetData(char *Data,DWORD size)\n");
	fprintf(fp,"\t{\n");

	fprintf(fp,"\t\tSerializer s;\n");
	fprintf(fp,"\t\tBuffer buf;\n");
	fprintf(fp,"\t\tbuf.Read(Data,size);\n\n");

	fprintf(fp,"\t\ts.BeginReading(&buf, false );\n");
	fprintf(fp,"\t\ts.Get(*this);\n");
	fprintf(fp,"\t\ts.EndReading();\n");

	fprintf(fp,"\t}\n\n");


	fprintf(fp,"\tchar* GetData(DWORD &Size)\n");
	fprintf(fp,"\t{\n");

	fprintf(fp,"\t\tSerializer s;\n");
	fprintf(fp,"\t\tBuffer buf;\n");
	fprintf(fp,"\t\ts.Put(*this) ;\n\n");

	fprintf(fp,"\t\ts.EndWriting() ;\n");
	fprintf(fp,"\t\tSize = buf.GetLength();\n");
	fprintf(fp,"\t\treturn (char*)buf.GetData();\n");

	fprintf(fp,"\t}\n\n");

	fprintf(fp,"\tSERIALIZE_DECLARE_FRIEND(%s);\n",classname);
	fprintf(fp,"}\n");
	fprintf(fp,"\n");
	fprintf(fp,"\n");
	fprintf(fp,"\n");

	fprintf(fp,"SERIALIZE_DEFINE_PUT(%s,obj,serializer)\n",classname);
	fprintf(fp,"{\n");

	for( it = varlist.begin(); it != varlist.end(); it++ )
	{
		char name[256];
		memset(name,0,sizeof(name));

		std::string  str = *it;
		GetName((char*)str.c_str(),name);

		fprintf(fp,"\tserializer.Put(obj.%s);\n",name);
	}
	fprintf(fp,"\treturn TRUE;\n");
	fprintf(fp,"}\n");
	fprintf(fp,"\n");
	fprintf(fp,"\n");

fprintf(fp,"SERIALIZE_DEFINE_GET(%s,obj,serializer)\n",classname);
fprintf(fp,"{\n");
	for( it = varlist.begin(); it != varlist.end(); it++ )
	{
		char name[256];
		memset(name,0,sizeof(name));

		std::string  str = *it;
		GetName((char*)str.c_str(),name);

		fprintf(fp,"\tserializer.Get(obj.%s);\n",name);
	}
fprintf(fp,"\treturn TRUE;\n");
fprintf(fp,"}\n");
	
	fclose(fp);
}

int _tmain(int argc, _TCHAR* argv[])
{
	FILE *fp = NULL;
	fp = fopen("r.txt","rb");

	if(fp == NULL)
	{
		printf("not found file %s \n","r.txt");
		return false;
	}

	int cnt = sizeof(Array);

	//첫 공백 문자 제거
	while(fgetc(fp) == ' ');

	char Temp[256];
	memset(Temp,0,256);

	char ClassName[256];
	memset(ClassName,0,256);

	char VarName[256];
	memset(VarName,0,256);

	char ch = 0;
	int pos = 0;
	while(ch != EOF)
	{
		ch = fgetc(fp);
		//printf("%c",ch);
		
		if(pos > 255) pos = 0;
		Temp[pos] = ch;	
		pos++;

		int sel = FindWord(Temp,pos);
		if(sel != -1)
		{
			if(sel == 0) //클래스 이름
			{
				while((ch = fgetc(fp)) == ' ');
				int o=0;
				do
				{
					ClassName[o++] = ch;
				}while((ch = fgetc(fp)) != '\n');

				ClassName[o-1] = NULL;

				printf("%s%s\n",Array[sel],ClassName);
				pos = 0;
				memset(Temp,0,256);
			}
			else		//변수 이름
			{
				ch = fgetc(fp);
				int o=0;
				do
				{
					VarName[o++] = ch;
				}while((ch = fgetc(fp)) != '\n');

				VarName[o-1] = NULL;

				char Var[525];
				memset(Var,0,sizeof(Var));
				sprintf(Var,"%s%s\n",Array[sel],VarName);

				varlist.push_back(Var);
				pos = 0;
				memset(Temp,0,256);
			}
		}
	}
	//fgets(Temp, 256, fp);

	if(ch == '\n')
	{
		 pos = 0;
		 memset(Temp,0,256);
		 while(fgetc(fp) == ' ');
	}

	MakeHeader(ClassName);
	
	if(fp != NULL)
		fclose(fp);

	return 0;
}

