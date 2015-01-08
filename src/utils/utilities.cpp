#include <utils/utilities.h>

void SplitLine(const std::string& str, const std::string& delim, std::deque<std::string>& output)
{
    unsigned int offset = 0;
    size_t delimIndex = 0;
    
    delimIndex = str.find(delim, offset);

    while (delimIndex != string::npos)
    {
    	string piece = str.substr(offset, delimIndex - offset);
	
        if(piece.compare("") != 0 && piece.compare(" ") != 0 && piece.compare("\t") != 0)
          output.push_back(piece);
          
        offset += delimIndex - offset + delim.length();
        delimIndex = str.find(delim, offset);
    }

    output.push_back(str.substr(offset));
}

string itos(int i)// convert int to string
{
  stringstream s;
  s << i;
  return s.str();
}

string ftos(double f)// convert double to string
{
  int prec= numeric_limits<long double>::digits10; // we assume no more than 1063582293 
  ostringstream out;
  out.precision(prec);//override the default
  out<<f;
  string str= out.str();
  
  return str;
}

