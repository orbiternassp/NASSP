
-- AGC Decoder LUA MFD module

require "math"
require "bit"

-- local initconnection = require("luadebugger")
-- initconnection(localhost, 10000, "luaidekey")

vesselname = 'AS-205'
v = {}
Prog = {}
Verb = {}
Noun = {}
Reg1 = {}
Reg2 = {}
Reg3 = {}

agcmode = {}
agcmode.progpressed = false
agcmode.oldtime = 0
agcmode.delta = 0
agcmode.index = 1
agcmode.flash = 0
agcmode.t0 = 0

p22 = {}
p22.stage = 0

-- Connect to vessel
v = {}
-- v = vessel.get_interface(vesselname)
v = vessel.get_focusinterface()

-- Frontend menu list
page = 1
t_warn = nil
apollo_agc_mode = {}
apollo_agc_mode[0] = "AGC decoder"
apollo_agc_mode[1] = "P22: Orbital Navigation Pr."
apollo_agc_mode[2] = "P23: Cislunar MDC Nav. Pr."
apollo_agc_mode[3] = "P27: AGC Update Program"
apollo_agc_mode[4] = "P52: Realign IMU"
apollo_agc_mode_idx = 0
hilight = 70*65536 + 70*256 + 70
str = nil
landmark = nil

errors = {}
dsky = {}
treg = {}
tarray = {}
range = {}

range["XXXXX"]  = "%s%s%s%s%s%s%s"
range["XXXXX."] = "%s%s%s%s%s%s."
range["XXXX.X"] = "%s%s%s%s%s.%s"
range["XXX.XX"] = "%s%s%s%s.%s%s"
range["XX.XXX"] = "%s%s%s.%s%s%s"
range["XX XX"]  =  "%s%s%s%s %s%s"
range["XX."]  =  "%s%s%s%s%s%s."
range["XX.XX"]  =  "%s%s%s%s.%s%s"
range["XXX."]  =  "%s%s%s%s%s%s."
range["X"]      =  "%s%s%s%s%s%s%s"

Rate = {}

Rate.lbs = {0.4537, "kg"}
Rate.fps = {0.3048, "m/s"}
Rate.nmi = {1.8521, "km"}
Rate.minsec = {1,"m:s"}
Rate.deg = {1, "deg"}
Rate.G = {1, "G"}
Rate.null = {1, " "}
Rate.empty = {1, " "}

Dim = {}

for rindex, ki, be in pairs(Rate) do
	Dim[rindex] = {}
	Dim[rindex] = ki
	Dim[rindex][ki] = {}
	Dim[rindex][ki] = be
end

function load_errors()
	for linesx in io.lines("Config/MFD/agcerrors.csv") do
		for v1, z1 in string.gmatch(linesx, "([0-9]+);(.+);") do
			errors[tostring(v1)] = z1
		end
	end
end

function load_dsky()
	for linesx in io.lines("Config/MFD/dsky.csv") do
		for v, z1, z2, z3, z4, z5, z6, z7 in string.gmatch(linesx, "([0-9]+);(.*);(.+);(.*);(.*);(.*);(.*);(.*)") do
			dsky[tostring(v)] = {}
			dsky[tostring(v)]["Description"] = z1
			dsky[tostring(v)]["Label1"] = z2
			dsky[tostring(v)]["Range1"] = z3
			dsky[tostring(v)]["Label2"] = z4
			dsky[tostring(v)]["Range2"] = z5
			dsky[tostring(v)]["Label3"] = z6
			dsky[tostring(v)]["Range3"] = z7
		end
	end
end

function setup(w,h)
  dispw = w
  disph = h
  ch=1
  cw=1
end

-- List of MFD buttons and menu entries for all pages

btn_label_1 = {'SEL','DWN',' UP'}
btn_menu_1 = {
  {l1='Run selected pr.',sel='s'},
  {l1='Cursor down',sel='d'},
  {l1='Cursor up',sel='u'}
}

btn_label_2 = {'LMC','NXT','RTN'}
btn_menu_2 = {
  {l1='Load landmark',sel='l'},
  {l1='Next step',sel='n'},
  {l1='Return to main',sel='d'}
}

btn_label_3 = {'BCK'}
btn_menu_3 = {
  {l1='Main menu',sel='b'}
}

function buttonlabel(bt)
  if page == 1 then
    if bt < 3 then
      return btn_label_1[bt+1]
    end
  elseif page == 2 then
    if bt < 3 then
      return btn_label_2[bt+1]
    end
  elseif page == 3 then
	if bt < 1 then
		return btn_label_3[bt+1]
	end
  end
  return nil
end

function buttonmenu ()
  if page == 1 then
    return btn_menu_1,3
  elseif page == 2 then
    return btn_menu_2,3
  elseif page == 3 then
	return btn_menu_3,1
  end
end

function update(skp)
  mfd:set_title(skp,'Apollo Guidance Computer MFD')
  skp:set_font(mfd:get_defaultfont(0))
  skp:set_textcolor(256*255)
  ch,cw = skp:get_charsize()
  --debout(tostring(page)..' '..tostring(apollo_agc_mode_idx))
  FAILREG0 = 253
  error_index = string.format("%o",readmem(FAILREG0))
  --debout(string.format("%s  %s", error_index, errors[error_index]))
  stat, errormsg = pcall(update_func[page],skp)
  if not stat then
	debout(errormsg)
  else
  	debout("-")
  end
  return true
end

-- Page 1: MFD main page

function consumebutton1(bt,event)
  if bt==0 then
    if event%PANEL_MOUSE.LBPRESSED == PANEL_MOUSE.LBDOWN then
      setpage_load(apollo_agc_mode_idx)
    end
    return true
  elseif bt==1 then
    if event%PANEL_MOUSE.LBPRESSED == PANEL_MOUSE.LBDOWN then
		setpage_down()
    end
    return true
  elseif bt==2 then
    if event%PANEL_MOUSE.LBPRESSED == PANEL_MOUSE.LBDOWN then
		setpage_up()
    end
    return true
  end
  return false
end

function consumekeybuffered1(key)
  if key==OAPI_KEY.S then
    setpage_load(apollo_agc_mode_idx)
    return true
  elseif key==OAPI_KEY.D then
    setpage_down()
    return true
  elseif key==OAPI_KEY.U then
    setpage_up()
    return true
  else
    return false
  end
end

function update1(skp)
  skp:text(cw,ch*2,'AGC Program modes',17)
  skp:set_backgroundmode(SKP.OPAQUE)
  skp:set_backgroundcolor(hilight)
  skp:text(cw,ch*(4+apollo_agc_mode_idx),'                            ',28)
  skp:set_backgroundmode(SKP.TRANSPARENT)
  skp:set_backgroundcolor(0)
  for ii=0,#apollo_agc_mode do
    t = apollo_agc_mode[ii]
    skp:text(cw,ch*(4+ii),t,#t)
  end
end

-- Page 2: MFD main page

function consumebutton2(bt,event)
  if bt==0 then
    if event%PANEL_MOUSE.LBPRESSED == PANEL_MOUSE.LBDOWN then
      setpage(2)
    end
    return true
  elseif bt==1 then
    if event%PANEL_MOUSE.LBPRESSED == PANEL_MOUSE.LBDOWN then
		setpage_next()
    end
    return true
  elseif bt==2 then
    if event%PANEL_MOUSE.LBPRESSED == PANEL_MOUSE.LBDOWN then
		setpage_return()
    end
    return true
  end
  return false
end

function consumekeybuffered2(key)
  if key==OAPI_KEY.M then
    setpage(2)
    return true
  elseif key==OAPI_KEY.N then
    setpage_next()
    return true
  elseif key==OAPI_KEY.R then
    setpage_return()
    return true
  else
    return false
  end
end

function update2(skp)
	skp:text(cw,ch*2,'AGC P22 Program',16)
	ddd()

	-- TODO Display layout
	skp:rectangle(cw,ch*4,cw*19,ch*13)
	skp:rectangle(cw,ch*14,cw*35,ch*17)
	skp:rectangle(cw,ch*18,cw*35,ch*22)
	skp:set_backgroundmode(SKP.OPAQUE)
	skp:set_backgroundcolor(0)
	skp:text(cw*11,ch*13.5,"ERROR STATUS",#"ERROR STATUS")
	skp:text(cw*10,ch*17.5,"PROGRAM STATUS",#"PROGRAM STATUS")

	FlashMask = 0x20
	OutChWord = bit.band(v:get_agcchannel(tonumber("11",8)), 0x7fff)
    Flash = bit.rshift(bit.band(OutChWord, FlashMask), 5)
	skp:text(cw*2,ch*5,"VERB",#"VERB")
	skp:text(cw*8,ch*5,"NOUN",#"NOUN")
	skp:text(cw*14,ch*5,"PROG",#"PROG")
end

function consumebutton3(bt,event)
  if bt==0 then
    if event%PANEL_MOUSE.LBPRESSED == PANEL_MOUSE.LBDOWN then
      setpage(1)
    end
    return true
  end
  return false
end

function consumekeybuffered3(key)
  if key==OAPI_KEY.N then
    setpage(1)
    return true
  else
    return false
  end
end

function rdisplay (skp)

--TODO trigger of display!!!
	treg.reg1 = Reg1
	treg.reg2 = Reg2
	treg.reg3 = Reg3
	
	h = 8
	desclength = string.len(dsky[noun]["Description"])
	max = 34
	start = -max + 1
	final = 1
	i = 0
	descdisp = string.format("%s:", dsky[noun]["Description"])
	while final < desclength do
		start = start + max
		final = start + max - 1
		if final > desclength then
			final = desclength
		end
		skp:text(cw*2, ch*(h+i*1.2), string.sub(descdisp, start, final), final-start+1)		
		i = i + 1
	end	
	
	for j, Reg in pairs(treg) do
		if j == "reg1" then
			h = 10.2
			Range = dsky[noun]["Range1"]
			Label = dsky[noun]["Label1"]
		end
		if j == "reg2" then
			h = 11.4
			Range = dsky[noun]["Range2"]
			Label = dsky[noun]["Label2"]
		end
		if j == "reg3" then
			h = 12.6
			Range = dsky[noun]["Range3"]
			Label = dsky[noun]["Label3"]
		end
		--outstr = noun.." "..Range.." "..Label.." "..j
		--return (outstr)
		for form, dim in string.gmatch(Range,"([\.X1 ]+)(.*)") do
			reg = tonumber(Reg)
			if dim == "" then
				dim = "empty"
			end
			if reg ~= nil then
				reg_conv = reg * Dim[dim][1]
				reg = string.format("%6.0f", reg_conv)
			else
				reg = Reg
			end
			ind = 1
			for c in string.gmatch(reg,".") do
				tarray[ind] = c
				ind = ind + 1
			end
			label = string.sub(Label,1,19)
			if dsky[noun] == nil then
				debout(form.." "..dim.." "..noun.." "..label.." "..j)
			else
				rdisp = string.format("%19s: "..range[form].." [%s]", label, tarray[1], tarray[2], tarray[3], tarray[4], tarray[5], tarray[6], Dim[dim][2])
				-- term.out(rdisp)
				skp:text(cw*2, ch*h, rdisp, string.len(rdisp))
			end
		end
	end
end
function update3(skp)
	skp:text(cw,ch*2,'AGC Decoder',12)
	ddd()

	-- TODO Display layout

	skp:rectangle(cw,ch*4,cw*37,ch*14)
	skp:rectangle(cw,ch*15,cw*37,ch*22)
	--skp:rectangle(cw,ch*18,cw*37,ch*22)
	skp:set_backgroundmode(SKP.OPAQUE)
	skp:set_backgroundcolor(0)
	skp:text(cw*11,ch*14.5,"ERROR STATUS",#"ERROR STATUS")
	-- skp:text(cw*10,ch*17.5,"PROGRAM STATUS",#"PROGRAM STATUS")

	FlashMask = 0x20
	OutChWord = bit.band(v:get_agcchannel(tonumber("11",8)), 0x7fff)
    Flash = bit.rshift(bit.band(OutChWord, FlashMask), 5)
	skp:text(cw*2,ch*5,"VERB",#"VERB")
	skp:text(cw*8,ch*5,"NOUN",#"NOUN")
	skp:text(cw*14,ch*5,"PROG",#"PROG")
	-- Verb
	if Verb == nil then
		skp:line(cw*4,ch*7.5,cw*6,ch*7.5)
	else
		if Flash == 1 and agcmode.flash == -1 then
			skp:line(cw*4,ch*7.5,cw*6,ch*7.5)
			--agcmode.flash = bit.bnot(agcmode.flash)
		else
			skp:text(cw*4,ch*6.5,string.format("%02.0f",Verb),string.len(string.format("%02.0f",Verb)))
			--agcmode.flash = bit.bnot(agcmode.flash)
		end
	end
	-- Noun
	if Noun == nil then
		skp:line(cw*10,ch*7.5,cw*12,ch*7.5)
	else
		if Flash == 1 and agcmode.flash == -1 then
			skp:line(cw*10,ch*7.5,cw*12,ch*7.5)
			agcmode.flash = bit.bnot(agcmode.flash)
		else
			skp:text(cw*10,ch*6.5,string.format("%02.0f",Noun),string.len(string.format("%02.0f",Noun)))
			agcmode.flash = bit.bnot(agcmode.flash)
		end
	end
	-- Prog
	if Prog == nil then
		skp:line(cw*16,ch*7.5,cw*18,ch*7.5)
	else
		skp:text(cw*16,ch*6.5,string.format("%02.0f",Prog),string.len(string.format("%02.0f",Prog)))
	end

	-- Registers
	if (Verb == 16 or Verb == 6) and Noun ~= nil and Verb ~=nil then
		noun = tostring(Noun)
		rdisplay(skp)
		--stat, errormsg = pcall(rdisplay,skp)
		--if not stat then
		--	debout(errormsg)
		--end
	end

	h = 15.6
	errdisp = string.format("%s: %s", error_index, errors[error_index])
	errlength = string.len(errdisp)
	max = 34
	start = -max + 1
	final = 1
	i = 0
	while final < errlength do
		start = start + max
		final = start + max - 1
		if final > errlength then
			final = errlength
		end
		skp:text(cw*2, ch*(h+i*1.2) , string.sub(errdisp, start, final), final-start+1)
		i = i + 1
	end	
end



--[[
    skp:text(cw*2,ch*8,"REG1",#"REG1")
	skp:text(cw*2,ch*9.5,"REG2",#"REG2")
	skp:text(cw*2,ch*11,"REG3",#"REG3")
	if Reg1 == nil then
		skp:line(cw*8,ch*9,cw*13,ch*9)
	else
		skp:text(cw*8,ch*8,string.format("%+06.0f",Reg1),string.len(string.format("%+06.0f",Reg1)))
	end
	if Reg2 == nil then
		skp:line(cw*8,ch*10.5,cw*13,ch*10.5)
	else
		skp:text(cw*8,ch*9.5,string.format("%+06.0f",Reg2),string.len(string.format("%+06.0f",Reg2)))
	end
	if Reg3 == nil then
		skp:line(cw*8,ch*12,cw*13,ch*12)
	else
		skp:text(cw*8,ch*11,string.format("%+06.0f",Reg3),string.len(string.format("%+06.0f",Reg3)))
	end

end
--]]

function setpage_p22 ()
	p22.command = "V37E22E"
	p22.stage = 2
	landmark = nil
end

function setpage(p)
  page = p
  mfd:invalidate_buttons()
  mfd:invalidate_display()
end

function setpage_load(index)
	if index == 0 then
		setpage(3)
	elseif index == 1 then
		setpage(2)
		oapi.open_inputbox("Input Landmark mame")
		setpage_p22()
	end
	return
end

function setpage_down()
	if apollo_agc_mode_idx < #apollo_agc_mode then
		apollo_agc_mode_idx = apollo_agc_mode_idx+1
		mfd:invalidate_display()
	end
end

function setpage_up()
	if apollo_agc_mode_idx > 0 then
        apollo_agc_mode_idx = apollo_agc_mode_idx-1
        mfd:invalidate_display()
	end
end


function setpage_next()
	p22.stage = p22.stage + 1
end

function setpage_return()
	setpage(1)
end


function consumebutton(bt,event)
  return consumebutton_func[page](bt,event)
end

function consumekeybuffered(key)
  return consumekeybuffered_func[page](key)
end

function debout (str)
	if type(str) == nil then
		return
	else
		oapi.dbg_out(str)
	end
	return
end

function readmem (addr)
	return v:get_agcerasable(math.modf(addr/256), bit.band(addr,0xff))
end


function ddd()
	BitDec = {}
	for j=0,511 do
		BitDec[tostring(j)] = " "
	end
	BitDec["21"] = "0"
	BitDec["3"]  = "1"
	BitDec["25"] = "2"
	BitDec["27"] = "3"
	BitDec["15"] = "4"
	BitDec["30"] = "5"
	BitDec["28"] = "6"
	BitDec["19"] = "7"
	BitDec["29"] = "8"
	BitDec["31"] = "9"
	BitDec["0"]  = " "

	SignDec = {}
	SignDec["0"] = " "
	SignDec["1"] = "+"
	SignDec["2"] = "-"
	SignDec["3"] = " "
	DSPTAB = 531

	SignMask = 0x400
	HiMask = 0x3e0
	LowMask = 0x1f

	FLAGWORD4 = bit.band(readmem(DSPTAB), 0x7ff)
	MRKNVFLG = bit.band(bit.rshift(FLAGWORD4, 8),1)
	NRMNVFLG = bit.band(bit.rshift(FLAGWORD4, 9),1)
	PRONVFLG = bit.band(bit.rshift(FLAGWORD4, 10),1)

	--debout(MRKNVFLG..' '..NRMNVFLG..' '..PRONVFLG)

	-- if NRMNVFLG == 1 or PRONVFLG == 1 then
	--	return
	-- end
--  -R3S	R3D4	R3D5
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    MR3S = bit.band(bit.rshift(bit.band(DownLinkWord, SignMask), 10),1)
    R3D4 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    R3D5 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  +R3S	R3D2	R3D3
    DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    PR3S = bit.band(bit.rshift(bit.band(DownLinkWord, SignMask), 10),1)
    R3D2 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    R3D3 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

-- 	R2D5	R3D1
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    R2D5 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    R3D1 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  -R2S	R2D3	R2D4
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    MR2S = bit.band(bit.rshift(bit.band(DownLinkWord, SignMask), 10),1)
    R2D3 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    R2D4 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  +R2S	R2D1	R2D2
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    PR2S = bit.band(bit.rshift(bit.band(DownLinkWord, SignMask), 10),1)
    R2D1 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    R2D2 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  -R1S	R1D4	R1D5
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    MR1S = bit.band(bit.rshift(bit.band(DownLinkWord, SignMask), 10),1)
    R1D4 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    R1D5 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  +R1S	R1D2	R1D3
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    PR1S = bit.band(bit.rshift(bit.band(DownLinkWord, SignMask), 10),1)
    R1D2 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    R1D3 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  R1D1
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    R1D1 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  ND1	ND2
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    ND1 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    ND2 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  VD1	VD2
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    VD1 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    VD2 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
    DSPTAB = DSPTAB + 1

--  MD1	MD2
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    MD1 = BitDec[tostring(bit.rshift(bit.band(DownLinkWord, HiMask), 5))]
    MD2 = BitDec[tostring(bit.band(DownLinkWord, LowMask))]
	DSPTAB = DSPTAB + 1

--  NA GA TR PC
	DownLinkWord = bit.band(readmem(DSPTAB), 0x7ff)
    NA = bit.rshift(bit.band(DownLinkWord, 0x8),3)
    GA = bit.rshift(bit.band(DownLinkWord, 0x20),5)
    TR = bit.rshift(bit.band(DownLinkWord, 0x80),7)
    PC = bit.rshift(bit.band(DownLinkWord, 0x100),8)

	SIGN3 = SignDec[tostring(bit.band(bit.bor(bit.lshift(MR3S,1),PR3S),0x3))]
	SIGN2 = SignDec[tostring(bit.band(bit.bor(bit.lshift(MR2S,1),PR2S),0x3))]
	SIGN1 = SignDec[tostring(bit.band(bit.bor(bit.lshift(MR1S,1),PR1S),0x3))]


	Verb = tonumber(VD1..VD2)
	Noun = tonumber(ND1..ND2)
	Prog = tonumber(MD1..MD2)
	Reg1 = R1D1..R1D2..R1D3..R1D4..R1D5
	Reg2 = tonumber(R2D1..R2D2..R2D3..R2D4..R2D5)
	Reg3 = R3D1..R3D2..R3D3..R3D4..R3D5

	if SIGN1 ~= " " then
		if SIGN1 == "-" then
			Reg1 = SIGN1..R1D1..R1D2..R1D3..R1D4..R1D5
		end
	end

	if SIGN2 ~= " " then
		if SIGN2 == "-" then
			Reg2 = tonumber(SIGN2..R2D1..R2D2..R2D3..R2D4..R2D5)
		end
	end

	if SIGN3 ~= " " then
		if SIGN3 == "-" then
			Reg3 = SIGN3..R3D1..R3D2..R3D3..R3D4..R3D5
		end
	end
	--oapi.dbg_out('type: '..type(SIGN1)..'; value: '..SIGN1..' type: '..type(SIGN2)..'; value: '..SIGN2..' type: '..type(SIGN3)..'; value: '..SIGN3)
	--oapi.dbg_out(bit.band(bit.bor(bit.lshift(MR2S,1),PR2S),0x3))
	--mfd:invalidate_display()
	--mfd:invalidate_button()
	return
--]]
end

load_errors()
load_dsky()


update_func = {
  update1,
  update2,
  update3
}

consumebutton_func = {
  consumebutton1,
  consumebutton2,
  consumebutton3
}

consumekeybuffered_func = {
  consumekeybuffered1,
  consumekeybuffered2,
  consumekeybuffered3
}
