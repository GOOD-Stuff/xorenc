# xorenc
Encryption using XOR. 

This project is a laboratory work on the subject "Information Security".

#HOWTO

First of all you must to create a files with plain text and key's text. To get the encrypted text, you need to run the following command: ./xor -k keys.txt -t text.txt -e encrypt.txt

Then you will see the following:

        Nice to meet you! Let's start!     
	    Your key file will be: keys.txt
	    Your plaintext file will be: text.txt
        Your encrypted file will be: encrypt.txt
    Your key:		юаяе
    Your plain text:	будь
    Your encr text:		s{y

At the output of the program you will receive a `encrypt.txt` file with the decrypted text.

The key `-e` is not required, in this case, you receive a `encr.txt` file.
