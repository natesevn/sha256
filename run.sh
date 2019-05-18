sudo apt-get install libssl-dev
g++ sha256.cpp main.cpp -I./ -g -lcrypto -o hmac 

echo "Starting code. Press 1 to get HMAC. Make sure you remember the password and message."
./hmac

echo " "
echo "Starting code. Press 2 to verify HMAC. Enter password, message, and previously generated HMAC."
./hmac
