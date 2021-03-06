local tipUtil = XLGetObject("API.Util")
local tFunHelper = XLGetGlobal("DiDa.FunctionHelper")
local Helper = XLGetGlobal("Helper")

function OnEnableChangeCtrol(self, isEnable)
	if self:GetClass() == "TextObject" then
		if isEnable then
			self:SetTextColorResID("262624")
		else
			self:SetTextColorResID("999999")
		end
		return
	elseif self:GetClass() == "EditObject" then
		if isEnable then
			self:SetTextColorID("262624")
		else
			self:SetTextColorID("999999")
		end
		return
	end
	local attr = self:GetAttribute()
	local text = self:GetControlObject("text")
	if text then
		if not attr.NormalTextColor and not isEnable then
			attr.NormalTextColor = text:GetTextColorResID()
		end
		if isEnable then
			text:SetTextColorResID(attr.NormalTextColor)
		else
			text:SetTextColorResID("999999")
		end
	end
	local edit = self:GetControlObject("edit")
	if edit then
		if not attr.NormalEditColor and not isEnable then
			attr.NormalEditColor = edit:GetTextColorID()
		end
		if isEnable then
			edit:SetTextColorID(attr.NormalEditColor)
		else
			edit:SetTextColorID("999999")
		end
	end
	local button = self:GetControlObject("button")
	if button then
		
	end
end

function OnFocusChangeEdit(self, isfocus)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	local bkg = owner:GetControlObject("bkg")
	if isfocus then
		bkg:SetTextureID(attr.HoverBkgID)
	else
		bkg:SetTextureID(attr.NormalBkgID)
		OnChangeEdit(self, true)
	end
end

function OnControlMouseLeaveEdit(self)
	--local edit = self:GetControlObject("edit")
	--edit:SetFocus(false)
end

function StringFormat02d(str)
	if str == nil then return "" end
	local nText = tonumber(str)
	if type(nText) == "number" then
		return string.format("%02d", nText)
	end
	return ""
end

function OnInitControlEdit(self)
	local attr = self:GetAttribute()
	local bkg = self:GetControlObject("bkg")
	bkg:SetTextureID(attr.NormalBkgID)
	bkg:SetObjPos(0, 0, attr.ImgWidth, "father.height")
	local txt = self:GetControlObject("text")
	txt:SetObjPos2(attr.ImgWidth+5, "(father.height-"..attr.ImgHeight..")/2", attr.ImgWidth, attr.ImgHeight)
	txt:SetText(attr.Text)
	if tFunHelper.IsUACOS() then
		txt:SetTextFontResID("font.text13.xp")
	end
	local edit = self:GetControlObject("edit")
	edit:SetMaxLength(attr.LimitLen)
	if type(attr.DefaultText) == "string" and  attr.DefaultText ~= "" then
		edit:SetText(StringFormat02d(attr.DefaultText))
	else
		edit:SetText(StringFormat02d(attr.MinNumber))
	end
end

function OnChangeEdit(self, bForce)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	local text = self:GetText()
	if not bForce and self:GetMaxLength() > string.len(text) then
		return
	end
	local nText = tonumber(text)
	self:SetText(StringFormat02d(nText))
	if type(attr.MaxNumber) == "number" and type(nText) == "number" and nText > attr.MaxNumber then
		self:SetText(StringFormat02d(attr.MaxNumber))
	end
	if type(attr.MinNumber) == "number" and type(nText) == "number" and nText < attr.MinNumber then
		self:SetText(StringFormat02d(attr.MinNumber))
	end
	local owner = self:GetOwnerControl()
	owner:FireExtEvent("OnChange")
end

function SetEditText(self, text)
	local edit = self:GetControlObject("edit")
	edit:SetText(StringFormat02d(text))
end

function OnInitSimpleCheckBox(self)
	local attr = self:GetAttribute()
	local button = self:GetControlObject("button")
	local text = self:GetControlObject("text")
	local btnattr = button:GetAttribute()
	btnattr.NormalBkgID = attr.UnCheckBkgID
	btnattr.HoverBkgID = attr.UnCheckBkgID
	btnattr.DownBkgID = attr.UnCheckBkgID
	btnattr.DisableBkgID = attr.UnCheckBkgID
	button:Updata()
	button:SetObjPos2(0, "(father.height-"..attr.ImgHeight..")/2", attr.ImgWidth, attr.ImgHeight)
	text:SetText(attr.Text)
	if tFunHelper.IsUACOS() then
		text:SetTextFontResID("font.text13.xp")
	end
	local nNeedLen = text:GetTextExtent()
	text:SetObjPos2(attr.ImgWidth+attr.Space, 0, nNeedLen, "father.height")
end

function OnClickSimpleCheckBox(self)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	--owner:SetCheck(not attr.select)
	owner:FireExtEvent("OnClick", not attr.select)
end

function SetCheck(self, bCheck)
	local button = self:GetControlObject("button")
	local btnattr = button:GetAttribute()
	local attr = self:GetAttribute()
	if bCheck then
		attr.select = true
		btnattr.NormalBkgID = attr.CheckBkgID
		btnattr.HoverBkgID = attr.CheckBkgID
		btnattr.DownBkgID = attr.CheckBkgID
		btnattr.DisableBkgID = attr.CheckBkgID
	else
		attr.select = false
		btnattr.NormalBkgID = attr.UnCheckBkgID
		btnattr.HoverBkgID = attr.UnCheckBkgID
		btnattr.DownBkgID = attr.UnCheckBkgID
		btnattr.DisableBkgID = attr.UnCheckBkgID
	end
	button:Updata()
end

function OnInitSimpleTitleCtrl(self)
	local attr = self:GetAttribute()
	local text = self:GetControlObject("text")
	local edit = self:GetControlObject("edit")
	text:SetTextColorResID(attr.TextColor)
	text:SetTextFontResID(attr.TextFont)
	edit:SetTextColorID(attr.TextColor)
	edit:SetFontID(attr.TextFont)
	text:SetText(attr.Text)
	edit:SetText(attr.Text)
	SwichShowTitleCtrl(self, false)
end

function SwichShowTitleCtrl(self, bEdit)
	local text = self:GetControlObject("text")
	local edit = self:GetControlObject("edit")
	if bEdit then
		local strText = text:GetText()
		edit:SetText(strText)
		text:SetVisible(false)
		edit:SetVisible(true)
		--edit:FireExtEvent("OnLButtonDown", 1, 1)
		edit:SetSelNone()
		
		local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
		timerManager:SetTimer(function(item, id)
			item:KillTimer(id)
			edit:SetSelAll()
			edit:SetFocus(true)
		end, 50)
	else
		local strText = edit:GetText()
		text:SetText(strText)
		edit:SetVisible(false)
		text:SetVisible(true)
		local attr = self:GetAttribute()
		attr.Text = strText
	end
end

function OnLButtonDownSimpleTitleCtrl(self)
	SwichShowTitleCtrl(self, true)
end

function OnControlFocusChangeSTC(self, bfocus)
	if not bfocus then
		SwichShowTitleCtrl(self, false)
	end
end

function SetText(self, text)
	local attr = self:GetAttribute()
	attr.Text = text
	local text = self:GetControlObject("text")
	local edit = self:GetControlObject("edit")
	text:SetText(attr.Text)
	edit:SetText(attr.Text)
end

function OnInitControlRemindPanle(self)
	local RemindTimeRightWeekCk = self:GetControlObject("RemindTimeRightWeekCk")
	RemindTimeRightWeekCk:SetVisible(false)
	RemindTimeRightWeekCk:SetChildrenVisible(false)
	--local CheckBoxBubble = self:GetControlObject("CheckBoxBubble")
	--CheckBoxBubble:SetCheck(true)
	local CheckBoxNA = self:GetControlObject("CheckBoxNA")
	OnClickCheckBox1(CheckBoxNA)
end

local tCheckBox1 = {CheckBoxNA=1, CheckBoxBubble=1}
local tCheckBox2 = {CheckBoxOnce=1, CheckBoxEveryDay=1, CheckBoxEveryWeek=1, CheckBoxEveryMonth=1}
local tCheckBox3 = {CheckBoxMonday=2, CheckBoxTuesday=3, CheckBoxWednesday=4, CheckBoxThursday=5, CheckBoxFriday=6, CheckBoxSaturday=7, CheckBoxSunday=1}
function SingleCheckBox(self, tCheckBox, strID)
	local owner = self:GetOwnerControl()
	if type(tCheckBox) ~= "table" then
		return
	end
	local obj
	if tCheckBox[strID] then
		for strOther, _ in pairs(tCheckBox) do
			if strOther ~= strID then
				obj = owner:GetControlObject(strOther)
				obj:SetCheck(false)
			else
				obj = owner:GetControlObject(strOther)
				obj:SetCheck(true)
			end
		end
	end
end

function OnClickCheckBox1(self)
	local id = self:GetID()
	local owner = self:GetOwnerControl()
	SingleCheckBox(self, tCheckBox1, id)
	SingleCheckBox(self, tCheckBox2, id)
	--SingleCheckBox(self, tCheckBox3, id)
	if tCheckBox2[id] or tCheckBox3[id] then
		SingleCheckBox(self, tCheckBox1, "CheckBoxBubble")
	end
	if tCheckBox3[id] then
		local attr = self:GetAttribute()
		self:SetCheck(not attr.select)
	elseif id == "CheckBoxOnce" or id == "CheckBoxEveryMonth" then--一次性提醒、每月
		local RemindTimeRightWeekCk = owner:GetControlObject("RemindTimeRightWeekCk")
		RemindTimeRightWeekCk:SetVisible(false)
		RemindTimeRightWeekCk:SetChildrenVisible(false)
		local RemindTimeRightTimeEdit = owner:GetControlObject("RemindTimeRightTimeEdit")
		RemindTimeRightTimeEdit:SetVisible(true)
		RemindTimeRightTimeEdit:SetChildrenVisible(true)
		if id == "CheckBoxEveryMonth" then
			ShowYearMonthEdit(RemindTimeRightTimeEdit, true)
		else
			ShowYearMonthEdit(RemindTimeRightTimeEdit, false)
		end
	elseif id == "CheckBoxEveryDay" or id == "CheckBoxEveryWeek" then--每日每周
		local RemindTimeRightWeekCk = owner:GetControlObject("RemindTimeRightWeekCk")
		RemindTimeRightWeekCk:SetVisible(true)
		RemindTimeRightWeekCk:SetChildrenVisible(true)
		local RemindTimeRightTimeEdit = owner:GetControlObject("RemindTimeRightTimeEdit")
		RemindTimeRightTimeEdit:SetVisible(false)
		RemindTimeRightTimeEdit:SetChildrenVisible(false)
		if id == "CheckBoxEveryDay" then
			ShowPerDayEdit(RemindTimeRightWeekCk, true)
		else
			ShowPerDayEdit(RemindTimeRightWeekCk, false)
			SingleCheckBox(self, tCheckBox3, "CheckBoxMonday")--默认单选周一
		end
	end
end

function ShowPerDayEdit(RemindTimeRightWeekCk, bOnlyShowPerDay)
	local owner = RemindTimeRightWeekCk:GetOwnerControl()
	local EditWeekLayout, EditTimeLayout = owner:GetControlObject("EditWeekLayout"), owner:GetControlObject("EditTimeLayout")
	if bOnlyShowPerDay then
		EditWeekLayout:SetVisible(false)
		EditWeekLayout:SetChildrenVisible(false)
		EditTimeLayout:SetVisible(true)
		EditTimeLayout:SetChildrenVisible(true)
		EditTimeLayout:SetObjPos2(0, 0, "father.width", 22)
	else
		EditWeekLayout:SetVisible(true)
		EditWeekLayout:SetChildrenVisible(true)
		EditTimeLayout:SetVisible(true)
		EditTimeLayout:SetChildrenVisible(true)
		EditTimeLayout:SetObjPos2(0, 34, "father.width", 22)
	end
	UpdateData2UI(owner)
end

function ShowYearMonthEdit(RemindTimeRightTimeEdit, bOnlyShowDayHour)
	local owner = RemindTimeRightTimeEdit:GetOwnerControl()
	local EditYearMonthLayout, EditDayHourMinLayout = owner:GetControlObject("EditYearMonthLayout"), owner:GetControlObject("EditDayHourMinLayout")
	if bOnlyShowDayHour then
		EditYearMonthLayout:SetVisible(false)
		EditYearMonthLayout:SetChildrenVisible(false)
		EditDayHourMinLayout:SetVisible(true)
		EditDayHourMinLayout:SetChildrenVisible(true)
		EditDayHourMinLayout:SetObjPos2(0, 0, "father.width-118", 22)
	else
		EditYearMonthLayout:SetVisible(true)
		EditYearMonthLayout:SetChildrenVisible(true)
		EditDayHourMinLayout:SetVisible(true)
		EditDayHourMinLayout:SetChildrenVisible(true)
		EditDayHourMinLayout:SetObjPos2(118, 0, "father.width-118", 22)
	end
	UpdateData2UI(owner)
end

function UpdateData2UI(self)
	local attr = self:GetAttribute()
	local data = attr.data or {}
	local EditHourWeek = self:GetControlObject("EditHourWeek")
	local EditMinuteWeek = self:GetControlObject("EditMinuteWeek")
	local y, mon, d, h, m = tipUtil:FormatCrtTime(tipUtil:GetCurrentUTCTime() or 0)
	EditHourWeek:SetEditText(data["hour"] or h)
	EditMinuteWeek:SetEditText(data["min"] or m)
	local dc = self:GetControlObject("EditDayTimeEdit")
	local hc = self:GetControlObject("EditHourTimeEdit")
	local minc = self:GetControlObject("EditMinuteTimeEdit")
	dc:SetEditText(data["day"] or d)
	hc:SetEditText(data["hour"] or h)
	minc:SetEditText(data["min"] or m)
end

function SwichShowSaveCancelBtn(self, bShow)
	local BottomSaveBtn = self:GetControlObject("BottomSaveBtn")
	local BottomCancelBtn = self:GetControlObject("BottomCancelBtn")
	if bShow then
		BottomSaveBtn:Show(true)
		BottomCancelBtn:Show(true)
	else
		BottomSaveBtn:Show(false)
		BottomCancelBtn:Show(false)
	end
end

function ShowOnceTargetTime(owner, nTime)
	local EditYearTimeEdit = owner:GetControlObject("EditYearTimeEdit") 
	local EditMonthTimeEdit = owner:GetControlObject("EditMonthTimeEdit") 
	local EditDayTimeEdit = owner:GetControlObject("EditDayTimeEdit") 
	local EditHourTimeEdit = owner:GetControlObject("EditHourTimeEdit") 
	local EditMinuteTimeEdit = owner:GetControlObject("EditMinuteTimeEdit")
	local y, mon, d, h, m 
	nTime = tonumber(nTime)
	if type(nTime) ~= "number" then 
		y, mon, d, h, m = 2015, 1, 1, 0, 0
	else
		 y, mon, d, h, m = tipUtil:FormatCrtTime(nTime)
	end

	EditYearTimeEdit:SetEditText(y)
	EditMonthTimeEdit:SetEditText(mon)
	EditDayTimeEdit:SetEditText(d)
	EditHourTimeEdit:SetEditText(h)
	EditMinuteTimeEdit:SetEditText(m)
end

function GetSimpleTimeStr(nUtc)
	nUtc = tonumber(nUtc)
	if type(nUtc) ~= "number" then return "" end
	local y, mon, d, h, m = tipUtil:FormatCrtTime(nUtc)
	return ""..(y or "").."-"..string.format("%02d", (mon or "")).."-"..string.format("%02d", (d or "")).."  "..((h) or "")..":"..string.format("%02d", (m or ""))
end


function UpdateRemindList(self)
	local owner = self:GetOwnerControl()
	local remindlist = owner:GetControlObject("root.ctrl")
	local ReBuildRemindList = XLGetGlobal("ReBuildRemindList") 
	local SaveRemindListData2File = XLGetGlobal("SaveRemindListData2File")
	SaveRemindListData2File()
	ReBuildRemindList(remindlist)
end

--保存
function OnClickBottomSaveBtn(self)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	local data = attr.data
	--标题
	local titlectrl = owner:GetControlObject("TitleObj")
	local titlectrlattr = titlectrl:GetAttribute()
	data["title"] = titlectrlattr.Text
	--内容
	local CenterEditObj = owner:GetControlObject("CenterEditObj")
	data["content"] = Helper:UrlEncode(CenterEditObj:GetText())
	--无提醒选择框
	local CheckBoxNA = owner:GetControlObject("CheckBoxNA")
	local CheckBoxNAattr = CheckBoxNA:GetAttribute()
	data["bopen"] = not CheckBoxNAattr.select
	--提醒方式
	local CheckBoxOnce = owner:GetControlObject("CheckBoxOnce")
	local CheckBoxOnceattr = CheckBoxOnce:GetAttribute()
	local CheckBoxEveryDay = owner:GetControlObject("CheckBoxEveryDay")
	local CheckBoxEveryDayattr = CheckBoxEveryDay:GetAttribute()
	local CheckBoxEveryWeek = owner:GetControlObject("CheckBoxEveryWeek")
	local CheckBoxEveryWeekattr = CheckBoxEveryWeek:GetAttribute()
	local CheckBoxEveryMonth = owner:GetControlObject("CheckBoxEveryMonth")
	local CheckBoxEveryMonthattr = CheckBoxEveryMonth:GetAttribute()
	if CheckBoxOnceattr.select then
		data["ntype"] = 1
		data["type"] = "准时提醒"
	elseif CheckBoxEveryDayattr.select then
		data["ntype"] = 2
		data["type"] = "每日提醒"
	elseif CheckBoxEveryWeekattr.select then
		data["ntype"] = 3
		data["type"] = "每周提醒"
	elseif CheckBoxEveryMonthattr.select then
		data["ntype"] = 4
		data["type"] = "每月提醒"
	end
	
	--年
	--月
	--日
	--时
	--分
	if data["ntype"] == 1 then
		local yc = owner:GetControlObject("EditYearTimeEdit")
		local ycedit = yc:GetControlObject("edit")
		local mc = owner:GetControlObject("EditMonthTimeEdit")
		local mcedit = mc:GetControlObject("edit")
		local dc = owner:GetControlObject("EditDayTimeEdit")
		local dcedit = dc:GetControlObject("edit")
		local hc = owner:GetControlObject("EditHourTimeEdit")
		local hcedit = hc:GetControlObject("edit")
		local minc = owner:GetControlObject("EditMinuteTimeEdit")
		local mincedit = minc:GetControlObject("edit")
		local y = ycedit:GetText()
		local m = mcedit:GetText()
		local d = dcedit:GetText()
		local h = hcedit:GetText()
		local mi = mincedit:GetText()
		data["noncetargettime"] = tipUtil:DateTime2Seconds(y, m, d, h, mi, 0)
	elseif data["ntype"] == 2 then
		local EditHourWeek = owner:GetControlObject("EditHourWeek")
		local EditHourWeekedit = EditHourWeek:GetControlObject("edit")
		local EditMinuteWeek = owner:GetControlObject("EditMinuteWeek")
		local EditMinuteWeekedit = EditMinuteWeek:GetControlObject("edit")
		data["hour"] = EditHourWeekedit:GetText()
		data["min"] = EditMinuteWeekedit:GetText()
	elseif data["ntype"] == 4 then --每月
		local dc = owner:GetControlObject("EditDayTimeEdit")
		local dcedit = dc:GetControlObject("edit")
		local hc = owner:GetControlObject("EditHourTimeEdit")
		local hcedit = hc:GetControlObject("edit")
		local minc = owner:GetControlObject("EditMinuteTimeEdit")
		local mincedit = minc:GetControlObject("edit")
		data["day"] = dcedit:GetText()
		data["hour"] = hcedit:GetText()
		data["min"] = mincedit:GetText()
	--周1-7
	--时
	--分
	else
		data["tweek"] = {}
		local obj, objattr
		for key, value in pairs(tCheckBox3) do
			obj = owner:GetControlObject(key)
			objattr = obj:GetAttribute()
			if objattr.select then
				data["tweek"][#data["tweek"]+1] = value
			end
		end
		local EditHourWeek = owner:GetControlObject("EditHourWeek")
		local EditHourWeekedit = EditHourWeek:GetControlObject("edit")
		local EditMinuteWeek = owner:GetControlObject("EditMinuteWeek")
		local EditMinuteWeekedit = EditMinuteWeek:GetControlObject("edit")
		data["hour"] = EditHourWeekedit:GetText()
		data["min"] = EditMinuteWeekedit:GetText()
	end
	owner:FireExtEvent("UpdateRemindList", data)
end

--取消 
function OnClickBottomCancelBtn(self)
	local owner = self:GetOwnerControl()
	local attr = owner:GetAttribute()
	owner:SetData(attr.data)
	tFunHelper.ChangeView(1)
end

function SetData(self, data)
	local titlectrl = self:GetControlObject("TitleObj")
	local RightTimeObj = self:GetControlObject("RightTimeObj")
	local CenterEditObj = self:GetControlObject("CenterEditObj")
	local attr = self:GetAttribute()
	if data == nil then
		OnInitControlRemindPanle(self)
		SwichShowSaveCancelBtn(self, false)
		titlectrl:SetText("标题：这个一定要填写")
		RightTimeObj:SetVisible(false)
		CenterEditObj:SetText("内容：可填可不填")
		self:SetEnable(false)
		self:SetChildrenEnable(false)
	else
		attr.data = data
		self:SetEnable(true)
		self:SetChildrenEnable(true)
		RightTimeObj:SetVisible(true)
		SwichShowSaveCancelBtn(self, true)
		if data["title"] == "新建提醒" then
			titlectrl:SetText("标题：这个一定要填写")
		else
			titlectrl:SetText(data["title"])
		end
		RightTimeObj:SetText("创建时间："..GetSimpleTimeStr(data["createtime"]))
		if not data["content"] or data["content"] == "" then
			CenterEditObj:SetText("内容：可填可不填")
		else
			CenterEditObj:SetText(Helper:UrlDecode(data["content"]) or "")
		end
		local CheckBoxOnce = self:GetControlObject("CheckBoxOnce")
		local CheckBoxEveryDay = self:GetControlObject("CheckBoxEveryDay")
		local CheckBoxEveryWeek = self:GetControlObject("CheckBoxEveryWeek")
		local CheckBoxEveryMonth = self:GetControlObject("CheckBoxEveryMonth")
		if data["ntype"] == 1 then
			OnClickCheckBox1(CheckBoxOnce)
			local nOnceTargetTime = data["noncetargettime"]
			ShowOnceTargetTime(self, nOnceTargetTime or tipUtil:GetCurrentUTCTime()) 
		elseif data["ntype"] == 2 then
			OnClickCheckBox1(CheckBoxEveryDay)
		elseif data["ntype"] == 3 then
			OnClickCheckBox1(CheckBoxEveryWeek)
		elseif data["ntype"] == 4 then
			OnClickCheckBox1(CheckBoxEveryMonth)
		end
		if data["ntype"] ~= 1 and type(data["tweek"]) == "table" then
				local obj
				for key, value in pairs(tCheckBox3) do
					obj = self:GetControlObject(key)
					obj:SetCheck(false)
					for i, idx in ipairs(data["tweek"]) do
						if value == idx then
							obj:SetCheck(true)
							break
						end
					end
				end
				local EditHourWeek = self:GetControlObject("EditHourWeek")
				local EditMinuteWeek = self:GetControlObject("EditMinuteWeek")
				local y, mon, d, h, m = tipUtil:FormatCrtTime(tipUtil:GetCurrentUTCTime() or 0)
				EditHourWeek:SetEditText(data["hour"] or h)
				EditMinuteWeek:SetEditText(data["min"] or m)
				local dc = self:GetControlObject("EditDayTimeEdit")
				local hc = self:GetControlObject("EditHourTimeEdit")
				local minc = self:GetControlObject("EditMinuteTimeEdit")
				dc:SetEditText(data["day"] or d)
				hc:SetEditText(data["hour"] or h)
				minc:SetEditText(data["min"] or m)
		end
		if not data["bopen"] then
			local CheckBoxNA = self:GetControlObject("CheckBoxNA")
			OnClickCheckBox1(CheckBoxNA)
		end
	end
end