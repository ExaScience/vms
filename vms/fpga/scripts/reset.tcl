puts  "Connecting"

connect -url tcp:zcu102-host:3121

puts "Reseting"

targets -set -nocase -filter {name =~ "*PSU*"}
stop
rst -system
after 2000

targets -set -nocase -filter {name =~ "*PMU*"}
stop
rst -system
after 2000

targets -set -nocase -filter {name =~ "*PSU*"}
stop
rst -system
after 2000
