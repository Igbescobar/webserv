#!/bin/bash

# Mandatory headers separated by an empty line
echo "Content-type: text/html"
echo ""

# HTML Body
echo "<html>"
echo "<head><title>Bash CGI Status</title></head>"
echo "<body>"
echo "<h1>Bash CGI is working!</h1>"
echo "<p>Your execve and pipe setup successfully executed a Bash script.</p>"
echo "</body>"
echo "</html>"
