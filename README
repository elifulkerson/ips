IPS(1)                           User Commands                           IPS(1)

# NAME

**ips** — grep for IP addresses

# SYNOPSIS

**ips** \[**-4**] \[**-6**] \[**-v**] \[**-%**] \[**-s**] \[**-q**] \[**-c** *lines*] \[**-f** *file*] \[*ip or range*]

*your_command* | **ips** \[*options*] \[*ip or range*]

# DESCRIPTION

**ips** reads arbitrary text from standard input (or from files) and **finds**
the IP addresses in it. By default it behaves like **grep**(1): it prints the
lines that contain an IP address. Supplying an IP, range, subnet, or address
category as a filter restricts matching to those addresses. The output can be
reshaped into basic one-per-line form, annotated with line numbers or context,
counted, sorted, or deduplicated. Both IPv4 and IPv6 are supported.

**ips** is the extractor counterpart to **xips**(1), which *transforms*
addresses you already have (expanding ranges, summarizing into CIDR, converting
formats). **ips** pulls addresses *out* of surrounding text.

# OPTIONS

## Address family

**-4**
: Match IPv4 addresses only.

**-6**
: Match IPv6 addresses only.

**-%**
: Include the IPv6 scope (zone id) when matching.

## Matching behavior

**-s**
: Strict — require IPs to be surrounded by whitespace.

**-x**
: Match "hex IPv4" addresses, for instance *FFFFFFFF*.

**--aton**
: Use "old" **inet_aton**(3) behavior; for instance, *10.5* is another name
  for *10.0.0.5*.

**--masks**
: Allow subnet masks to be shown as standalone IPs (suppressed by default).

## Output shaping

**-b**, **--basic**
: Basic mode — one IP per line, no surrounding context.

**-n**
: Output line numbers.

**-q**
: Quiet — produce no output (use with the exit status).

**-c** *X*, **--context** *X*
: Print *X* lines of context around each match.

**--cisco**
: When printing context, also include the last non-indented line (e.g. the
  Cisco configuration section header).

**--count**
: Count matching IPs and print the total instead of listing them.

**--sort**
: Collect all matching IPs and output them in numeric order (IPv4 then IPv6).

**--unique**, **--uniq**
: Deduplicate IPs in order of appearance.

## Filtering by category

**--private**
: Only output private / RFC1918 addresses.

**--public**
: Only output non-private addresses.

**--cidr** *R*
: Only output IPs within CIDR *R* (may be repeated; matches any).

**--link-local**
: Only IPv6 link-local addresses (*fe80::/10*).

**--link-local-mac**
: Only link-local addresses with EUI-64 MAC-derived interface IDs.

**--v4mapped**
: Only IPv4-mapped IPv6 addresses (*::ffff:x.x.x.x*).

**--6to4**
: Only 6to4 IPv6 addresses (*2002::/16*; embeds IPv4 in bytes 2–5).

**--teredo**
: Only Teredo IPv6 addresses (*2001::/32*; embeds the client IPv4 in bytes
  12–15 XOR 0xFF).

## Input and miscellaneous

**-f** *file*, **--file** *file*
: Read input from a file instead of stdin (may be repeated).

**-v**
: Display version information.

# SHORTHAND MATCHERS

**-A**, **-B**, **-C**
: Match the classical Class A, B, or C address ranges.

**-D**, **--multicast**
: Match Class D (multicast) addresses.

**-E**, **--experimental**
: Match Class E (experimental) addresses.

**--loopback**
: Match 127.0.0.0 through 127.255.255.255.

**--10**, **--172**, **--192**
: Match the RFC 1918 ranges 10.0.0.0/8, 172.16.0.0/12, and 192.168.0.0/16.

# FILTER ARGUMENT SYNTAX

A trailing filter argument (or a **--cidr** value) may be any of:

**192.168.1.1**
: A single address.

**192.168.1.1-192.168.1.255**
: An inclusive hyphenated range.

**192.168.1.0/24**
: A CIDR subnet.

**192.168.1.1/C**
: Class A/B/C shorthand prefix.

**'192.168.1.1 255.255.255.0'**
: An address with a dotted subnet mask.

**192.168.1.5-15**
: A short hyphenated range.

# EXIT STATUS

**ips** follows **grep**(1) conventions:

**0**
: At least one matching IP was found.

**1**
: No matching IP was found.

This makes **ips** convenient in shell conditionals, e.g. `if ips -q ...; then`.

# EXAMPLES

Grep lines that contain an IP:

```
$ printf "server 10.0.0.5 up\nno ip here\ngateway 192.168.1.1\n" | ips
server 10.0.0.5 up
gateway 192.168.1.1
```

Basic mode — one IP per line:

```
$ printf "two ips 10.0.0.5 and 8.8.8.8 here\n" | ips -b
10.0.0.5
8.8.8.8
```

Show line numbers:

```
$ printf "a 1.2.3.4\nb nothing\nc 5.6.7.8\n" | ips -n
     1  a 1.2.3.4
     3  c 5.6.7.8
```

Match only a CIDR:

```
$ printf "10.0.0.5\n192.168.1.1\n8.8.8.8\n" | ips --cidr 10.0.0.0/8 -b
10.0.0.5
```

Count matches:

```
$ printf "10.0.0.5 10.0.0.6\n8.8.8.8\n" | ips --count
3
```

Filter to private addresses:

```
$ printf "10.0.0.5\n8.8.8.8\n192.168.1.1\n" | ips --private -b
10.0.0.5
192.168.1.1
```

Use the exit status in a script:

```
$ printf "no ips here\n" | ips -q; echo "exit=$?"
exit=1
```

# SEE ALSO

**xips**(1) — expand, summarize, sort, and convert IP addresses you already
have.

<http://www.elifulkerson.com>

# AUTHOR

Written by Eli Fulkerson.

IPS(1)                              ips v0.8                              IPS(1)
