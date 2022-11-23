port 18002
connect
wait 1000
send I'm client1 and this is MY message
recv
wait 3000
send What did you say again to me the client1 ?
wait 100
recv
close
port 18005
connect
send I'm here again, client 1
wait 3000
recv
close