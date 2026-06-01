#!/usr/bin/env python3

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
    ("echo -e 'hat\\n\\hat\\n\\hat\\n127.0.0.1\\npants\\npants\\pants'", "-c 1 | wc | awk '{print $1}' | foo", 3),

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

        ("echo 206.187.99.92-93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92-93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92-93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92/93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92/93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92/93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92- 93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92- 93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92- 93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 -93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92 -93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 -93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 - 93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92 - 93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 - 93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92/ 93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92/ 93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92/ 93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 /93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92 /93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 /93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 / 93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92 / 93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 / 93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92to93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92to93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92to93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 to93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92 to93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 to93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92to 93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92to 93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92to 93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

        ("echo 206.187.99.92 to 93 \\(redacted\\)", "", 1),
        ("echo 206.187.99.92 to 93 \\(redacted\\)", "206.187.0.0-206.188.0.0", 1),
        ("echo 206.187.99.92 to 93 \\(redacted\\)", "20.187.0.0-20.188.0.0", 0),

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

rfc1918_tests = (
    # --10 positive and boundary tests
    ("echo 10.0.0.0",        "--10", 1),
    ("echo 10.0.0.1",        "--10", 1),
    ("echo 10.255.255.255",  "--10", 1),
    ("echo 9.255.255.255",   "--10", 0),
    ("echo 11.0.0.0",        "--10", 0),
    # --172 positive and boundary tests
    ("echo 172.16.0.0",      "--172", 1),
    ("echo 172.16.0.1",      "--172", 1),
    ("echo 172.31.255.255",  "--172", 1),
    ("echo 172.15.255.255",  "--172", 0),
    ("echo 172.32.0.0",      "--172", 0),
    # --192 positive and boundary tests
    ("echo 192.168.0.0",     "--192", 1),
    ("echo 192.168.0.1",     "--192", 1),
    ("echo 192.168.255.255", "--192", 1),
    ("echo 192.167.255.255", "--192", 0),
    ("echo 192.169.0.0",     "--192", 0),
)

class_tests = (
    # -A class: 1.0.0.0 - 126.255.255.255
    ("echo 1.0.0.0",         "-A", 1),
    ("echo 64.0.0.0",        "-A", 1),
    ("echo 126.255.255.255", "-A", 1),
    ("echo 0.255.255.255",   "-A", 0),
    ("echo 127.0.0.0",       "-A", 0),
    # -B class: 128.0.0.0 - 191.255.255.255
    ("echo 128.0.0.0",       "-B", 1),
    ("echo 160.0.0.0",       "-B", 1),
    ("echo 191.255.255.255", "-B", 1),
    ("echo 127.255.255.255", "-B", 0),
    ("echo 192.0.0.0",       "-B", 0),
    # -C class: 192.0.0.0 - 223.255.255.255
    ("echo 192.0.0.0",       "-C", 1),
    ("echo 200.0.0.0",       "-C", 1),
    ("echo 223.255.255.255", "-C", 1),
    ("echo 191.255.255.255", "-C", 0),
    ("echo 224.0.0.0",       "-C", 0),
    # -D / --multicast: 224.0.0.0 - 239.255.255.255
    ("echo 224.0.0.0",       "-D", 1),
    ("echo 230.0.0.0",       "-D", 1),
    ("echo 239.255.255.255", "-D", 1),
    ("echo 223.255.255.255", "-D", 0),
    ("echo 240.0.0.0",       "-D", 0),
    ("echo 224.0.0.0",       "--multicast", 1),
    # -E / --experimental: 240.0.0.0 - 254.255.255.255
    ("echo 240.0.0.0",       "-E", 1),
    ("echo 250.0.0.0",       "-E", 1),
    ("echo 254.255.255.255", "-E", 1),
    ("echo 239.255.255.255", "-E", 0),
    ("echo 255.0.0.0",       "-E", 0),
    ("echo 240.0.0.0",       "--experimental", 1),
)

hex_tests = (
    ("echo C0A80101",  "-x",       1),   # 192.168.1.1
    ("echo 7f000001",  "-x",       1),   # 127.0.0.1
    ("echo 00000000",  "-x",       1),   # 0.0.0.0 (returns 0 without -x)
    ("echo C0A80101",  "",         0),   # hex not matched without -x
    ("echo C0A80101",  "-x --192", 1),   # 192.168.1.1 in --192 range
    ("echo C0A80101",  "-x --10",  0),   # 192.168.1.1 not in --10 range
)

scope_tests = (
    ("echo fe80::1%eth0", "",             1),  # scope stripped, IP still matches
    ("echo fe80::1%eth0", "-%",           1),  # scope included in output
    ("echo fe80::1%eth0", "-6",           1),  # -6 flag with scope id
    ("echo fe80::1%eth0", "fe80::0/124",  1),  # range match with scope id present
    ("echo fe80::1%eth0", "fe90::0/124",  0),  # out of range
)

intext_tests = (
    ("echo 'the address is 192.168.1.1 end'",            "",              1),
    ("echo 'no ip here just text'",                       "",              0),
    ("echo 'address 192.168.1.1 in range'",              "192.168.1.0/24", 1),
    ("echo 'address 192.168.1.1 not in range'",          "10.0.0.0/8",    0),
    ("echo 'two ips 10.0.0.1 and 192.168.1.1 here'",    "",              1),
    ("echo 'two ips 10.0.0.1 and 192.168.1.1 here'",    "10.0.0.0/8",    1),
    ("echo 'two ips 10.0.0.1 and 192.168.1.1 here'",    "172.16.0.0/12", 0),
)

strict_tests = (
    ("echo '[192.168.1.1]'",          "",   1),  # non-strict strips brackets, matches
    ("echo '[192.168.1.1]'",          "-s", 0),  # strict: bracket-attached, no match
    ("echo '192.168.1.1'",            "-s", 1),  # standalone IP, always matches
    ("echo 'pre 192.168.1.1 post'",   "-s", 1),  # whitespace-separated, matches
    ("echo 'pre192.168.1.1post'",     "-s", 0),  # no whitespace around IP, no match
)

crlf_tests = (
    ("printf '192.168.1.1\\r\\n'", "",    1),  # CRLF line containing IP
    ("printf 'rabbit\\r\\n'",       "",    0),  # CRLF line without IP
)

# count_tests: cmd | ./ips --count | foo, expected exit = count value
count_tests = (
    ("echo 192.168.1.1",                                "--count | foo", 1),   # single IP → count 1
    ("echo rabbit",                                     "--count | foo", 0),   # no IP → count 0
    ("printf '192.168.1.1\\n10.0.0.1\\n8.8.8.8\\n'",  "--count | foo", 3),   # 3 distinct IPs
    ("printf '192.168.1.1\\n192.168.1.1\\n'",          "--count | foo", 2),   # duplicates counted (not deduped)
    ("echo '1.2.3.4 and 5.6.7.8'",                    "--count | foo", 2),   # two IPs in text
)

private_tests = (
    # IPv4 RFC1918 and special ranges
    ("echo 10.0.0.1",        "--private", 1),
    ("echo 10.0.0.0",        "--private", 1),
    ("echo 10.255.255.255",  "--private", 1),
    ("echo 172.16.0.1",      "--private", 1),
    ("echo 172.31.255.255",  "--private", 1),
    ("echo 192.168.1.1",     "--private", 1),
    ("echo 192.168.255.255", "--private", 1),
    ("echo 127.0.0.1",       "--private", 1),   # loopback
    ("echo 169.254.1.1",     "--private", 1),   # link-local
    ("echo 172.15.255.255",  "--private", 0),   # just outside 172.16/12
    ("echo 172.32.0.0",      "--private", 0),   # just outside 172.16/12
    ("echo 8.8.8.8",         "--private", 0),   # public
    ("echo 1.1.1.1",         "--private", 0),   # public
    # IPv6
    ("echo ::1",             "--private", 1),   # loopback
    ("echo fc00::1",         "--private", 1),   # unique local (fc prefix)
    ("echo fd00::1",         "--private", 1),   # unique local (fd prefix)
    ("echo fe80::1",         "--private", 1),   # link-local
    ("echo 2001:db8::1",     "--private", 0),   # public
)

public_tests = (
    ("echo 8.8.8.8",         "--public", 1),
    ("echo 1.1.1.1",         "--public", 1),
    ("echo 10.0.0.1",        "--public", 0),
    ("echo 172.16.0.1",      "--public", 0),
    ("echo 192.168.1.1",     "--public", 0),
    ("echo 127.0.0.1",       "--public", 0),
    ("echo 169.254.1.1",     "--public", 0),
    ("echo 2001:db8::1",     "--public", 1),
    ("echo ::1",             "--public", 0),
    ("echo fe80::1",         "--public", 0),
    ("echo fc00::1",         "--public", 0),
)

# mask_suppression_tests: standalone subnet masks suppressed by default, shown with --masks
mask_suppression_tests = (
    ("echo 255.255.255.0",   "",        0),   # suppressed by default
    ("echo 255.0.0.0",       "",        0),   # suppressed by default
    ("echo 255.255.0.0",     "",        0),   # suppressed by default
    ("echo 255.255.255.0",   "--masks", 1),   # shown with --masks
    ("echo 255.0.0.0",       "--masks", 1),   # shown with --masks
    # ip+mask in context mode: full line still output (triggered by the IP, not mask)
    ("echo '192.168.1.1 255.255.255.0'", "",  1),
    # ip+mask in basic mode: combined output still works
    ("echo '192.168.1.1 255.255.255.0'", "-b | wc -l | awk '{print $1}' | foo", 1),
    # plain IPs unaffected
    ("echo 192.168.1.1",     "",        1),
    ("echo 8.8.8.8",         "",        1),
)

# basic_mask_tests: in -b mode, "ip mask" should be one output line, not two
basic_mask_tests = (
    # line count: ip+mask → 1 line (not 2 separate lines)
    ("echo '192.168.1.1 255.255.255.0'", "-b | wc -l | awk '{print $1}' | foo", 1),
    ("echo '10.0.0.1 255.0.0.0'",        "-b | wc -l | awk '{print $1}' | foo", 1),
    ("echo '8.8.8.8'",                   "-b | wc -l | awk '{print $1}' | foo", 1),
    # mask appears in output as part of the combined line
    ("echo '192.168.1.1 255.255.255.0'", "-b | grep '255.255.255.0'",           0),
    # mask is NOT emitted as a standalone line
    ("echo '192.168.1.1 255.255.255.0'", "-b | grep '^255.255.255.0$'",         1),
    ("echo '10.0.0.1 255.0.0.0'",        "-b | grep '^255.0.0.0$'",             1),
)

# link_local_tests: --link-local passes fe80::/10, rejects others; IPv4 always rejected
link_local_tests = (
    # fe80::/10 addresses pass
    ("echo fe80::1",                          "--link-local",     1),
    ("echo fe80::a8bb:ccff:fedd:eeff",        "--link-local",     1),
    # non-link-local IPv6 rejected
    ("echo 2001:db8::1",                      "--link-local",     0),
    ("echo ::1",                              "--link-local",     0),
    ("echo fc00::1",                          "--link-local",     0),
    # IPv4 always rejected
    ("echo 192.168.1.1",                      "--link-local",     0),
    ("echo 169.254.1.1",                      "--link-local",     0),
    # count: two link-local out of mixed input
    ("echo 'fe80::1 fe80::2 2001:db8::1 192.168.1.1'", "--link-local --count | foo", 2),
)

# link_local_mac_tests: --link-local-mac requires EUI-64 FF:FE bytes at positions 11-12
link_local_mac_tests = (
    # has FF:FE → passes
    ("echo fe80::a8bb:ccff:fedd:eeff",        "--link-local-mac", 1),
    ("echo fe80::200:5eff:fe00:5301",         "--link-local-mac", 1),
    # fe80 but no FF:FE → fails
    ("echo fe80::1",                          "--link-local-mac", 0),
    ("echo fe80::dead:beef",                  "--link-local-mac", 0),
    # non-link-local → fails regardless
    ("echo 2001:db8::a8bb:ccff:fedd:eeff",   "--link-local-mac", 0),
    # IPv4 → fails
    ("echo 192.168.1.1",                      "--link-local-mac", 0),
    # count
    ("echo 'fe80::a8bb:ccff:fedd:eeff fe80::1 fe80::200:5eff:fe00:5301'", "--link-local-mac --count | foo", 2),
)

# file_tests: (content_cmd, ips_args, expected_exit)
# generated as: content_cmd > /tmp/ips_test.txt && ./ips -f /tmp/ips_test.txt args
file_tests = (
    ("echo 192.168.1.1",                      "",              1),
    ("echo rabbit",                            "",              0),
    ("echo 192.168.1.1",                       "192.168.1.0/24", 1),
    ("echo 192.168.1.1",                       "10.0.0.0/8",    0),
    ("echo 'the address is 10.0.0.1 here'",   "--10",          1),
    ("echo 'the address is 11.0.0.1 here'",   "--10",          0),
)


# ---------------------------------------------------------------------------
# --cidr: filter extracted IPs to those within a CIDR (repeatable, match any)
# ---------------------------------------------------------------------------
cidr_filter_tests = (
    # match only IPs within 192.168.1.0/24
    ("printf '10.0.0.1\\n192.168.1.50\\n172.16.5.1\\n8.8.8.8\\n'",
     "--cidr 192.168.1.0/24 -b | wc -l | awk '{if ($1 == 1) exit 0; else exit 1}'", 0),
    # address inside passes
    ("echo '192.168.1.50'", "--cidr 192.168.1.0/24 -b", 1),
    # address outside is rejected
    ("echo '10.0.0.1'",     "--cidr 192.168.1.0/24 -b", 0),
    # multiple --cidr flags: match any
    ("printf '10.0.0.1\\n192.168.1.50\\n8.8.8.8\\n'",
     "--cidr 192.168.1.0/24 --cidr 10.0.0.0/8 -b | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    # IPv6 cidr filter: match only link-local
    ("printf 'fe80::1\\n2001:db8::1\\n::1\\n'",
     "--cidr fe80::/10 -b | wc -l | awk '{if ($1 == 1) exit 0; else exit 1}'", 0),
    ("echo 'fe80::1'",  "--cidr fe80::/10 -b", 1),
    ("echo '2001:db8::1'", "--cidr fe80::/10 -b", 0),
    # exact single-IP CIDR filter
    ("printf '1.2.3.4\\n1.2.3.5\\n'",
     "--cidr 1.2.3.4/32 -b", 1),  # 1.2.3.4 matches; 1.2.3.5 does not but output is non-empty
)

v4mapped_tests = (
    # auto-behavior: ::ffff:x.x.x.x is detected and output in native form
    ("printf '::ffff:10.0.0.1\\n2001:db8::1\\n'",
     "-b | grep -Fx '::ffff:10.0.0.1'", 0),  # mapped addr present in output
    ("printf '::ffff:10.0.0.1\\n2001:db8::1\\n'",
     "-b | grep -Fx '2001:db8::1'", 0),       # non-mapped also present
    # --v4mapped filter: only output v4mapped addresses
    ("printf '::ffff:10.0.0.1\\n2001:db8::1\\n::ffff:8.8.8.8\\n'",
     "--v4mapped -b | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    ("echo '2001:db8::1'",      "--v4mapped -b", 0),   # non-mapped: filtered out
    ("echo '::ffff:10.0.0.1'",  "--v4mapped -b", 1),   # mapped: matches (intent 1 = should match)
    # --private uses embedded IPv4 for v4mapped
    ("printf '::ffff:10.0.0.1\\n::ffff:8.8.8.8\\n'",
     "--private -b | grep -Fx '::ffff:10.0.0.1'", 0),  # 10.x.x.x is private: passes
    ("printf '::ffff:10.0.0.1\\n::ffff:8.8.8.8\\n'",
     "--private -b | grep -Fx '::ffff:8.8.8.8'", 1),   # 8.8.8.8 is public: filtered
    # --public uses embedded IPv4 for v4mapped
    ("printf '::ffff:10.0.0.1\\n::ffff:8.8.8.8\\n'",
     "--public -b | grep -Fx '::ffff:8.8.8.8'", 0),    # public: passes
    ("printf '::ffff:10.0.0.1\\n::ffff:8.8.8.8\\n'",
     "--public -b | grep -Fx '::ffff:10.0.0.1'", 1),   # private: filtered
    # --cidr with IPv4 range matches v4mapped via embedded address
    ("printf '::ffff:10.0.0.1\\n::ffff:10.0.0.200\\n::ffff:192.168.1.1\\n'",
     "--cidr 10.0.0.0/24 -b | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    # non-mapped IPv6 filtered out by v4-only --cidr
    ("echo '2001:db8::1'", "--cidr 10.0.0.0/24 -b", 0),
    # IPv4 range positional arg matches v4mapped via embedded address
    ("printf '::ffff:10.0.0.1\\n::ffff:10.0.0.200\\n'",
     "10.0.0.0/24 -b | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    ("echo '::ffff:192.168.1.1'", "10.0.0.0/24 -b", 0),  # not in 10/24: filtered
)


tunnel_tests = (
    # 6to4: 2002::/16 prefix; IPv4 embedded in bytes 2-5
    # 2002:0a00:0001:: embeds 10.0.0.1; 2002:c000:0201:: embeds 192.0.2.1
    ("printf '2002:c000:0201::1\\n2002:0a00:0001::1\\n2001:db8::1\\n'",
     "-b | grep -Fx '2002:c000:0201::1'", 0),   # 6to4 addr present without flag
    ("printf '2002:c000:0201::1\\n2002:0a00:0001::1\\n2001:db8::1\\n'",
     "--6to4 -b | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),  # only 2 6to4
    ("echo '2001:db8::1'",        "--6to4 -b", 0),   # non-6to4: filtered
    ("echo '2002:0a00:0001::1'",  "--6to4 -b", 1),   # 6to4: passes
    # --private uses embedded IPv4: 2002:0a00:0001:: embeds 10.0.0.1 (private)
    ("printf '2002:c000:0201::1\\n2002:0a00:0001::1\\n'",
     "--private -b | grep -Fx '2002:0a00:0001::1'", 0),   # 10.x is private
    ("printf '2002:c000:0201::1\\n2002:0a00:0001::1\\n'",
     "--private -b | grep -Fx '2002:c000:0201::1'", 1),   # 192.0.2.x is public
    # IPv4 range filter uses embedded IPv4
    ("printf '2002:c000:0201::1\\n2002:0a00:0001::1\\n'",
     "10.0.0.0/8 -b | grep -Fx '2002:0a00:0001::1'", 0),  # embeds 10.0.0.1: match
    ("printf '2002:c000:0201::1\\n2002:0a00:0001::1\\n'",
     "10.0.0.0/8 -b | grep -Fx '2002:c000:0201::1'", 1),  # embeds 192.0.2.1: no match
    # Teredo: 2001:0000::/32 prefix; client IPv4 in bytes 12-15 XOR 0xFF
    # 2001::f5ff:fffe embeds 10.0.0.1 (0x0a000001 XOR ff = 0xf5fffffe)
    ("printf '2001::f5ff:fffe\\n2001:db8::1\\n'",
     "--teredo -b | grep -Fx '2001::f5ff:fffe'", 0),   # Teredo: passes
    ("echo '2001:db8::1'",   "--teredo -b", 0),         # not Teredo (bytes 2-3 != 0x0000)
    ("printf '2001::f5ff:fffe\\n'",
     "--private -b", 1),    # embedded 10.0.0.1 is private
    # --6to4 --teredo combined: show both types (OR)
    ("printf '2002:c000:0201::1\\n2001::f5ff:fffe\\n2001:db8::1\\n'",
     "--6to4 --teredo -b | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
)


sort_unique_tests = (
    # --sort: smallest IPv4 first
    ("printf '10.0.0.5\\n10.0.0.1\\n10.0.0.3\\n'",
     "--sort | head -1 | grep -Fx '10.0.0.1'", 0),
    # --sort: duplicates kept
    ("printf '10.0.0.1\\n10.0.0.1\\n'",
     "--sort | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    # --sort: IPv4 sorted before IPv6
    ("printf '::1\\n10.0.0.1\\n'",
     "--sort | head -1 | grep -Fx '10.0.0.1'", 0),
    # --unique: dedup streaming, count reduced
    ("printf '10.0.0.1\\n10.0.0.1\\n10.0.0.2\\n'",
     "--unique | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    # --unique: first occurrence kept (appearance order)
    ("printf '10.0.0.5\\n10.0.0.1\\n10.0.0.5\\n'",
     "--unique | head -1 | grep -Fx '10.0.0.5'", 0),
    # --uniq: alias for --unique
    ("printf '1.1.1.1\\n1.1.1.1\\n2.2.2.2\\n'",
     "--uniq | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    # --sort --unique: sorted and deduplicated
    ("printf '10.0.0.3\\n10.0.0.1\\n10.0.0.3\\n'",
     "--sort --unique | wc -l | awk '{if ($1 == 2) exit 0; else exit 1}'", 0),
    ("printf '10.0.0.3\\n10.0.0.1\\n10.0.0.3\\n'",
     "--sort --unique | head -1 | grep -Fx '10.0.0.1'", 0),
    # --sort with IPv6: smallest ::1 before 2001:db8::
    ("printf '2001:db8::2\\n::1\\n2001:db8::1\\n'",
     "--sort | head -1 | grep -Fx '::1'", 0),
    # --sort --count: correct count
    ("printf '10.0.0.1\\n10.0.0.2\\n10.0.0.3\\n'",
     "--sort --count | foo", 3),
    # --sort --unique --count: deduplicated count
    ("printf '10.0.0.1\\n10.0.0.1\\n10.0.0.2\\n'",
     "--sort --unique --count | foo", 2),
)


# ips uses grep-style exit codes: 0 = at least one match found, 1 = no match.
# For piped tests (args contain "| foo" / "| awk"), $? is the downstream command's
# exit code, so the tuple's `expected` is used verbatim. For direct tests, the tuple's
# `expected` is the match intent (1 = should match, 0 = should not) and is translated
# to the grep exit code here.
def grep_exit(args, expected):
    if '|' in args:
        return expected
    return 0 if expected else 1


def run_test(cmd, args, expected):
    print(cmd, "|./ips", args, "> /dev/null")
    print("if [ $? -eq", grep_exit(args, expected), "]; then\ntrue\nelse\n\techo FAIL.  Test was",
          cmd.replace('|', "pipe"), "with args:", args.replace('|', "pipe"), "\nfi\n\n")


def main(argv=None):
    print("#!/bin/bash")
    print("")
    print("foo() { return $(cat); }")

    print("\n# --- original tests ---")
    for t in tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- RFC1918 shortcut tests ---")
    for t in rfc1918_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- IP class shortcut tests ---")
    for t in class_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- hex IPv4 tests ---")
    for t in hex_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- IPv6 scope ID tests ---")
    for t in scope_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- IP embedded in text tests ---")
    for t in intext_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- strict mode tests ---")
    for t in strict_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- CRLF handling tests ---")
    for t in crlf_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --count mode tests ---")
    for t in count_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --private filter tests ---")
    for t in private_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --public filter tests ---")
    for t in public_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- mask suppression tests ---")
    for t in mask_suppression_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- basic mode ip+mask tests ---")
    for t in basic_mask_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --link-local filter tests ---")
    for t in link_local_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --link-local-mac filter tests ---")
    for t in link_local_mac_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --cidr filter tests ---")
    for t in cidr_filter_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --v4mapped / v4mapped auto-behavior tests ---")
    for t in v4mapped_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --6to4 / --teredo embedded-IPv4 tests ---")
    for t in tunnel_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- --sort / --unique / --uniq tests ---")
    for t in sort_unique_tests:
        run_test(t[0], t[1], t[2])

    print("\n# --- file input (-f) tests ---")
    for t in file_tests:
        print(t[0], "> /tmp/ips_test.txt && ./ips -f /tmp/ips_test.txt", t[1], "> /dev/null")
        print("if [ $? -eq", grep_exit(t[1], t[2]), "]; then\ntrue\nelse\n\techo FAIL.  File test was",
              t[0], "with args:", t[1], "\nfi\n\n")

    print("echo Test Complete")

    return 0        # success

if __name__ == '__main__':
    status = main()
    sys.exit(status)
