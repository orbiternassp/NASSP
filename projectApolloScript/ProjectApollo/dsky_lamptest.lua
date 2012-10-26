-- Sample script doing V35 "DSKY lamp test"
-- Type run("ProjectApollo/dsky_lamptest") to execute this script

v = vessel.get_focusinterface()
v:set_panelswitch("DskySwitchVerb",1)
proc.wait_simdt(1)
v:set_panelswitch("DskySwitchThree",1)
proc.wait_simdt(1)
v:set_panelswitch("DskySwitchFive",1)
proc.wait_simdt(1)
v:set_panelswitch("DskySwitchEnter",1)
