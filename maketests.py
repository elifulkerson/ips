#!/usr/bin/env python

"""
Module docstring.
"""
import sys

"""
to fix - I'm not happy with thisngs that resulte in "invalid range string specified" on the input, I should be able to work w/out quotes and shit.
"""

tests = (
        ("echo 00000000", "", 0),
        ("echo t...%20", "", 0),
    ("echo 127.0.0.1", "", 1),
    ("echo rabbit", "", 0),
    ("echo 127.0.0.1", "-6", 0),
    ("echo 127.0.0.1", "-4", 1),
    ("echo ::1", "-6", 1),
    ("echo ::1", "-4", 0),
    ("echo 127.0.0.1", "--localhost", 1),
    ("echo 127.20.20.20", "--localhost", 1),
    ("echo 127.255.255.255", "--localhost",1),
    ("echo 128.0.0.0", "--localhost",0),
    ("echo 126.255.255.255", "--localhost", 0),
    ("echo bbbb::0000", "--10", 0),
    ("echo bbbb::0000", "--172", 0),
    ("echo bbbb::0000", "--192", 0),
    ("echo -e 'hat\\n\hat\\n\hat\\n127.0.0.1\\npants\\npants\\pants'", "-c 1 | wc | awk '{print $1'} | foo", 3),

        ("echo 192.168.5.5", "192.168.4.0/24", 0),
        ("echo 192.168.5.5", "192.168.5.0/24", 1),
        ("echo 192.168.5.5", "192.168.6.0/24", 0),
        ("echo 192.168.5.5", "192.168.5.0/30", 0),
        ("echo 192.168.5.5", "192.168.5.4/30", 1),
        ("echo 192.168.5.5", "192.168.5.8/30", 0),
        ("echo 192.168.5.5", "'192.168.4.0 255.255.255.0'", 0),
        ("echo 192.168.5.5", "'192.168.5.0 255.255.255.0'", 1),
        ("echo 192.168.5.5", "'192.168.6.0 255.255.255.0'", 0),
        ("echo 192.168.5.5", "'192.168.5.0 255.255.255.252'", 0),
        ("echo 192.168.5.5", "'192.168.5.4 255.255.255.252'", 1),
        ("echo 192.168.5.5", "'192.168.5.8 255.255.255.252'", 0),
        ("echo 192.168.5.5", "192.168.4.0/C", 0),
        ("echo 192.168.5.5", "192.168.5.0/C", 1),
        ("echo 192.168.5.5", "192.168.6.0/C", 0),

        ("echo 192.168.5.5", "192.168.5.0-192.168.5.4", 0),
        ("echo 192.168.5.5", "192.168.5.0-192.168.5.5", 1),
        ("echo 192.168.5.5", "192.168.5.0-192.168.5.6", 1),
        ("echo 192.168.5.5", "192.168.5.5-192.168.5.6", 1),
        ("echo 192.168.5.5", "192.168.5.6-192.168.5.7", 0),
        ("echo 192.168.5.5", "0.0.0.0-255.255.255.255", 1),
        ("echo 192.168.5.5", "192.168.5.5-192.168.5.5", 1),
        ("echo 192.168.5.5", "192.168.5.5", 1),

        ("echo 192.168.5.0 255.255.255.0", "192.168.5.5", 1),
        ("echo 192.168.5.0 255.255.255.0", "192.168.4.255", 0),
        ("echo 192.168.5.0 255.255.255.0", "192.168.5.0", 1),
        ("echo 192.168.5.0 255.255.255.0", "192.168.5.255", 1),
        ("echo 192.168.5.0 255.255.255.0", "192.168.6.0", 0),

        ("echo 192.168.5.0/24", "192.168.5.5", 1),
        ("echo 192.168.5.0/24", "192.168.4.255", 0),
        ("echo 192.168.5.0/24", "192.168.5.0", 1),
        ("echo 192.168.5.0/24", "192.168.5.1", 1),
        ("echo 192.168.5.0/24", "192.168.5.255", 1),
        ("echo 192.168.5.0/24", "192.168.6.0", 0),

        ("echo 192.168.5.0/C", "192.168.5.5", 1),
        ("echo 192.168.5.0/C", "192.168.4.255", 0),
        ("echo 192.168.5.0/C", "192.168.5.0", 1),
        ("echo 192.168.5.0/C", "192.168.5.1", 1),
        ("echo 192.168.5.0/C", "192.168.5.255", 1),
        ("echo 192.168.5.0/C", "192.168.6.0", 0),

        ("echo 192.168.5.0-192.168.5.255", "192.168.5.5", 1),
        ("echo 192.168.5.0-192.168.5.255", "192.168.4.255", 0),
        ("echo 192.168.5.0-192.168.5.255", "192.168.5.0", 1),
        ("echo 192.168.5.0-192.168.5.255", "192.168.5.1", 1),
        ("echo 192.168.5.0-192.168.5.255", "192.168.5.255", 1),
        ("echo 192.168.5.0-192.168.5.255", "192.168.6.0", 0),

        ("echo 192.168.5.0- 192.168.5.255", "192.168.5.5", 1),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.4.255", 0),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.5.0", 1),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.5.1", 1),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.5.255", 1),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.6.0", 0),

        ("echo 192.168.5.0 -192.168.5.255", "192.168.5.5", 1),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.4.255", 0),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.5.0", 1),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.5.1", 1),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.5.255", 1),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.6.0", 0),

        ("echo 192.168.5.0 - 192.168.5.255", "192.168.5.5", 1),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.4.255", 0),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.5.0", 1),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.5.1", 1),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.5.255", 1),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.6.0", 0),

        ("echo 192.168.5.0 to 192.168.5.255", "192.168.5.5", 1),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.4.255", 0),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.5.0", 1),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.5.1", 1),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.5.255", 1),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.6.0", 0),


        ("echo 192.168.5.0 to 192.168.5.255", "192.168.0.0/16", 1),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.0.0/24", 0),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.0.0/C", 0),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.0.0/B", 1),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.0.0-192.168.255.255", 1),
        ("echo 192.168.5.0 to 192.168.5.255", "192.168.0.0-192.168.4.255", 0),
        ("echo 192.168.5.0 to 192.168.5.255", "'192.168.0.0 255.255.255.0'", 0),
        ("echo 192.168.5.0 to 192.168.5.255", "'192.168.0.0 255.255.0.0'", 1),

        ("echo 192.168.5.0 - 192.168.5.255", "192.168.0.0/16", 1),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.0.0/24", 0),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.0.0/C", 0),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.0.0/B", 1),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.0.0-192.168.255.255", 1),
        ("echo 192.168.5.0 - 192.168.5.255", "192.168.0.0-192.168.4.255", 0), 
        ("echo 192.168.5.0 - 192.168.5.255", "'192.168.0.0 255.255.255.0'", 0),
        ("echo 192.168.5.0 - 192.168.5.255", "'192.168.0.0 255.255.0.0'", 1),

        ("echo 192.168.5.0- 192.168.5.255", "192.168.0.0/16", 1),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.0.0/24", 0),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.0.0/C", 0),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.0.0/B", 1),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.0.0-192.168.255.255", 1),
        ("echo 192.168.5.0- 192.168.5.255", "192.168.0.0-192.168.4.255", 0),
        ("echo 192.168.5.0- 192.168.5.255", "'192.168.0.0 255.255.255.0'", 0),
        ("echo 192.168.5.0- 192.168.5.255", "'192.168.0.0 255.255.0.0'", 1),
        
        ("echo 192.168.5.0 -192.168.5.255", "192.168.0.0/16", 1),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.0.0/24", 0),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.0.0/C", 0),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.0.0/B", 1),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.0.0-192.168.255.255", 1),
        ("echo 192.168.5.0 -192.168.5.255", "192.168.0.0-192.168.4.255", 0),
        ("echo 192.168.5.0 -192.168.5.255", "'192.168.0.0 255.255.255.0'", 0),
        ("echo 192.168.5.0 -192.168.5.255", "'192.168.0.0 255.255.0.0'", 1),        

        ("echo 192.168.5.0 255.255.255.0", "192.168.0.0/16", 1),
        ("echo 192.168.5.0 255.255.255.0", "192.168.0.0/24", 0),
        ("echo 192.168.5.0 255.255.255.0", "192.168.0.0/C", 0),
        ("echo 192.168.5.0 255.255.255.0", "192.168.0.0/B", 1),
        ("echo 192.168.5.0 255.255.255.0", "192.168.0.0-192.168.255.255", 1),
        ("echo 192.168.5.0 255.255.255.0", "192.168.0.0-192.168.4.255", 0),
        ("echo 192.168.5.0 255.255.255.0", "'192.168.0.0 255.255.255.0'", 0),
        ("echo 192.168.5.0 255.255.255.0", "'192.168.0.0 255.255.0.0'", 1),

        ("echo fe70::5", "fe80::0/124", 0),
        ("echo fe80::5", "fe80::0/124", 1),
        ("echo fe90::5", "fe80::0/124", 0),
        ("echo fe80::5", "fe80::0-fe80::4", 0),
        ("echo fe80::5", "fe80::0-fe80::5", 1),
        ("echo fe80::5", "fe80::0-fe80::6", 1),
        ("echo fe80::5", "fe80::5-fe80::5", 1),
        ("echo fe80::5", "fe80::5-fe80::6", 1),
        ("echo fe80::5", "fe80::6-fe80::7", 0),

        ("echo fe80::5/128", "fe80::0-fe80::4", 0),
        ("echo fe80::5/128", "fe80::4-fe80::6", 1),
        ("echo fe80::5/128", "fe80::6-fe80::7", 0),

        ("echo fe80::5-fe80::5", "fe80::0-fe80::4", 0),
        ("echo fe80::5-fe80::5", "fe80::4-fe80::6", 1),
        ("echo fe80::5-fe80::5", "fe80::6-fe80::7", 0),

        ("echo fe80::5 -fe80::5", "fe80::0-fe80::4", 0),
        ("echo fe80::5 -fe80::5", "fe80::4-fe80::6", 1),
        ("echo fe80::5 -fe80::5", "fe80::6-fe80::7", 0),

        ("echo fe80::5- fe80::5", "fe80::0-fe80::4", 0),
        ("echo fe80::5- fe80::5", "fe80::4-fe80::6", 1),
        ("echo fe80::5- fe80::5", "fe80::6-fe80::7", 0),

        ("echo fe80::5 to fe80::5", "fe80::0-fe80::4", 0),
        ("echo fe80::5 to fe80::5", "fe80::4-fe80::6", 1),
        ("echo fe80::5 to fe80::5", "fe80::6-fe80::7", 0),

        ("echo 206.187.99.92-93 \(redacted\)", "", 1),
        ("echo 206.187.99.92-93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92-93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92/93 \(redacted\)", "", 1),
        ("echo 206.187.99.92/93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92/93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),
        
        ("echo 206.187.99.92- 93 \(redacted\)", "", 1),
        ("echo 206.187.99.92- 93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92- 93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 -93 \(redacted\)", "", 1),
        ("echo 206.187.99.92 -93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 -93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 - 93 \(redacted\)", "", 1),
        ("echo 206.187.99.92 - 93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 - 93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92/ 93 \(redacted\)", "", 1),
        ("echo 206.187.99.92/ 93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92/ 93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 /93 \(redacted\)", "", 1),
        ("echo 206.187.99.92 /93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 /93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 / 93 \(redacted\)", "", 1),
        ("echo 206.187.99.92 / 93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 / 93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92to93 \(redacted\)", "", 1),
        ("echo 206.187.99.92to93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92to93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 to93 \(redacted\)", "", 1),
        ("echo 206.187.99.92 to93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 to93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92to 93 \(redacted\)", "", 1),
        ("echo 206.187.99.92to 93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92to 93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 to 93 \(redacted\)", "", 1),
        ("echo 206.187.99.92 to 93 \(redacted\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 to 93 \(redacted\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 192.168.1.10-30", "192.168.1.10", 1),
        ("echo 192.168.1.10-30", "192.168.1.30", 1),
        ("echo 192.168.1.10-30", "192.168.1.5", 0),
        ("echo 192.168.1.10-30", "192.168.1.100", 0),
        ("echo 192.168.1.10-30", "192.168.1.5-7", 0),
        ("echo 192.168.1.10-30", "192.168.1.5-15", 1),
        ("echo 192.168.1.10-30", "192.168.1.10-30", 1),
        ("echo 192.168.1.10-30", "192.168.1.45-55", 0),
        ("echo 192.168.1.10-30", "192.168.1.15-25", 1),
        ("echo 192.168.1.10-30", "192.168.1.25-35", 1),

)

def main(argv=None):
	
	print "#!/bin/bash"
	print ""
	print "foo() { return $(cat); }"

        for t in tests:
     	        print t[0], "|./ips.exe", t[1], "> /dev/null"
    	        #print "if [ $? -eq", t[2], "]; then\necho OK\nelse\n\techo FAIL.  Test was", t[0].replace('|', "pipe"), "with args:", t[1].replace('|', "pipe"),"\nfi\n\n"
                print "if [ $? -eq", t[2], "]; then\ntrue\nelse\n\techo FAIL.  Test was", t[0].replace('|', "pipe"), "with args:", t[1].replace('|', "pipe"),"\nfi\n\n"

        print "echo Test Complete"



        # bonus -s ignoring whitespace tests
#        for t in tests:
#     	        print t[0], "t |./ips.exe", t[1], "> /dev/null"
#    	        print "if [ $? -eq", t[2], "]; then\necho OK\nelse\n\techo FAIL.  Test was", t[0].replace('|', "pipe"), "with args:", t[1].replace('|', "pipe"),"\nfi\n\n"
#                
        return 0        # success
                
if __name__ == '__main__':
        status = main()
	sys.exit(status)