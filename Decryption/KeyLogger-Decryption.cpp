#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const string& SALT1 = "cruciverbalist",
            & SALT2 = "scrabulizer",
            & SALT3 = "highyer0#12",
            & SALT4 = "beekachu97",
            & SALT5 = "AnAkLuSmOs",
            & BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string base64_Decoder(const string& s);
string DecryptMain(string str);

int main(int argc, char* argv[])
{
    if (argc != 3)
        return cout << "Decryption needs two arguments. Input and Output Files." << endl, 2;
    
    string inp(argv[1]),
           out(argv[2]),
            data;
    
    ifstream filein(inp);
    if (!filein)
        return cout << "Cannot parse input file" << inp << "'" << endl, 3;
    filein >> data;
    if(!filein)
        return cout<< "Input file is corrupted"<<endl,4;

    data = DecryptMain(data);
    
    ofstream fileout(out);
    if (!fileout)
        return cout << "Cannot write into output file" << endl, 5;
    fileout << data;
    cout << "Successfully Decoded" << endl;
    return 0;
}

string DecryptMain(string str)
{
    str.erase(5, 4);
    str.erase(6, 2);
    str = base64_Decoder(str);
    str.erase(6, SALT1.length());
    str = base64_Decoder(str);
    str = str.substr(0, str.length() - SALT4.length());
    str = str.substr(0, str.length() - SALT5.length());
    str = base64_Decoder(str);
    str.erase(3, SALT4.length());
    str = base64_Decoder(str);
    str.erase(10, SALT2.length());
    str = str.substr(SALT1.length());
    str = str.substr(0, str.length() - SALT3.length());
    return str;
}

string base64_Decoder(const string& s)
{
    std::string ret;
    std::vector<int> vec(256, -1);
    for (int i = 0; i < 64; i++)
        vec[BASE64_CODES[i]] = i;
    int val = 0, bits = -8;
    for (const auto& c : s)
    {
        if (vec[c] == -1) break;
        val = (val << 6) + vec[c];
        bits += 6;

        if (bits >= 0)
        {
            ret.push_back(char((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return ret;
}