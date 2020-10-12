Sub ReportTotalCost
	oScriptControl = CreateObject("MSScriptControl.ScriptControl")
    oScriptControl.Language = "JScript"	
    oScriptControl.AddCode "function getValByKey(jsonObj, key) { return (key in jsonObj) ? jsonObj[key] : """"; }"
    oScriptControl.AddCode "function getValByIndex(jsonObj, index) { return (jsonObj.length > index) ? jsonObj[index] : """"; }"
	oScriptControl.AddCode "function getLength( jsonObj ) { return jsonObj.length; }"

	' Creating a Request object to obtain data
	oRequest = CreateObject("WinHttp.WinHttpRequest.5.1")
	oRequest.Open "POST", "http://localhost:8080", false
	oRequest.SetTimeouts 0, 0, 0, 0 'wait infinitely

	' 
	oRequest.Send "{ ""command"":""openFile"", ""fileName"": ""C:\Program Files (x86)\Spider Project\Professional\Projects\01km_exp.008.sprj"" }"	
    oResponse = oScriptControl.Eval("(" + oRequest.ResponseText + ")")    
    docHandle = oResponse.docHandle

	oRequest.Send "{ ""command"":""getObjectHandle"", ""docHandle"":""" + docHandle + """, ""table"":""GanttAct"", ""index""=""0"" }"
    oResponse = oScriptControl.Eval("(" + oRequest.ResponseText + ")")    
    phaseHandle = oResponse.objectHandle

	oRequest.Send "{ ""command"":""getObjectHandle"", ""docHandle"":""" + docHandle + """, ""table"":""ReportSpend"", ""code""=""TotalCost"" }"
    oResponse = oScriptControl.Eval("(" + oRequest.ResponseText + ")")    
	templateHandle = oResponse.objectHandle
	
	oRequest.Send "{ ""command"":""getReport"", ""objectHandle"":""" + phaseHandle + """, ""templateHandle"":""" + templateHandle + """ }"
    oResponse = oScriptControl.Eval("(" + oRequest.ResponseText + ")")    

	MsgBox oRequest.ResponseText
	oGraphs = oScriptControl.RUN("getValByKey", oResponse, "graphs") 	' Getting a list of graphs 
	oGraph = oScriptControl.RUN("getValByKey", oGraphs, 0)				' Getting the first graph in the list
	oArray = oScriptControl.RUN("getValByKey", oGraph, "array")			' Getting the array of values of hte first graph 
	arrayLength = oScriptControl.RUN("getLength", oArray)			' Getting the length of the array
	oSheet = ThisComponent.Sheets.getByName("Sheet1")				' Obtaining current sheet
	FOR i = 0 TO arrayLength-1										' Populating the sheet with the values of the array: "time", "cost"
		oRow = oScriptControl.RUN("getValByKey", oArray, i)
		timeInSec = oScriptControl.RUN("getValByIndex", oRow, 0)
		cost = oScriptControl.RUN("getValByIndex", oRow, 1)
		oCell = oSheet.getCellByPosition(1,i)
		oCell.value = timeInSec
		oCell = oSheet.getCellByPosition(2,i)
		oCell.value = cost
	NEXT i		
End Sub
