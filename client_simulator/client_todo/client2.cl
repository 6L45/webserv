port 18005
wait 3000
connect
send Im client 2 and this is my message
recv
wait 1000
send What did you say again? ps: client 2
wait 200
recv
close
port 18002
connect
send I'm here, it's me client 2
wait 300
recv
close