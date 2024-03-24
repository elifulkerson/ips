#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <arpa/inet.h>
#include <algorithm>
#include <iomanip>
#include <cmath>

// ips
//
// ... by Eli Fulkerson.  See http://www.elifulkerson.com for updates
//
// Eat input.  Every time we find an IPv4 or IPv6 address, output it.
//


/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



// TODO

// MAYBE TODO:
// meh
// - if we are searching for an IPv4 address or range and an autoconf ipv6 of that ipv4 shows up, we should show it as a match.  Does pton already do this maybe?
// --masks?  --macs?  (not really make sense in ips but kind of helpful and related)
// need an option to input ipv4 addresses and output them as ipv4-in-ipv6 format.  also mac addresses->ipv6 autoconfig addresses
// -to4 -to6  209.173.53.167 the valid IPv6 version will be 0:0:0:0:0:ffff:d1ad:35a7.  Allow conversion of ipv4->ipv6 (always allowed) and ipv6->ipv4 (sometimes allowed)
// -- while we're at it, MAC address support again.  ipv6->embedded mac, mac-> auto ipv6?
// convert incoming ips -> hex, binary, octal, decimal, whatever on output
// open files instead of stdin, also recursive like grep


using namespace std;


void debug_printipv6(sockaddr_in6 ip) {
	char s[1000];
	size_t maxlen = 1000;
	inet_ntop(AF_INET6, &ip.sin6_addr, s, maxlen);
	printf("result: %s", s);
}

bool predicate(char c) {
	//std::string allowed(".:%0123456789abcdefABCDEF");
	std::string allowed(".:%0123456789abcdefABCDEF-/to");  // -,/,t,o are now allowed as a crappy compromise...
	if (allowed.find(c) == string::npos) {
		return true;
	}
	return false;
}

bool predicate_strict(char c) {
	std::string allowed(".:%0123456789abcdefABCDEF");
	
	if (allowed.find(c) == string::npos) {
		return true;
	}
	return false;
}

bool has_only_digits(const string s){
  return s.find_first_not_of( "0123456789" ) == string::npos;
}

bool has_only_hex_digits(const string s){
  return s.find_first_not_of( "0123456789abcdefABCDEF" ) == string::npos;
}

unsigned int SwapBytes(unsigned int original)
{
	return ((original & 0x000000ff) << 24) + ((original & 0x0000ff00) << 8) + ((original & 0x00ff0000) >> 8) + ((original & 0xff000000) >> 24);
}

void check_context_line_for_cisco_specific_match(string cisco_context_line, vector<string> &cisco_text_match) {
	// ok, we spit out a stanza heading.  So - is this heading a...
	// object type X
	// object-group type X
	// command?  If so, we need to push the "name" of the thing onto cisco_text_match, and then we will also match on that text further down.


		istringstream iss(cisco_context_line);
		vector<string> words;
		
		while (iss) {
			string word;
			iss >> word;
			words.push_back(word);
		}

		if (words.size() < 3) {
			return;
		}

		if (words[0] != "object" && words[0] != "object-group") {
			return;
		}

		// if the first word isn't in our list...@@

		bool found = false;
		for (int i = 0; i < cisco_text_match.size(); i++) {
			if (cisco_text_match[i] == words[2]) {
				found = true;
				break;
			}
		}

		if (found == false) {
			
			cisco_text_match.push_back(words[2]);
		}


}

void format_output(bool mode_line_number, bool mode_quiet, bool mode_context, int mode_context_amount, int &context_countdown, int &last_context_line, int current_line, deque<string> &context_lines, string word, bool mode_cisco_context_line, string cisco_context_line, int cisco_context_number, bool &cisco_in_stanza, vector<string> &cisco_text_match) {

    if (mode_quiet) {
        return;
    }

    if (last_context_line == current_line && mode_context) {
        // we already spat this line out
        return;
    }

	
    if (mode_context) {	
		if (mode_cisco_context_line && cisco_context_number >= last_context_line && cisco_context_number != current_line) {
			if (mode_line_number) {
				cout << std::right << std::setw(6) << cisco_context_number << "  " << cisco_context_line << endl;
			}
			else {
				cout << cisco_context_line << endl;
			}

			cout << " ..." << endl;

			cisco_in_stanza = true;
			
			check_context_line_for_cisco_specific_match(cisco_context_line, cisco_text_match);
			
		}

		int tmp_context = context_lines.size();
        for (auto context_line: context_lines) {
			
			if (mode_line_number) {
				cout << std::right << std::setw(6) << current_line - tmp_context + 1 << "  " << context_line << endl;
				tmp_context--;
			}
			else {
				cout << context_line << endl;
			}
        }

        context_lines.clear();

        //remember that we did this
        last_context_line = current_line;
        context_countdown = mode_context_amount + 1; //@@ not sure why the +1

        return;
    }

	if (!mode_line_number) {
		cout << word << endl;
	}
	else {
		cout << std::right << std::setw(6) << current_line  << "  " << word << endl;
	}
    
    return;
}

int getBroadcastv4(sockaddr_in host, sockaddr_in mask, sockaddr_in &broadcast) {
	broadcast.sin_addr.s_addr = host.sin_addr.s_addr | ~mask.sin_addr.s_addr;
	return 0;
}

int getNetworkNumberv4(sockaddr_in host, sockaddr_in mask, sockaddr_in &network) {
	network.sin_addr.s_addr = host.sin_addr.s_addr & mask.sin_addr.s_addr;
	return 0;
}

int getBroadcastv6(sockaddr_in6 host, sockaddr_in6 mask, sockaddr_in6 &broadcast) {
	// I realize that "broadcast is a misnomer, but I'm keeping it as slang for-the-top-address in solidarity with the v4 of this function.
	for (int i = 0; i < 16; i++) {
		broadcast.sin6_addr.s6_addr[i] = host.sin6_addr.s6_addr[i] | ~mask.sin6_addr.s6_addr[i];
	}
	return 0;
}
int getNetworkNumberv6(sockaddr_in6 host, sockaddr_in6 mask, sockaddr_in6 &network) {

	for (int i = 0; i < 16; i++) {
		network.sin6_addr.s6_addr[i] = host.sin6_addr.s6_addr[i] & mask.sin6_addr.s6_addr[i];
	}
	return 0;
}

int convertMaskv6(string value, sockaddr_in6 &mask) {

	// we are only allowing /X syntax here.  Does anybody actually do FFFF:FFFF:FFFF:: etc?
	
	//int numbits = atoi(value.c_str());
	int numbits = strtol(value.c_str(), NULL, 10);

	if (numbits == 0 && value.c_str() != "0") {  // fucking string conversion using 0 as error
		return 1;
	}

	int b = -8;
	
	for (int i = 0; i < 16; i++) {
	
		b += 8;

		if (numbits - b >= 8) {
	
			mask.sin6_addr.s6_addr[i] = mask.sin6_addr.s6_addr[i] | 0xFF;	
			continue;
		}

		if (numbits - b <= 0) {
			mask.sin6_addr.s6_addr[i] = 0x0000;
			continue;
		}

		mask.sin6_addr.s6_addr[i] = 0x00;
		
		for (int j = 0; j < (numbits - b); j++) {
			mask.sin6_addr.s6_addr[i] = mask.sin6_addr.s6_addr[i] + pow(2, (8-j-1));
		}
	}

	//char s[1000];
	//size_t maxlen = 1000;
	//inet_ntop(AF_INET6, &mask.sin6_addr, s, maxlen);
	//printf("result: %s\n", s);
	
	return 0;
}

int convertMaskv4(string value, sockaddr_in &mask) {

	//cout << "!!@@@" << value << endl;

	// I realize that without checking the first octet you can't really say its /A or whatever.  We are using the /A etc here simply as a common shorthand.

	if (value == "0" || value == "0.0.0.0") {
		return (inet_pton(AF_INET, "0.0.0.0", &mask.sin_addr));
	}
	if (value == "1" || value == "128.0.0.0") {
		return (inet_pton(AF_INET, "128.0.0.0", &mask.sin_addr));
	}
	if (value == "2" || value == "192.0.0.0") {
		return (inet_pton(AF_INET, "192.0.0.0", &mask.sin_addr));
	}
	if (value == "3" || value == "224.0.0.0") {
		return (inet_pton(AF_INET, "224.0.0.0", &mask.sin_addr));
	}
	if (value == "4" || value == "240.0.0.0") {
		return (inet_pton(AF_INET, "240.0.0.0", &mask.sin_addr));
	}
	if (value == "5" || value == "248.0.0.0") {
		return (inet_pton(AF_INET, "248.0.0.0", &mask.sin_addr));
	}
	if (value == "6" || value == "252.0.0.0") {
		return (inet_pton(AF_INET, "252.0.0.0", &mask.sin_addr));
	}
	if (value == "7" || value == "254.0.0.0") {
		return (inet_pton(AF_INET, "254.0.0.0", &mask.sin_addr));
	}
	if (value == "8" || value == "255.0.0.0" || value == "A" || value == "a") {
		return (inet_pton(AF_INET, "255.0.0.0", &mask.sin_addr));
	}
	if (value == "9" || value == "255.128.0.0") {
		return (inet_pton(AF_INET, "255.128.0.0", &mask.sin_addr));
	}
	if (value == "10" || value == "255.192.0.0") {
		return (inet_pton(AF_INET, "255.192.0.0", &mask.sin_addr));
	}
	if (value == "11" || value == "255.224.0.0") {
		return (inet_pton(AF_INET, "255.224.0.0", &mask.sin_addr));
	}
	if (value == "12" || value == "255.240.0.0") {
		return (inet_pton(AF_INET, "255.240.0.0", &mask.sin_addr));
	}
	if (value == "13" || value == "255.248.0.0") {
		return (inet_pton(AF_INET, "255.248.0.0", &mask.sin_addr));
	}
	if (value == "14" || value == "255.252.0.0") {
		return (inet_pton(AF_INET, "255.252.0.0", &mask.sin_addr));
	}
	if (value == "15" || value == "255.254.0.0") {
		return (inet_pton(AF_INET, "255.254.0.0", &mask.sin_addr));
	}
	if (value == "16" || value == "255.255.0.0" || value == "B" || value == "b"){
		return (inet_pton(AF_INET, "255.255.0.0", &mask.sin_addr));
	}
	if (value == "17" || value == "255.255.128.0") {
		return (inet_pton(AF_INET, "255.255.128.0", &mask.sin_addr));
	}
	if (value == "18" || value == "255.255.192.0") {
		return (inet_pton(AF_INET, "255.255.192.0", &mask.sin_addr));
	}
	if (value == "19" || value == "255.255.224.0") {
		return (inet_pton(AF_INET, "255.255.224.0", &mask.sin_addr));
	}
	if (value == "20" || value == "255.255.240.0") {
		return (inet_pton(AF_INET, "255.255.240.0", &mask.sin_addr));
	}
	if (value == "21" || value == "255.255.248.0") {
		return (inet_pton(AF_INET, "255.255.248.0", &mask.sin_addr));
	}
	if (value == "22" || value == "255.255.252.0") {
		return (inet_pton(AF_INET, "255.255.252.0", &mask.sin_addr));
	}
	if (value == "23" || value == "255.255.254.0") {
		return (inet_pton(AF_INET, "255.255.254.0", &mask.sin_addr));
	}
	if (value == "24" || value == "255.255.255.0" || value == "C" || value == "c") {
		return (inet_pton(AF_INET, "255.255.255.0", &mask.sin_addr));
	}
	if (value == "25" || value == "255.255.255.128") {
		return (inet_pton(AF_INET, "255.255.255.128", &mask.sin_addr));
	}
	if (value == "26" || value == "255.255.255.192") {
		return (inet_pton(AF_INET, "255.255.255.192", &mask.sin_addr));
	}
	if (value == "27" || value == "255.255.255.224") {
		return (inet_pton(AF_INET, "255.255.255.224", &mask.sin_addr));
	}
	if (value == "28" || value == "255.255.255.240") {
		return (inet_pton(AF_INET, "255.255.255.240", &mask.sin_addr));
	}
	if (value == "29" || value == "255.255.255.248") {
		return (inet_pton(AF_INET, "255.255.255.248", &mask.sin_addr));
	}
	if (value == "30" || value == "255.255.255.252") {
		return (inet_pton(AF_INET, "255.255.255.252", &mask.sin_addr));
	}
	if (value == "31" || value == "255.255.255.254") {
		return (inet_pton(AF_INET, "255.255.255.254", &mask.sin_addr));
	}
	if (value == "32" || value == "255.255.255.255") {
		return (inet_pton(AF_INET, "255.255.255.255", &mask.sin_addr));
	}
	
	//cout << "mask returns 1" << endl;
	return 0;
}

bool isMaskv4(string value) {
	struct sockaddr_in tmpmask;

	// check if its a valid ip first, since convertMask accepts some things that aren't
	if (inet_pton(AF_INET, value.c_str(), &tmpmask) && convertMaskv4(value.c_str(), tmpmask) != 0) {
		return true;
	}

	return false;
}

int compareIPv4( sockaddr_in first, sockaddr_in second) {
	if (SwapBytes(first.sin_addr.s_addr) > SwapBytes(second.sin_addr.s_addr)) {
		return 1;
	}

	if (SwapBytes(first.sin_addr.s_addr) < SwapBytes(second.sin_addr.s_addr)) {
		return -1;
	}
	return 0;
}


bool betweenIPv4(sockaddr_in needle, sockaddr_in haystack_start, sockaddr_in haystack_end) {

	if ( compareIPv4(needle, haystack_start) == -1) {
		return false;
	}

	if (compareIPv4(needle, haystack_end) == 1) {
		return false;
	}

	// same is between
	return true;
}
																					 

int convertIPv4toIPv6(sockaddr_in four, sockaddr_in6 &six) {
	//@@todo
	return 0;
}

// returns 1 if first > second, -1 if first < second, 0 if first == second
int compareIPv6( sockaddr_in6 first, sockaddr_in6 second) {

	for (int i=0;i<16;i++) {
		if (first.sin6_addr.s6_addr[i] > second.sin6_addr.s6_addr[i]) {
			//greater
			
			return 1;
		}

		if (first.sin6_addr.s6_addr[i] < second.sin6_addr.s6_addr[i]) {
			
			return -1;
		}
	}

	// same
	
	return 0;
}

//returns true if needle is within haystack_ or if all three are equal
bool betweenIPv6( sockaddr_in6 needle, sockaddr_in6 haystack_start, sockaddr_in6 haystack_end) {

	// if we are less than or equal to the start, we fail
	if ( compareIPv6(needle, haystack_start) == -1) {
		return false;
	}

	if (compareIPv6(needle, haystack_end) == 1) {
		return false;
	}

	// same is between
	return true;
}

string cleanup_matchstring(string w1, string w2, string w3) {
	/*
	1.1.1.1 - 2.2.2.2
	1.1.1.1/a etc
	1.1.1.1/A etc
	1.1.1.1 255.255.255.0
	1.1.1.1 to 2.2.2.2
	1.1.1.1- 2.2.2.2
	1.1.1.1 -2.2.2.2
	1.1.1.1 <cr> (any of the previous)
	*/

	// first step.  Since we might have as many as 3 relevant arguments, we are going to shove these down into
	// one and then split them back out to lose some complexity.  This works in cases except "1.1.1.1 255.255.255.0" so we
	// need to check for that first first...

	string matchstring = "";

	//cout << "cleanup_matchstring: "<< "w1: " << w1 << " w2: " << w2 << " w3: " << w3 << endl;

	
	if (isMaskv4(w3)) {  // only have to worry about v4 here, since we aren't doing FFFF.FFFF.... for ipv6.
		matchstring = w2 + " " + w3;	
	}
	else {
	
		if ((w2 == "to") || (w2 == "-")) {			
			matchstring = w1 + "-" + w3;	
		}
		else {
			bool dash_in_middle = false;
			for (int j = 0; j < w3.size(); j++) {
				// here, we are looking for a - that is in the middle but not at either end
				if (j != 0 && j != w3.size() && w3[j] == '-') {
					dash_in_middle = true;
				}
				
			}

			if (dash_in_middle) {
				//cout << "dash in middle" << endl;
				matchstring = w3;
			}
			else {
				//cout << "spacing fix" << endl;
				matchstring = w2 + w3;  // this should fix weird spacing
			}
			//@@ except it doesn't - if w3 was already x-y, now its crapx-y and thus invalid
		}
	}


	// at this point, we should be chewed down to the following in matchstring
	// 1.1.1.1-2.2.2.2
	// someshit1.1.1.1/A
	// someshit1.1.1.1/24
	// 1.1.1.1 255.255.255.0
	// ... or some very invalid shit.

	int countdash = 0;
	int countslash = 0;
	int countspace = 0;


	//@@ replace this with a proper function...
	for (int j = 0; j < matchstring.size(); j++) {
		if (matchstring[j] == '-') {
			countdash++;
		}
		if (matchstring[j] == '/') {
			countslash++;
		}
		if (matchstring[j] == ' ') {
			countspace++;
		}
	}



	// if there is a "/", ignore our matchstring and just use w3 - this kills the "someshit1.1.1.1/24"
	if (countslash > 0) {
		matchstring = w3;
	}

	//cout << "cleanup_matchstring: "<< "final: " << matchstring << endl;

	return matchstring;

}

int string_to_ipv6_range(string w1, string w2, string w3, string& prettystring, sockaddr_in6 &ipv6_range_start, sockaddr_in6 &ipv6_range_end) {
	
	string matchstring = cleanup_matchstring(w1, w2, w3);

	int countdash = 0;
	int countslash = 0;
	int countspace = 0;


	//@@ replace this with a proper function...
	for (int j = 0; j < matchstring.size(); j++) {
		if (matchstring[j] == '-') {
			countdash++;
		}
		if (matchstring[j] == '/') {
			countslash++;
		}
		if (matchstring[j] == ' ') {
			countspace++;
		}
	}

	// if we have more than 1 of any of those at this point, its a fail
	if (countslash > 1 || countdash > 1 || countspace > 1) {
		return 1;
	}

	prettystring = matchstring;

	// we know what we are working with from the count* variables, we don't need the symbols anymore...
	replace(matchstring.begin(), matchstring.end(), '-', ' ');
	replace(matchstring.begin(), matchstring.end(), '/', ' ');

	istringstream iss(matchstring);

	string firstarg;
	string secondarg;

	iss >> firstarg;
	iss >> secondarg;

	struct sockaddr_in6 sa6;

	// is this legitimate ipv4?  time to check
	if (inet_pton(AF_INET6, firstarg.c_str(), &(sa6.sin6_addr))) {
		// firstarg is an ipv6, we're good
	}
	else {
		
		return 1;
	}

	// if there is a dash, process it as a range
	if (countdash > 0) {

		// added this later - I want to support people doing "192.168.1.X-Y" for sub ranges as well as the full expansion, only on the last octet though.		
		// ipv6 version.  I think we're safe here - the "last_of" for a :: will be the trailing :, and we're putting it right back...
		if ( (secondarg.compare("0") == 0) || ( has_only_hex_digits(secondarg) && strtol(secondarg.c_str(), NULL, 16) > 0  && strtol(secondarg.c_str(), NULL, 16) < 65536)) {
			int pos = firstarg.find_last_of(":");
			secondarg = firstarg.substr(0,pos) + ":" + secondarg;
		}

		if (inet_pton(AF_INET6, firstarg.c_str(), &ipv6_range_start.sin6_addr) && inet_pton(AF_INET6, secondarg.c_str(), &ipv6_range_end.sin6_addr))
		{
		
			return(0);
		}
	}
	// no dash, process it as a mask...
	else {
		// /24 or /C or 255.255.255.128 syntax, we treat as mask
		struct sockaddr_in6 tmpmask;
		if (convertMaskv6(secondarg, tmpmask) == 0) {
			
			if (inet_pton(AF_INET6, firstarg.c_str(), &ipv6_range_start.sin6_addr)) {
				getNetworkNumberv6(ipv6_range_start, tmpmask, ipv6_range_start);
				getBroadcastv6(ipv6_range_start, tmpmask, ipv6_range_end);
		
				return (0);
			}
		}
	
	}
	// lastly, its OK to fail
	return 1;

}


int string_to_ipv4_range(bool mode_use_old_aton_behavior, string w1, string w2, string w3, string& prettystring, sockaddr_in &ipv4_range_start, sockaddr_in &ipv4_range_end) {
	
	//@@ once this works, it can probably replace the mostly duplicate code in the original range calculator
	string matchstring = cleanup_matchstring(w1, w2, w3);

	int countdash = 0;
	int countslash = 0;
	int countspace = 0;


	//@@ replace this with a proper function...
	for (int j = 0; j < matchstring.size(); j++) {
		if (matchstring[j] == '-') {
			countdash++;
		}
		if (matchstring[j] == '/') {
			countslash++;
		}
		if (matchstring[j] == ' ') {
			countspace++;
		}
	}

	// if we have more than 1 of any of those at this point, its a fail
	if (countslash > 1 || countdash > 1 || countspace > 1) {
		return 1;
	}

	prettystring = matchstring;

	// we know what we are working with from the count* variables, we don't need the symbols anymore...
	replace(matchstring.begin(), matchstring.end(), '-', ' ');
	replace(matchstring.begin(), matchstring.end(), '/', ' ');

	istringstream iss(matchstring);

	string firstarg;
	string secondarg;

	iss >> firstarg;
	iss >> secondarg;

	struct sockaddr_in sa;

	// is this legitimate ipv4?  time to check
	if ((mode_use_old_aton_behavior && inet_aton(firstarg.c_str(), &(sa.sin_addr))) ||
		(!mode_use_old_aton_behavior && inet_pton(AF_INET, firstarg.c_str(), &(sa.sin_addr)))) {
		// firstarg is an ipv4, we're good
	}
	else {
		return 1;
	}

	// if there is a dash, process it as a range
	if (countdash > 0) {

		// added this later - I want to support people doing "192.168.1.X-Y" for sub ranges as well as the full expansion, only on the last octet though.		
		if ( (secondarg.compare("0") == 0) || ( has_only_digits(secondarg) && atoi(secondarg.c_str()) > 0  && atoi(secondarg.c_str()) < 256)) {
			int pos = firstarg.find_last_of(".");
			secondarg = firstarg.substr(0,pos) + "." + secondarg;
		}

		if ((!mode_use_old_aton_behavior && inet_pton(AF_INET, firstarg.c_str(), &ipv4_range_start.sin_addr) && inet_pton(AF_INET, secondarg.c_str(), &ipv4_range_end.sin_addr)) ||
			(mode_use_old_aton_behavior && inet_aton(firstarg.c_str(), &ipv4_range_start.sin_addr) && inet_aton(secondarg.c_str(), &ipv4_range_end.sin_addr))) {
			return 0;
		}
	}
	// no dash, process it as a mask...
	else {
		// /24 or /C or 255.255.255.128 syntax, we treat as mask
		struct sockaddr_in tmpmask;
		if (convertMaskv4(secondarg, tmpmask) != 0) {

			if (mode_use_old_aton_behavior && inet_aton(firstarg.c_str(), &ipv4_range_start.sin_addr)) {
				getNetworkNumberv4(ipv4_range_start, tmpmask, ipv4_range_start);
				getBroadcastv4(ipv4_range_start, tmpmask, ipv4_range_end);
				return 0;
			} 

			//printf("@@biff");

			if (inet_pton(AF_INET, firstarg.c_str(), &ipv4_range_start.sin_addr)) {
				getNetworkNumberv4(ipv4_range_start, tmpmask, ipv4_range_start);
				getBroadcastv4(ipv4_range_start, tmpmask, ipv4_range_end);
				return 0;
			}

			if (!mode_use_old_aton_behavior && inet_pton(AF_INET, firstarg.c_str(), &ipv4_range_start.sin_addr)) {
				getNetworkNumberv4(ipv4_range_start, tmpmask, ipv4_range_start);
				getBroadcastv4(ipv4_range_start, tmpmask, ipv4_range_end);
				return 0;
			}
		}
	}
	return 1;
	
}

bool range_within_range_v4(bool mode_use_old_aton_behavior, sockaddr_in user_start, sockaddr_in user_end, string w1, string w2, string w3, string &prettystring) {

	// we are within a range if either endpoint of range A is overlapping with range B
	struct sockaddr_in document_start;
	struct sockaddr_in document_end;

	if (string_to_ipv4_range(mode_use_old_aton_behavior, w1, w2, w3, prettystring, document_start, document_end) != 0) {
		return false;
	}

	if (betweenIPv4(document_start, user_start, user_end)) {
		return true;
	}

	if (betweenIPv4(document_end, user_start, user_end)) {
		return true;
	}

	if (betweenIPv4(user_start, document_start, document_end)) {
		return true;
	}

	if (betweenIPv4(user_end, document_start, document_end)) {
		return true;
	}
	
	return false;
}

bool range_within_range_v6(sockaddr_in6 user_start, sockaddr_in6 user_end, string w1, string w2, string w3, string &prettystring) {
	// we are within a range if either endpoint of range A is overlapping with range B
	struct sockaddr_in6 document_start;
	struct sockaddr_in6 document_end;

	if (string_to_ipv6_range(w1, w2, w3, prettystring, document_start, document_end) != 0) {
		return false;
	}

	if (betweenIPv6(document_start, user_start, user_end)) {
		return true;
	}

	if (betweenIPv6(document_end, user_start, user_end)) {
		return true;
	}

	if (betweenIPv6(user_start, document_start, document_end)) {
		return true;
	}

	if (betweenIPv6(user_end, document_start, document_end)) {
		return true;
	}

	return false;
}


int main(int argc, char* argv[]) {


	string input = "";
	string output = "";

	bool mode_ipv4 = true;
	bool mode_ipv6 = true;
	bool mode_ipv6_include_scope = false;
    //bool mode_context = false;
	bool mode_context = true;				// basically, the old -c 0 is what I want all the time, the --range-within-document stuff makes straight ip output dumb.  @@maybe add that back with its own flag later
    int mode_context_amount = 0;           // with 0 context we are just going to output the line itself.  Either '-c' or '-c 0' should give us this.
    bool mode_quiet = false;               // -q, no output

    
    int number_of_matches = 0;             // return value.  Increment every time we get a match
    int current_line = 0;                  // keep track of total lines as we go
    bool mode_strict = false;              // require ips to be surrounded by whitespace already

    string mode_range_string = "";
    bool mode_range = false;
	bool mode_range_shortcut = false;     // this mode doesn't parse for a range, but we want it treated as a range later on
	int range_type = 0; // 4, or 6. 

	struct sockaddr_in ipv4_range_start;
	struct sockaddr_in ipv4_range_end;
	struct sockaddr_in6 ipv6_range_start;
	struct sockaddr_in6 ipv6_range_end;

	bool mode_line_number = false;

	bool mode_cisco_context_line = false;

	bool mode_hexipv4 = false;

   
	bool mode_use_old_aton_behavior = false;

	std::string final_arg = "";

	//parse some arguments.  Starting at 1 because 0 is "ips"
	for ( int i = 1; i < argc; i++) {

		std::string arg = argv[i];

        if (arg == "-c" || arg == "--context") {
            mode_context = true;

            if ( i+1 < argc) {
                mode_context_amount = atoi(argv[i+1]);
                if (mode_context_amount != 0) {      
                    i++;       // probably actually don't need this.  random arguments that aren't matched will just get ignored anyway.
                }
            }
			continue;
        }

		// shorthand for ranges
		if (arg == "-A") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "1.0.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "126.255.255.255", &ipv4_range_end.sin_addr);
			continue;
		}

		if (arg == "-B") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "128.0.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "191.255.255.255", &ipv4_range_end.sin_addr);
			continue;
		}

		if (arg == "-C") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "192.0.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "223.255.255.255", &ipv4_range_end.sin_addr);
			continue;
		}

		if (arg == "-D" || arg == "--multicast") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "224.0.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "239.255.255.255", &ipv4_range_end.sin_addr);
			continue;
		}

		if (arg == "-E" || arg == "--experimental") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "240.0.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "254.255.255.255", &ipv4_range_end.sin_addr);
			continue;
		}

		if (arg == "--10") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "10.0.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "10.255.255.255", &ipv4_range_end.sin_addr);
			continue;
		}


		if (arg == "--172") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "172.16.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "172.31.255.255", &ipv4_range_end.sin_addr);
			continue;
		}

		if (arg == "--192") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "192.168.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "192.168.255.255", &ipv4_range_end.sin_addr);
			continue;
			
		}

		if (arg == "--localhost" || arg == "--loopback") {
			mode_range_shortcut = true;
			range_type = 4;
			inet_pton(AF_INET, "127.0.0.0", &ipv4_range_start.sin_addr);
			inet_pton(AF_INET, "127.255.255.255", &ipv4_range_end.sin_addr);
			continue;
		}



        if (arg == "-s" || arg == "--strict") {
            mode_strict = true;
			continue;
        }

		if (arg == "-b" || arg == "--basic") {
			mode_context = false;
			continue;
		}


        if (arg == "-q" || arg == "--quiet") {
            mode_quiet = true;
			continue;
        }

        if (arg == "-x" || arg == "--hexipv4") {
        	mode_hexipv4 = true;
        	continue;
        }

		if (arg == "-4") {
			mode_ipv6 = false;
			continue;
		}

		if (arg == "-6") {
			mode_ipv4 = false;
			continue;
		}

		if (arg == "-n") {
			mode_line_number = true;
			continue;
		}

		if (arg == "--cisco") {
			mode_cisco_context_line = true;
			continue;
		}

		if (arg == "-%") {
			mode_ipv6_include_scope = true;
			continue;
		}

		if (arg == "--aton") {
			//because stuff like 10:1 used to be a valid way to say 10:0:0:1, but not anymore (inet_aton vs inet_pton)
			mode_use_old_aton_behavior = true;
			continue;
		}

		if (arg == "-v" || arg == "--version") {
			cout << "ips v0.7  5 July 2017" << endl
				 << "by Eli Fulkerson.  See http://www.elifulkerson.com for updates" << endl << flush;
			return 0;
		}

		if (arg == "-h" || arg == "--help" || arg == "--usage" || arg == "?" || arg == "/?" || arg == "-?" || arg == "/h" || arg == "/H") {
			cout << "Syntax: your_command | ips [-4] [-6] [-v] [-%] [-s] [-q] [-c lines] [-v]  ip (or range)" << endl
				<< endl
				<< "Eats STDIN, outputs lines containing IP addresses or matching specified IP or range."
				<< endl
				<< "Options:" << endl
				<< " -4     : match IPv4 addresses only" << endl
				<< " -6     : match IPv6 addresses only" << endl
				<< " -%     : include IPv6 scope" << endl
				<< " -s     : strict, require IPs to be surrounded by whitespace" << endl
				<< " -n     : output line numbers" << endl
				<< " -q     : quiet, no output" << endl
				<< " -c X   : print X lines of context.  (Also --context)" << endl
                << " -b     : 'basic' mode - one ip per line, no context.  (--basic)" << endl
                << " -x     : match 'hex ipv4' addresses, for isntance 'FFFFFFFF'" << endl
				<< " -v     : Display version information" << endl
				<< " --cisco:  Include the last non-indented line when printing context" << endl
				<< " --aton : Use \"old\" inet_aton behavior.  For instance, 10.5 is another name for 10.0.0.5" << endl
				<< endl
				<< "Shorthand:" << endl
				<< " A,-B,-C,-D,-E    : Shorthand to match addresses in the classical IP address classes.  Also --multicast, --experimental for D and E" << endl
				<< " --loopback       : Shorthand to match 127.0.0.0 through 127.255.255.255" << endl
				<< " --10,--192,--172 : Shorthand to match RFC 1918 address ranges" << endl
				<< endl
				<< "IP/Range Syntax:" << endl
				<< " 192.168.1.1" << endl
				<< " 192.168.1.1-192.168.1.255" << endl
				<< " 192.168.1.1/24" << endl
				<< " 192.168.1.1/C" << endl
				<< " '192.168.1.1 255.255.255.0'" << endl
				<< " 192.168.1.5-15" << endl
				
				<< flush;


			return 0;
		}

		// should fail to hit if all flags
		final_arg = argv[i];
	}

	
	// parse the final arg.  If it is already an ip, double it, then feed it to mode_range.  If it exists otherwise, feed it to mode_range as-is.
	// this override --range and --ip and makes them useless, hopefully
	struct sockaddr_in sa;
	struct sockaddr_in6 sa6;
	if (final_arg.length() > 0 ) {
		// if final_arg, by itself, is a valid IPv4 or IPv6...
		if ((!mode_use_old_aton_behavior && inet_pton(AF_INET, final_arg.c_str(), &(sa.sin_addr))) ||
			(mode_use_old_aton_behavior && inet_aton(final_arg.c_str(), &(sa.sin_addr))) ||
			inet_pton(AF_INET6, final_arg.c_str(), &(sa6.sin6_addr))) {
			final_arg = final_arg + "-" + final_arg;
		}
		mode_range = true;
		mode_range_string = final_arg;
	}
	
	if (mode_range) {

		// we aren't actually using these here but the function demands..
		string prettystring = "";
		string w1 = "";
		string w2 = "";

		if (string_to_ipv4_range(mode_use_old_aton_behavior, w1, w2, mode_range_string, prettystring, ipv4_range_start, ipv4_range_end) == 0) {
			range_type = 4;
		}
		else {
			if (string_to_ipv6_range(w1, w2, mode_range_string, prettystring, ipv6_range_start, ipv6_range_end) == 0) {
				range_type = 6;
			}
			else {
				cerr << "Invalid range string specified" << endl;
				exit(1);
			}
		}
	}


    std::deque<string> context_lines;
    int context_countdown = 0;                  // when we get a match, set context_countdown to mode_context_amount.  Decrement every non match.  if 0, don't print context.
    int last_context_line = 0;                  // since we can match more than once on a line, remember that we did so and don't output again for the same line in context mode

	// rotating buffer for strings n - 1 and n - 2, used for in-document-range-matching
	string prevword = "";
	string prevprevword = "";
	string currword = "";

	// sometimes we want more than the current word can give us
	string prettystring = "";

	string cisco_context_line = "";
	int cisco_context_number = -1;
	bool cisco_in_stanza = false;
	
	vector<string> cisco_text_match;

	while (true) {

		getline(cin, input);

		if (!cin) {
			// @@ it would be nice to have this work, but with ranges in play - a single range can match multiple times
			// ... and then have their output suppressed, so you don't get a real number_of_matches anymore.
			if (number_of_matches > 0) {
				return 1;
			} else {
				return 0;
			}
			
			//return number_of_matches;
		}

        current_line++;

		//cerr << current_line << ": " << input << endl;

		if (input[0] != ' ') {

			// close the old cisco, if we had one to close
			if (cisco_in_stanza) {
				cout << " ..." << endl;
				cisco_in_stanza = false;
			}
			cisco_context_line = input;
			cisco_context_number = current_line;
		}


        context_lines.push_back(input);
        if (context_lines.size() > mode_context_amount + 1) {
            context_lines.pop_front();
        }


        // this is an early tokenization, pre the predicate stuff, because I *now* also want perfect real string matching.

        vector<string> words_pre;
		istringstream iss_pre(input);

		

		while (iss_pre) {
			string word;
			iss_pre >> word;
			words_pre.push_back(word);
		}

		for (int i = 0; i < words_pre.size(); i++) {

        	if (cisco_text_match.size() > 0) {

				for (int k=0; k < cisco_text_match.size(); k++) {
					//cout << cisco_text_match[k] << " " << word << endl;
					if (cisco_text_match[k] == words_pre[i]) {

						format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, prettystring, mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza, cisco_text_match);						
						number_of_matches++;
						continue;
					}
				}
			}
		}

		//cerr << "@@Parsing line: " << input << endl;

		// The output of traceroute is a common intended use case, and gives us
		//replace (input.begin(), input.end(), '[', ' ');
		// things like [127.0.0.1].  So lets tokenize on [] also make traceroute happy
		//replace (input.begin(), input.end(), ']', ' ');

		// meh, honestly, maybe we should consider deleting everything that is not in the set
		// ".:%0123456789abcdefABCDEF"  so that it won't interfere with tokenization if people
		// use funky punctuation or otherwise run their words together.

		// This is fucking with the reverse range stuff, but its also used for initial tokenization, so we do it again later with a different predicate
        if(!mode_strict) {
		  replace_if( input.begin(), input.end(), predicate, ' ');
        }
        
		vector<string> words;
		istringstream iss(input);

		while (iss) {
			string word;
			iss >> word;
			words.push_back(word);
		}

		for (int i = 0; i < words.size(); i++) {

			int countdot = 0;
			int countcolon = 0;
			int countpercent = 0;

			string word = words[i];

			//rotate our previous word buffers (used for in-document--range)
			prevprevword = prevword;
			prevword = currword;
			currword = word;


			for (int j = 0; j < word.size(); j++) {
				if (word[j] == '.') {
					countdot++;
				}

				if (word[j] == ':') {
					countcolon++;
				}

				if (word[j] == '%') {
					countpercent++;
				}
			}

			if (mode_range || mode_range_shortcut) {

				if (range_within_range_v4(mode_use_old_aton_behavior, ipv4_range_start, ipv4_range_end, prevprevword, prevword, currword, prettystring) == true) {
					prettystring = "\"" + prevprevword + " " + prevword + " " + currword + "\"";
					format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, prettystring, mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza, cisco_text_match);
					number_of_matches++;
					continue;
				}
				if (range_within_range_v6(ipv6_range_start, ipv6_range_end, prevprevword, prevword, currword, prettystring) == true) {
					prettystring = "\"" + prevprevword + " " + prevword + " " + currword + "\"";
					format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, prettystring, mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza, cisco_text_match);
					number_of_matches++;
					continue;
				}
			}

			// ok, we're done with the range stuff now.  Fix the range-breaking-strict-mode stuff
			if (!mode_strict) {

				// ok, so we had a problem here.  We have already tokenized things, and are going word-by-word.
				// so at some point we replaced "replace_if" with word.erase, this was done so we wouldn't have any
				// extra whitespace in our already-post-whitespaced list of words.  This however breaks things like
				// 192.168.1.11-51, which ends up being either "192.168.1.1151" or "192.168.1.11 51", neither of which
				// past muster as an IP.

				// BUT, at this point we are past any range_within_range stuff, so we can afford to not care about
				// the range implications.  If word.split(' ')[0] is a valid ip, we're a valid ip.


				// @@ I *think* this is only a problem with the dash - operator, because I can't think of anything else
				// that deserves to be interstitial.  I should handle this by going into the range function and properly
				// handling at least that case on the final octet.

				//replace_if(word.begin(), word.end(), predicate_strict, ' ');
				if (word.find("-") != string::npos) {
					word = word.substr(0, word.find("-"));
				}

				if (word.find("/") != string::npos) {
					word = word.substr(0, word.find("/"));
				}

				if (word.find("t") != string::npos) {
					word = word.substr(0, word.find("t"));
				}

				if (word.find("o") != string::npos) {
					word = word.substr(0, word.find("o"));
				}

				// This was the original attempt:
				//replace_if(word.begin(), word.end(), predicate_strict, ' ');
				// replaced with:
				//word.erase(remove_if(word.begin(), word.end(), predicate_strict), word.end());
				// but I now think both are irrelevant compared to the find/substr bit above
			}


            // hex behavior

			if (mode_hexipv4) {

				bool its_a_hex = false;

				// We didn't actually need to handle 0x separately, it was already taken care of by the tokenzation

				//if (word.length() == 10 && word.substr(0,2) == "0x" && has_only_hex_digits(word.substr(2,8))) {
				//	cerr << "1" << endl;
				//	sa.sin_addr.s_addr = SwapBytes( strtoul(word.substr(2,8).c_str(), NULL, 16) );
				//	its_a_hex = true;
				//}

				if (word.length() == 8 && has_only_hex_digits(word)) {
					sa.sin_addr.s_addr = SwapBytes( strtoul(word.c_str(), NULL, 16) );
					its_a_hex = true;
				}

				if (its_a_hex) {


					if ((mode_range || mode_range_shortcut) && (betweenIPv4(sa, ipv4_range_start, ipv4_range_end) == false)) {
						// well, we were an IP address, but we're looking for a range and we didn't match the range.
						continue;
					}

					format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, word, mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza, cisco_text_match);

                	number_of_matches++;
					continue;
				}

            }

            // Early rejection of some strings on the theory that this is faster than inet_pton
            
            // if we don't have either a . or a : or if we have both, its not a valid IP address
            if ((countdot == 0 && countcolon == 0) || (countdot != 0 && countcolon != 0)) {
                continue;
            }

            // if we have a . but we don't have exactly 3 .'s, its not a valid IPv4 address
			if (!mode_use_old_aton_behavior) {
				if (countdot != 0 && countdot != 3) {
					continue;
				}
			}

            // if we have a : but don't have at least 2 :'s, its not a valid IPv6 address
            if (countcolon == 1) {
                continue;
            }

            // if we have more than 7 :'s... its not a valid IPv6 address
            if (countcolon > 7) {
                continue;
            }

            // if we have a % but don't have exactly 1, its not a valid IPv6%scopeid
            if (countpercent != 0 and countpercent != 1) {
                continue;
            }
            

			// Final check - lets shove it into inet_pton and see if its really an IP			
			if (mode_ipv4 && range_type != 6) {
				struct sockaddr_in sa;
				if ((!mode_use_old_aton_behavior && inet_pton(AF_INET, word.c_str(), &(sa.sin_addr))) ||
					(mode_use_old_aton_behavior && inet_aton(word.c_str(), &(sa.sin_addr)))) {

					// ok, its an ip address.  BUT
					if ((mode_range || mode_range_shortcut) && (betweenIPv4(sa, ipv4_range_start, ipv4_range_end) == false)) {
						// well, we were an IP address, but we're looking for a range and we didn't match the range.
						continue;
					}

					format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, word, mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza, cisco_text_match);

                    number_of_matches++;
					continue;
				}
				
			}

			if (mode_ipv6 && range_type != 4 && countcolon != 0) {

				struct sockaddr_in6 sa6;

				if (countpercent == 1) {
					// we have a %scopeid to deal with.  inet_pton doesn't seem to like scope ids, so split it off,
					// check the base, then print the whole thing if desired

					vector<string> ipparts;
					std::stringstream ss(word);
					std::string item;

					while (std::getline(ss, item, '%')) {
						ipparts.push_back(item);
					}

					if (ipparts.empty()) {
						//Fix for a crash bug with for instance "t...%20".
						continue;
					}


					if (inet_pton(AF_INET6, ipparts[0].c_str(), &(sa6.sin6_addr))) {

						if ((mode_range || mode_range_shortcut) && (betweenIPv6(sa6, ipv6_range_start, ipv6_range_end) == false)) {
							continue;
						}


						if (mode_ipv6_include_scope) {
							format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, word, mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza,cisco_text_match);
						} else {
							format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, ipparts[0].c_str(), mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza, cisco_text_match);
						}

                        number_of_matches++;
						continue;
					}

				} else {
					if (inet_pton(AF_INET6, word.c_str(), &(sa6.sin6_addr))) {

						if ((mode_range || mode_range_shortcut) && (betweenIPv6(sa6, ipv6_range_start, ipv6_range_end) == false)) {
							continue;
						}

						format_output(mode_line_number, mode_quiet, mode_context, mode_context_amount, context_countdown, last_context_line, current_line, context_lines, word, mode_cisco_context_line, cisco_context_line, cisco_context_number, cisco_in_stanza, cisco_text_match);

                        number_of_matches++;
						continue;
					}
				}
			}
		}

        if (context_countdown > 0 ) {
            
            if ( context_lines.size() > 0) {
				if (mode_line_number) {
					//cout << current_line << ": " << context_lines.back() << endl;
					cout << std::right << std::setw(6) << current_line << "  " << context_lines.back() << endl;
				}
				else {
					cout << context_lines.back() << endl;
				}
                context_lines.clear();
            }

            context_countdown--;
        } else {

            context_countdown = 0;
        }

        /*
		if (!cin) {
			// @@ it would be nice to have this work, but with ranges in play - a single range can match multiple times
			// ... and then have their output suppressed, so you don't get a real number_of_matches anymore.
			if (number_of_matches > 0) {
				return 1;
			} else {
				return 0;
			}
			
			//return number_of_matches;
		}
		*/
	}
}
