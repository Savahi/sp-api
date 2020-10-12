Sub Table
	oScriptControl = CreateObject("MSScriptControl.ScriptControl")
    oScriptControl.Language = "JScript"	
    oScriptControl.AddCode "function getVal(jsonObj, propertyName) { return (propertyName in jsonObj) ? jsonObj[propertyName] : """"; }"
	oScriptControl.AddCode "function getLength( jsonObj ) { return jsonObj.length; }"

	oRequest = CreateObject("WinHttp.WinHttpRequest.5.1")
	oRequest.Open "POST", "http://localhost:8080", false
	oRequest.SetTimeouts 0, 0, 0, 0 'wait infinitely\                                                                      

	oRequest.Send "{ ""command"":""openFile"", ""fileName"": ""C:\Program Files (x86)\Spider Project\Professional\Projects\01km_exp.008.sprj"" }"
    oResponse = oScriptControl.Eval("(" + oRequest.ResponseText + ")")    

	oRequest.Send "{ ""command"":""getTableHandle"", ""docHandle"":""" + oResponse.docHandle + """, ""table"":""GanttAct"" }"
    oResponse = oScriptControl.Eval("(" + oRequest.ResponseText + ")")    

	oRequest.Send "{ ""command"":""getTable"", ""tableHandle"":""" + oResponse.tableHandle + """ }"
    oResponse = oScriptControl.Eval("(" + oRequest.ResponseText + ")")    

	oArray = oScriptControl.RUN("getVal", oResponse, "array")
	arrayLength = oScriptControl.RUN("getLength", oArray)
	FOR i = 0 TO arrayLength-1
		oRow = oScriptControl.RUN("getVal", oArray, i)
		durSum = oScriptControl.RUN("getVal", oRow, "DurSum")
		'Sheet1.Cells(i,2).value = durSum
		oSheet = ThisComponent.Sheets.getByName("Sheet1")
		oCell = oSheet.getCellByPosition(1,i)
		oCell.value = durSum
	NEXT i		
End Sub