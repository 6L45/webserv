port 18005
wait 2000
connect
send Im client 2 and this is my message
recv
send What did you say again? ps: client 2
recv
wait 2000
close
port 18002
connect
send I'm here, it's me client 2
wait 3000
recv
close