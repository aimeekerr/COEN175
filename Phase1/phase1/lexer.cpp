//Aimee Kerr
//COEN 175L
//Phase 1
#include <iostream>
#include <cctype>
#include <string>
#include <set>

using namespace std;

int main() {
  char currchar;
  char tempchar;
  string temp;
  set<string> keywords = {"auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while"};
  set<string> operators = {"=","|","||","&&","==","!=","<",">","<=",">=","+","-","*","/","%","&","!","++","--",".","->","(",")","[","]","{","}",";",":",","};
  while(cin.eof() != true)
  {
    currchar = cin.get();
    if(currchar == ' ')
    {
      currchar = cin.get();
    }
    if(currchar == '\\')
    {
      currchar = cin.get();
      currchar = cin.get();
    }
    if(currchar == '/')
    {
      currchar = cin.get();
      if(currchar != '*')
      {
	cin.putback(currchar);
	cin.putback('/');
	currchar = cin.get();
      }
      else
      {
	currchar = cin.get();
	while(temp != "*/")
	{
	  currchar = cin.get();
	  if(currchar == '*' && cin.peek() == '/')
	  {
	    currchar = cin.get();
	    currchar = cin.get();
	    temp = "*/";
	  }
	}
	temp = "";
      }
    }
    if(isdigit(currchar))
    {
      while(isdigit(currchar))
      {
	temp.push_back(currchar);
	currchar = cin.get();
      }
      cout << "number:"  << temp << endl;
      temp = "";
    }
    if(currchar == '"')
    {
      temp.push_back(currchar);
      currchar = cin.get();
      while(currchar != '"')
      {
	if(currchar == '\\' && cin.peek() == '"')
	{
	  temp.push_back('\\');
	  temp.push_back('"');
	  cin.get();
	  currchar = cin.get();
	  while(currchar != '\\')
	  {
	    temp.push_back(currchar);
	    currchar = cin.get();
	  }
	  temp.push_back('\\');
	  currchar = cin.get();
	}
	temp.push_back(currchar);
	currchar = cin.get(); 
      }
      temp.push_back(currchar);
      cout << "string:" << temp << endl;
      temp = "";
    }
    if(currchar == '\'')
    {
      temp.push_back(currchar);
      currchar = cin.get();
      while(currchar != '\'')
      {
	if(currchar == '\\' && cin.peek() == '\'')
	{
	  temp.push_back('\\');
	  temp.push_back('\'');
	  cin.get();
	  currchar = cin.get();
	  while(currchar != '\\')
	  {
	    temp.push_back(currchar);
	    currchar = cin.get();
	  }
	  temp.push_back('\\');
	  currchar = cin.get();
	}
	temp.push_back(currchar);
	currchar = cin.get(); 
      }
      temp.push_back(currchar);
      cout << "character:" << temp << endl;
      temp = "";
    }
    if(isalpha(currchar) || currchar == '_')
    {
      while(isalpha(currchar) || currchar == '_' || isdigit(currchar))
      {
	temp.push_back(currchar);
	currchar = cin.get();
      }
      if(keywords.count(temp) > 0)
	cout << "keyword:"  << temp << endl;
      else
	cout << "identifier:"  << temp << endl;
      temp = "";
    }
    temp.push_back(currchar);
    if(operators.count(temp) > 0)
    {
      currchar = cin.get();
      temp.push_back(currchar);
      if(operators.count(temp) <= 0)
      {
        cin.putback(currchar);
	temp.pop_back();
      }
      cout << "operator:"  << temp << endl;
    }
    temp = "";
  }

  return 0;
}
