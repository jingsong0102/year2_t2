##change your ip address and preferred port in server-in.txt and client-in.txt respectively
##similarly for other test cases: client-in-long-msg.txt, client-in-invalid-cmd.txt, client-in-invalid-msg-length.txt, 
#step 1: on one command prompt, type in and run 
c:\yourdir> echoserver.exe < server-in.txt

#step 2: on the other command prompt, type in and run 
c:\yourdir> echocli.exe < client-in.txt
