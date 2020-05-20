#include <vector>
#include <string>
using namespace std;

namespace Base64
{
	string base64_Encoder(const string&);
	string base64_Decoder(const string&);
	
	const string& SALT1 = "cruciverbalist",
		& SALT2 = "scrabulizer",
		& SALT3 = "highyer0#12",
		& SALT4 = "beekachu97",
		& SALT5 = "AnAkLuSmOs";

	//Quadruple Encoding
	string EncryptMain(string str)
	{
		str = SALT1 + str + SALT3;
		str.insert(10, SALT2);
		str = base64_Encoder(str);
		str.insert(3, SALT4);
		str = base64_Encoder(str);
		str = str + SALT5;
		str = str + SALT4;
		str = base64_Encoder(str);
		str.insert(6, SALT1);
		str = base64_Encoder(str);
		str.insert(6, "Ha");
		str.insert(5, "An54");
		return str;
	}
	const string& BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	
	//Used for Base64 Encoding
	string base64_Encoder(const string& str)
	{
		string retstr;
		int val = 0,// Indec used to map input to table
			bits = -6;// No of bits in subgroup
		const unsigned int Base63 = 0x3F; //3F = 63
		
		for (const auto& c : str)
		{
			val = (val << 8) + c;
			bits += 8;
			while (bits >= 0)
			{
				retstr.push_back(BASE64_CODES[(val >> bits) & Base63]);
				bits -= 6;
			}
		}

		if (bits > -6)
			retstr.push_back(BASE64_CODES[((val << 8) >> (bits + 8)) & Base63]);
		
		while (retstr.size() % 4)
			retstr.push_back('=');

		return retstr;
	}
}