#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <sstream>

// xips
//
// ... by Eli Fulkerson.  See http://www.elifulkerson.com for updates
//
// Eat input.  Expand it into an IP list.
// new!  mode -s -- summarize the input you ate
//
// Original intended use - something like:
// echo 172.20.141.0/24 | xips | xargs -n 1 tcping -n 1
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


using namespace std;


string cidr_to_subnetmask(int cidr) {
    switch(cidr) {
        case 0:
            return "0.0.0.0";
        case 1:
            return "128.0.0.0";
        case 2:
            return "192.0.0.0";
        case 3:
            return "224.0.0.0";
        case 4:
            return "240.0.0.0";
        case 5:
            return "248.0.0.0";
        case 6:
            return "252.0.0.0";
        case 7:
            return "254.0.0.0";
        case 8:
            return "255.0.0.0";
        case 9:
            return "255.128.0.0";
        case 10:
            return "255.192.0.0";
        case 11:
            return "255.224.0.0";
        case 12:
            return "255.240.0.0";
        case 13:
            return "255.248.0.0";
        case 14:
            return "255.252.0.0";
        case 15:
            return "255.254.0.0";
        case 16:
            return "255.255.0.0";
        case 17:
            return "255.255.128.0";
        case 18:
            return "255.255.192.0";
        case 19:
            return "255.255.224.0";
        case 20:
            return "255.255.240.0";
        case 21:
            return "255.255.248.0";
        case 22:
            return "255.255.252.0";
        case 23:
            return "255.255.254.0";
        case 24:
            return "255.255.255.0";
        case 25:
            return "255.255.255.128";
        case 26:
            return "255.255.255.192";
        case 27:
            return "255.255.255.224";
        case 28:
            return "255.255.255.240";
        case 29:
            return "255.255.255.248";
        case 30:
            return "255.255.255.252";
        case 31:
            return "255.255.255.254";
        case 32:
            return "255.255.255.255";
        default:
            return "";
    }

    return "";
}

string int_to_bitstring(int val) {
    string buf = "";

    for (int i = 7; i >= 0; i--) {
        if (val >= pow(2,i)) {
            buf += "1";
            val -= pow(2,i);
        } else {
            buf += "0";
        }
    }
    
    return buf; 
}

string ipv6_address_to_bitstring(sockaddr_in6 &ipv6_address) {

    string buf = "";
    
    for (int i = 0; i < 16; i++) {
        buf += int_to_bitstring(ipv6_address.sin6_addr.s6_addr[i]);
    }
    return buf;
}

void bitstring_to_ipv6_address(string bits, sockaddr_in6 &ipv6_address) {
    int scratch[16];

    for (int i = 0; i < 16; i++) {
        scratch[i] = 0;
        for (int j = 0; j < 8; j++) {

            if ((i*8) + j >= bits.length()) {

            } else {

                //if (bits.at( (i*8)+j) == '0') {
                    // well, nothing
                //}

                if (bits.at( (i*8)+j) == '1') {
                    
                    scratch[i] += pow(2,7-j);
                }
            }
        }
    }

    for (int i = 0; i < 16; i++) {
        ipv6_address.sin6_addr.s6_addr[i] = scratch[i];
    }

}



string int_to_binary_string(int val) {

    string buffer = "";

    for (int i=7; i >= 0; i--) {

        if (val >= pow(2, i)) {
            buffer += "1";
            val = val - pow(2,i);
        } else {
            buffer += "0";
        }

    }
    return buffer;

}

string rightpad_string_bits(string bits) {

    // right pad with zeros..
    while (bits.length() < 32) {
        bits = bits + "0";
    }

    return bits;
}

string string_bits_to_string_ip(string bits) {
    bits = rightpad_string_bits(bits);

    //cout << "bits are " << bits << endl << flush;

    short a,b,c,d;
    short pval;

    a = 0;
    pval = 7;
    for (int i=0; i < 8; i++) {

        if (bits.at(i) == '1') {
            a = a + pow(2, pval);
        }
        pval--;

    }

    b = 0;
    pval = 7;
    for (int i=8; i < 16; i++) {

        if (bits.at(i) == '1') {
            b = b + pow(2, pval);
        }
        pval--;        
    }

    c = 0;
    pval = 7;
    for (int i=16; i < 24; i++) {

        if (bits.at(i) == '1') {
            c = c + pow(2, pval);
        }
        pval--;        
    }

    d = 0;
    pval = 7;
    for (int i=24; i < 32; i++) {

        if (bits.at(i) == '1') {
            d = d + pow(2, pval);
        }
        pval--;
    }

    string buffer = to_string(a) + "." + to_string(b) + "." + to_string(c) + "." + to_string(d);
    return buffer;

}



struct node
{
    int value;  //0 or 1
    unsigned long long count;  //@@ this only gets me 64 bits I think - I want 128.  So I'll have to do some sort of double setup if I want to summarize *all* the ipv6s

    node *leaf0;   // the node where the next bit is a 0
    node *leaf1;   // the node where the next bit is a 1
};

class ipbtree
{
    public:
        ipbtree();
        ~ipbtree();

        void insert(string ip);  // insert a string of bits
        void insert6(string ip);  // insert a string of bits

        bool exists(string bits);  // check to see if a string of bits exists in the tree

	    void summarize(int maxdepth, bool display_cidr, bool display_network_object);
	    void summarize6(int maxdepth, bool display_cidr, bool display_network_object);

	    void sort(bool display_cidr, bool display_network_object);
    	void sort(int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object);

	    void sort6(bool display_cidr, bool display_network_object);
    	void sort6(int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object);

        void destroy_tree();

    private:
        void destroy_tree(node *leaf);
        void insert(int depth, string bits, node *leaf);
        bool exists (int depth, string bits, node *leaf);
    	void summarize(int maxdepth, int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object);
	    void summarize6(int maxdepth, int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object);

        node *root;
};

ipbtree::ipbtree(){
    root = new node;

    root->leaf0 = NULL;
    root->leaf1 = NULL;
    root->count = 0;
    root->value = 0;
}

ipbtree::~ipbtree(){
    destroy_tree();
}

void ipbtree::destroy_tree(){
    destroy_tree(root);
}

void ipbtree::destroy_tree(node *leaf) {
    destroy_tree(leaf->leaf0);
    destroy_tree(leaf->leaf1);
    delete leaf;
}

void ipbtree::insert(string ip) {



    unsigned short a, b, c, d;

    sscanf(ip.c_str(), "%hu.%hu.%hu.%hu", &a, &b, &c, &d);

    string bits = int_to_binary_string(a) + int_to_binary_string(b) + int_to_binary_string(c) + int_to_binary_string(d);

    if (!exists(bits)) {
        insert(0, bits, root);
    } 
}

void ipbtree::insert6(string ip) {
    // @@need to get string bits from an ipv6
    
    struct sockaddr_in6 sa6;
    inet_pton(AF_INET6, ip.c_str(), &(sa6.sin6_addr));

    string bits = ipv6_address_to_bitstring(sa6);

    if (!exists(bits)) {
        
        insert(0, bits, root);
    } else {
        cout << "dupe detected" << endl << flush;
    }

}

void ipbtree::insert(int depth, string bits, node *leaf) {

    if (depth == bits.length()) {
        return;
    }

    if (bits.at(depth) == '0') {
        if (leaf->leaf0 == NULL) {

            leaf->leaf0 = new node;
            leaf->leaf0->leaf0 = NULL;
            leaf->leaf0->leaf1 = NULL;
            leaf->leaf0->count = 0;
            leaf->leaf0->value = 0;
        }
        
        leaf->count++;
        insert(depth+1, bits, leaf->leaf0);
        
    }

    if (bits.at(depth) == '1') {
        if (leaf->leaf1 == NULL) {

            leaf->leaf1 = new node;
            leaf->leaf1->leaf0 = NULL;
            leaf->leaf1->leaf1 = NULL;
            leaf->leaf1->count = 0;
            leaf->leaf1->value = 1;
        }
        
        leaf->count++;
        insert(depth+1, bits, leaf->leaf1);
    }

}

bool ipbtree::exists(string bits) {

    return exists(0, bits, root);
}

bool ipbtree::exists(int depth, string bits, node *leaf) {

    if (depth == bits.length()) {
        return true;
    }

    bool found_leaf0 = false;
    bool found_leaf1 = false;
    
    if (bits.at(depth) == '0') {
        if (leaf->leaf0 == NULL) {
        } else {
            found_leaf0 = exists(depth+1, bits, leaf->leaf0);
        }
    }

    if (bits.at(depth) == '1') {
        if (leaf->leaf1 == NULL) {
        } else {
            found_leaf1 = exists(depth+1, bits, leaf->leaf1);
        }
    }

    if (found_leaf0 == true || found_leaf1 == true) {
        return true;
    }

    return false;
}

void ipbtree::sort(bool display_cidr, bool display_network_object) {
    string buf = "";
    sort(0,buf,root, display_cidr, display_network_object);
}

void ipbtree::sort(int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object) {
    if (curdepth > 32) {
        return;
    }

    // we are sorting single IP addresses, so we only care about level 32
    if (curdepth == 32) {
        // also no subnet mask, because we're talking singles
		if (display_network_object) {
			cout << "network-object host " << string_bits_to_string_ip(buf) << endl << flush;
		} else {
	        cout << string_bits_to_string_ip(buf) << endl << flush;
		}
        return;
    }

    if (leaf->leaf0 != NULL) {
        sort (curdepth+1, buf + "0", leaf->leaf0, display_cidr, display_network_object);
    }

    if (leaf->leaf1 != NULL) {
        sort (curdepth+1, buf + "1", leaf->leaf1, display_cidr, display_network_object);
    }
    
    return;
}

void ipbtree::sort6(bool display_cidr, bool display_network_object) {
    string buf = "";
    sort6(0,buf,root, display_cidr, display_network_object);
}

void ipbtree::sort6(int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object) {


    if (curdepth > 128) {
        return;
    }

    // we are sorting single IP addresses, so we only care about level 128
    if (curdepth == 128) {

        struct sockaddr_in6 scratch6;
        char buf2[INET6_ADDRSTRLEN];

        bitstring_to_ipv6_address(buf, scratch6);
        inet_ntop(AF_INET6, &(scratch6.sin6_addr), buf2, INET6_ADDRSTRLEN);

		if (display_network_object) {
			cout << "network-object host " << buf2 << endl << flush;
		} else {
	        cout << buf2 << endl << flush;
		}

        return;
    }

    if (leaf->leaf0 != NULL) {   
        sort6(curdepth+1, buf + "0", leaf->leaf0, display_cidr, display_network_object);
    }

    if (leaf->leaf1 != NULL) {
        sort6 (curdepth+1, buf + "1", leaf->leaf1, display_cidr, display_network_object);
    }
    
    return;
}

void ipbtree::summarize(int maxdepth, bool display_cidr, bool display_network_object) {
    string buf = "";
    summarize(maxdepth, 0, buf, root, display_cidr, display_network_object);
}

void ipbtree::summarize(int maxdepth, int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object) {

    if (curdepth == maxdepth) {
        if (display_cidr) {
            cout << string_bits_to_string_ip(buf) << "/" << curdepth << endl << flush;
        } else {
			if (display_network_object) {
				cout << "network-object " << string_bits_to_string_ip(buf) << " " << cidr_to_subnetmask(curdepth) << endl << flush;
			} else {
	            cout << string_bits_to_string_ip(buf) << " " << cidr_to_subnetmask(curdepth) << endl << flush;
			}
        }
        return;
    }

    // if our count is more than half of our capacity
    if (leaf->count * 2 > pow(2, (32 - curdepth))) {
        if (display_cidr) {
            cout << string_bits_to_string_ip(buf) << "/" << curdepth << endl << flush;
        } else {
			if (display_network_object) {
				cout << "network-object " << string_bits_to_string_ip(buf) << " " << cidr_to_subnetmask(curdepth) << endl << flush;
			} else {
	            cout << string_bits_to_string_ip(buf) << " " << cidr_to_subnetmask(curdepth) << endl << flush;
			}
			
        }
        return;
    }

    if (leaf->leaf0 != NULL) {
        summarize(maxdepth, curdepth+1, buf + "0", leaf->leaf0, display_cidr, display_network_object);
    }

    if (leaf->leaf1 != NULL) {
        summarize(maxdepth, curdepth+1, buf + "1", leaf->leaf1, display_cidr, display_network_object);
    }

    return;
    
}

void ipbtree::summarize6(int maxdepth, bool display_cidr, bool display_network_object) {
    string buf = "";
    summarize6(maxdepth, 0, buf, root, display_cidr, display_network_object);
}

void ipbtree::summarize6(int maxdepth, int curdepth, string buf, node *leaf, bool display_cidr, bool display_network_object) {

    struct sockaddr_in6 scratch6;
    char buf2[INET6_ADDRSTRLEN];

    if (curdepth == maxdepth) {
        
        bitstring_to_ipv6_address(buf, scratch6);
        inet_ntop(AF_INET6, &(scratch6.sin6_addr), buf2, INET6_ADDRSTRLEN);

        cout << buf2 << "/" << curdepth << endl << flush;
        return;
    }

    // ok - pow(is) is only double.  This means this will break if try to summate more than that many ipv6's

        // if our count is more than half of our capacity
    if (leaf->count * 2 > pow(2, (128 - curdepth))) {

            
        bitstring_to_ipv6_address(buf, scratch6);
        inet_ntop(AF_INET6, &(scratch6.sin6_addr), buf2, INET6_ADDRSTRLEN);

        cout << buf2 << "/" << curdepth << endl << flush;
        return;
    }


    if (leaf->leaf0 != NULL) {
        summarize6(maxdepth, curdepth+1, buf + "0", leaf->leaf0, display_cidr, display_network_object);
    }

    if (leaf->leaf1 != NULL) {
        summarize6(maxdepth, curdepth+1, buf + "1", leaf->leaf1, display_cidr, display_network_object);
    }

    return;    
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

    // OK we don't need most of this, because "xips" doesn't work on arbitrary parts of text the way "ips" does, so
    // what we are really doing is...
    string matchstring = w1 + w2 + w3;

    if (isMaskv4(w2)) {  // only have to worry about v4 here, since we aren't doing FFFF.FFFF.... for ipv6.
        matchstring = w1 + " " + w2;    
        return matchstring;
    }


    // if the middle character is needs to be a "-", make it a "-"
    if ((w2 == "to") || (w2 == "-") || (w2 == "through") || (w2 == "thru" /* JC */)) {           
        matchstring = w1 + "-" + w3;    
        return matchstring;
    }

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

int ipv4_range_to_list(sockaddr_in &ipv4_range_start, sockaddr_in &ipv4_range_end, bool mode_summarize, bool mode_sort, ipbtree *summary_tree, bool display_network_object) {

    unsigned int start = 0;
    unsigned int end = 0;

    sockaddr_in scratch;

    start = SwapBytes(ipv4_range_start.sin_addr.s_addr);
    end = SwapBytes(ipv4_range_end.sin_addr.s_addr);


    for (unsigned int i = start; i <= end; i++) {

        scratch.sin_addr.s_addr = SwapBytes(i);
        //cout << i << endl << flush;

        if (mode_summarize || mode_sort) {
            summary_tree->insert(inet_ntoa(scratch.sin_addr));
        } else {
			if (display_network_object) {
				cout << "network-object host " << inet_ntoa(scratch.sin_addr) << endl << flush;
			} else {
				cout << inet_ntoa(scratch.sin_addr) << endl << flush;				
			}
        }

        //cout << inet_ntoa(scratch.sin_addr) << endl << flush;
    }


}

int ipv6_range_to_list(sockaddr_in6 &ipv6_range_start, sockaddr_in6 &ipv6_range_end, bool mode_summarize, bool mode_sort, ipbtree *summary_tree, bool display_network_object) {

    // oh my.
    unsigned short i0;
    unsigned short i1;
    unsigned short i2;
    unsigned short i3;
    unsigned short i4;
    unsigned short i5;
    unsigned short i6;
    unsigned short i7;
    unsigned short i8;
    unsigned short i9;
    unsigned short i10;
    unsigned short i11;
    unsigned short i12;
    unsigned short i13;
    unsigned short i14;
    unsigned short i15;

    sockaddr_in6 scratch;
    char buf[INET6_ADDRSTRLEN];

    //apologies to formatting purists
    for (i0 = ipv6_range_start.sin6_addr.s6_addr[0]; i0 <= ipv6_range_end.sin6_addr.s6_addr[0]; i0++) {
        scratch.sin6_addr.s6_addr[0] = i0;
    for (i1 = ipv6_range_start.sin6_addr.s6_addr[1]; i1 <= ipv6_range_end.sin6_addr.s6_addr[1]; i1++) {
        scratch.sin6_addr.s6_addr[1] = i1;
    for (i2 = ipv6_range_start.sin6_addr.s6_addr[2]; i2 <= ipv6_range_end.sin6_addr.s6_addr[2]; i2++) {    
        scratch.sin6_addr.s6_addr[2] = i2;
    for (i3 = ipv6_range_start.sin6_addr.s6_addr[3]; i3 <= ipv6_range_end.sin6_addr.s6_addr[3]; i3++) {
        scratch.sin6_addr.s6_addr[3] = i3;
    for (i4 = ipv6_range_start.sin6_addr.s6_addr[4]; i4 <= ipv6_range_end.sin6_addr.s6_addr[4]; i4++) {
        scratch.sin6_addr.s6_addr[4] = i4;
    for (i5 = ipv6_range_start.sin6_addr.s6_addr[5]; i5 <= ipv6_range_end.sin6_addr.s6_addr[5]; i5++) {
        scratch.sin6_addr.s6_addr[5] = i5;
    for (i6 = ipv6_range_start.sin6_addr.s6_addr[6]; i6 <= ipv6_range_end.sin6_addr.s6_addr[6]; i6++) {
        scratch.sin6_addr.s6_addr[6] = i6;
    for (i7 = ipv6_range_start.sin6_addr.s6_addr[7]; i7 <= ipv6_range_end.sin6_addr.s6_addr[7]; i7++) {
        scratch.sin6_addr.s6_addr[7] = i7;
    for (i8 = ipv6_range_start.sin6_addr.s6_addr[8]; i8 <= ipv6_range_end.sin6_addr.s6_addr[8]; i8++) {    
        scratch.sin6_addr.s6_addr[8] = i8;
    for (i9 = ipv6_range_start.sin6_addr.s6_addr[9]; i9 <= ipv6_range_end.sin6_addr.s6_addr[9]; i9++) {
        scratch.sin6_addr.s6_addr[9] = i9;
    for (i10 = ipv6_range_start.sin6_addr.s6_addr[10]; i10 <= ipv6_range_end.sin6_addr.s6_addr[10]; i10++) {
        scratch.sin6_addr.s6_addr[10] = i10;
    for (i11 = ipv6_range_start.sin6_addr.s6_addr[11]; i11 <= ipv6_range_end.sin6_addr.s6_addr[11]; i11++) {
        scratch.sin6_addr.s6_addr[11] = i11;
    for (i12 = ipv6_range_start.sin6_addr.s6_addr[12]; i12 <= ipv6_range_end.sin6_addr.s6_addr[12]; i12++) {
        scratch.sin6_addr.s6_addr[12] = i12;
    for (i13 = ipv6_range_start.sin6_addr.s6_addr[13]; i13 <= ipv6_range_end.sin6_addr.s6_addr[13]; i13++) {
        scratch.sin6_addr.s6_addr[13] = i13;
    for (i14 = ipv6_range_start.sin6_addr.s6_addr[14]; i14 <= ipv6_range_end.sin6_addr.s6_addr[14]; i14++) {    
        scratch.sin6_addr.s6_addr[14] = i14;
    for (i15 = ipv6_range_start.sin6_addr.s6_addr[15]; i15 <= ipv6_range_end.sin6_addr.s6_addr[15]; i15++) {
        scratch.sin6_addr.s6_addr[15] = i15;

        inet_ntop(AF_INET6, &(scratch.sin6_addr), buf, INET6_ADDRSTRLEN);


        if (mode_summarize || mode_sort) {
            summary_tree->insert6(buf);
        } else {
			if (display_network_object) {
				cout << "network-object host " << buf << endl << flush;
			} else {
	            cout << buf << endl << flush;
			}			
        }

        
        
    }}}}}}}}}}}}}}}}  // bam!

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


int main(int argc, char* argv[]) {

    string input = "";

	bool mode_ipv4 = true;
	bool mode_ipv6 = true;
   
	bool mode_use_old_aton_behavior = false;

    int range_type = 0; // 4, or 6. 

    struct sockaddr_in ipv4_range_start;
    struct sockaddr_in ipv4_range_end;
    struct sockaddr_in6 ipv6_range_start;
    struct sockaddr_in6 ipv6_range_end;

    struct sockaddr_in scratch;
    struct sockaddr_in6 scratch6;

    bool mode_summarize = false;
    bool mode_sort = false;

    int max_cidr = 32;
    int max_cidr_ipv6 = 64;


    bool display_cidr = false;
	bool display_network_object = false;

	std::string final_arg = "";

	//parse some arguments.  Starting at 1 because 0 is "ips"
	for ( int i = 1; i < argc; i++) {

		std::string arg = argv[i];

		if (arg == "-4") {
			mode_ipv6 = false;
			continue;
		}

		if (arg == "-6") {
			mode_ipv4 = false;
			continue;
		}

		if (arg == "--aton") {
			//because stuff like 10:1 used to be a valid way to say 10:0:0:1, but not anymore (inet_aton vs inet_pton)
			mode_use_old_aton_behavior = true;
			continue;
		}


        if (arg == "-s" || arg == "--summarize") {
            mode_summarize = true;
            continue;
        }

        if (arg == "--sort" ) {
            mode_sort = true;
            continue;
        }


        // the idea with plain -m is that you probably aren't doing both ipv4/ipv6 in the same thing, so meh
        if (arg == "-m" || arg == "--max-depth") {
            if ( i+1 < argc) {
                max_cidr = atoi(argv[i+1]);
                max_cidr_ipv6 = atoi(argv[i+1]);
                i++;
                continue;
            }
        }

        if (arg == "--max-depth-ipv4") {
            if ( i+1 < argc) {
                max_cidr = atoi(argv[i+1]);
                i++;
                continue;
            }
        }

        if (arg == "--max-depth-ipv6") {
            if ( i+1 < argc) {
                max_cidr_ipv6 = atoi(argv[i+1]);
                i++;
                continue;
            }
        }

        if (arg == "-c" || arg == "--cidr") {
            display_cidr = true;
            continue;
        }

		if (arg == "--asa") {
			display_network_object = true;
			continue;
		}

		if (arg == "-v" || arg == "--version") {
			cout << "xips v0.5  30 Apr 2018" << endl
				 << "by Eli Fulkerson.  See http://www.elifulkerson.com for updates" << endl << flush;
			return 0;
		}

		if (arg == "-h" || arg == "--help" || arg == "--usage" || arg == "?" || arg == "/?" || arg == "-?" || arg == "/h" || arg == "/H") {
			cout << "Syntax: your_command | xips [-4] [-6] [-v] " << endl
				<< endl
				<< "Eats STDIN, expands IP addresses."
				<< endl
				<< "Options:" << endl
				<< " -4     : expand IPv4 addresses only" << endl
				<< " -6     : expand IPv6 addresses only" << endl
                << " -s     : summarize, rather than expand (by default, to smallest network at > 50% utilization)" << endl
                << " --sort : expand and sort numerically" << endl
                << " -m X   : summarize to a max depth of /X (also --max-depth X)" << endl
                << " -c     : output /cidr notation rather than subnet mask" << endl
				<< " --asa  : output asa network-object notation" << endl
				<< " -v     : Display version information" << endl
                << " --max-depth-ipv4 X : summarize to a max cidr of X for IPv4 only" << endl
                << " --max-depth-ipv6 X : summarize to a max cidr of X for IPv6 only" << endl
				

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


    ipbtree *summary_tree = new ipbtree;
    ipbtree *summary_tree6 = new ipbtree;

    bool found_an_ipv4 = false;
    bool found_an_ipv6 = false;

	while (true) {

        range_type = 0;

		getline(cin, input);

        // ok, first off, if its just a singleton IP rather than an expandable range, we want to just spit it out.

        if (mode_ipv4) {
            if ((!mode_use_old_aton_behavior && inet_pton(AF_INET, input.c_str(), &(scratch.sin_addr))) || mode_use_old_aton_behavior && inet_aton(input.c_str(), &(scratch.sin_addr))) {

                if (mode_summarize || mode_sort) {
                    summary_tree->insert(input);
                } else {
					if (display_network_object) {
						cout << "network-object host " << input << endl << flush;
					} else {
						cout << input << endl << flush;
					}
                }

                found_an_ipv4 = true;

                continue;
            }
        }

        if (mode_ipv6) {
            if (inet_pton(AF_INET6, input.c_str(), &(scratch6.sin6_addr))) {
                //cout << input << endl << flush;

                if (mode_summarize || mode_sort) {
                    summary_tree6->insert6(input);
                } else {
					if (display_network_object) {
						cout << "network-object host " << input << endl << flush;
					} else {
						cout << input << endl << flush;
					}
                }

                found_an_ipv6 = true;
                continue;
            }
        }

        // we aren't actually using this here but the function demands..
        string prettystring = "";

        // ok - we actually want to use w1,w2,w3 contrary to prior beliefs, so lets just split input into them
        istringstream iss(input);


        // unlike 'ips', this is just going to be the first three ' ' delineated words on the line
        string w1 = "";
        string w2 = "";
        string w3 = "";

        getline(iss, w1, ' ');
        getline(iss, w2, ' ');
        getline(iss, w3, ' ');

        if (string_to_ipv4_range(mode_use_old_aton_behavior, w1, w2, w3, prettystring, ipv4_range_start, ipv4_range_end) == 0) {
            range_type = 4;
        }
        else {
            if (string_to_ipv6_range(w1, w2, w3, prettystring, ipv6_range_start, ipv6_range_end) == 0) {
                range_type = 6;
            }
        }

        // we didn't detect any ranges, so no error
        if (range_type == 4  && mode_ipv4) {

            ipv4_range_to_list(ipv4_range_start, ipv4_range_end, mode_summarize, mode_sort, summary_tree, display_network_object);
            found_an_ipv4 = true;
            continue;
        }

        if (range_type == 6 && mode_ipv6) {
            ipv6_range_to_list(ipv6_range_start, ipv6_range_end, mode_summarize, mode_sort, summary_tree6, display_network_object);
            found_an_ipv6 = true;
            continue;
        }

		if (!cin) {
            break;
		}
	}

    //cout << " about to summarize " << endl << flush;
    if (mode_summarize) {
        if (mode_ipv4 && found_an_ipv4) {
            summary_tree->summarize(max_cidr, display_cidr, display_network_object);
        }

        if (mode_ipv6 && found_an_ipv6) {
            summary_tree6->summarize6(max_cidr_ipv6, display_cidr, display_network_object);
        }
    }

    if (mode_sort) {
        if (mode_ipv4 && found_an_ipv4) {
            summary_tree->sort(display_cidr, display_network_object);
        }

        if (mode_ipv6 && found_an_ipv6) {
            summary_tree6->sort6(display_cidr, display_network_object);
        }
    }

    return 0;
}
