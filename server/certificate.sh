#!/bin/bash

# Create a self signed certificate using openssl

openssl genrsa -des3 -out server.key 1024
openssl req -new -key server.key -out server.csr

# remove password

cp server.key server.key.org
openssl rsa -in server.key.org -out server.key

openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt

# Generate dhparam file

openssl dhparam -out dh512.pem 512
