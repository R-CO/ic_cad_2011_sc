#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

#ifndef LITTLE_TOOLS
#define LITTLE_TOOLS

inline size_t CountComma(const string &);
inline string IntToString(const int);
inline size_t CountSpace(const string &);
inline void GetFromTo(const string &, int &, int &);

inline string IntToString(const int integer)
{
       string result;
       int i = 0;
       int temp;
       char c;

       if ( integer == 0 ) {
            result = "0";
       }
       else {
            for ( i = 1; (integer / i) != 0; i *= 10 ) {
            }
            for ( temp = integer, i /= 10; i != 0; temp %= i, i /= 10 ) {
                c = temp / i + 48;
                result += c;
            }
       }

       return result;
}

inline size_t CountComma(const string &veriStatement)
{
       size_t i = 0;
      size_t commaCount = 0;

       do {
           i = veriStatement.find_first_of(',',i);
           if ( i != string::npos ) {
                commaCount++;
                i++;
           }
       } while ( i != string::npos );

       return commaCount;
}

inline size_t CountSpace(const string &subStatement)
{
    size_t i = 0;
    size_t spaceCount = 0;

    do {
        i = subStatement.find_first_of(" \t", i);
        if ( i != string::npos ) {
            spaceCount++;
            i++;
        }
    } while ( i != string::npos );

    return spaceCount;
}

inline void GetFromTo(const string &bus, int &from, int &to)
{
    size_t begin, end;
    string tempString;
    
    begin = bus.find('[');
    begin = bus.find_first_not_of(" \t[", begin);
    end = bus.find_first_of(" \t:", begin);
    tempString = bus.substr(begin, end-begin);
    to = atoi(tempString.c_str());
    begin = bus.find_first_not_of(" \t:", end);
    end = bus.find_first_of(" \t]", begin);
    tempString = bus.substr(begin, end-begin);
    from = atoi(tempString.c_str());
}

#endif

