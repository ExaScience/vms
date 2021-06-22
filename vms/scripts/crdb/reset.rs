send ""
send ""
print "Wait main"

expect {
    "MainMenu"
}

print send 03

send "03"

print "Wait Pin 02 - 1"

expect {
    "Pin: 02,    output enabled: 1"
}

print "send 02 - 1"
send "02"


print "Wait Pin 02 - 2"
expect {
    "Pin: 02,    output enabled: 0" 
}

print "send 02 - 2"
send "02"

print "Wait Pin 02 - 3"
expect {
    "Pin: 02,    output enabled: 1"
}

print send 
send ""

print "Wait MainMenu"
expect {
    "MainMenu"
}

! killall -9 minicom

